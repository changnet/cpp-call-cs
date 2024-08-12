using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Loader
{
    public class HotLoader
    {
        private struct PluginData
        {
            public string path;
            public AssemblyLoadContext loader;
            public Type type;
            public Object? ins;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PluginArgs
        {
            public IntPtr PluginPath;
            public IntPtr p1;
        }

        delegate void LogFuncDelegate([MarshalAs(UnmanagedType.LPUTF8Str)] string foo);

        private static LogFuncDelegate? logFunc = null;
        private static Dictionary<string, PluginData> md = new Dictionary<string, PluginData>();

        public static int setLogFunc(IntPtr arg, int argLength)
        {
            logFunc = (LogFuncDelegate)Marshal.GetDelegateForFunctionPointer(arg, typeof(LogFuncDelegate));
            return 0;
        }

        public static int Load(IntPtr arg, int argLength)
        {
            if (argLength < System.Runtime.InteropServices.Marshal.SizeOf(typeof(PluginArgs)))
            {
                Console.WriteLine($"plugin load invalid args");
                return 1;
            }
            PluginArgs args = Marshal.PtrToStructure<PluginArgs>(arg);

            string pluginPath = Marshal.PtrToStringUTF8(args.PluginPath) ?? "";
            string PluginClass = Marshal.PtrToStringUTF8(args.p1) ?? "";
            Console.WriteLine($"plugin load from {pluginPath}");

            if (logFunc != null)
            {
                string str = string.Format("plugin load from {0} ok", pluginPath);
                logFunc(str);
            }


            if (md.ContainsKey(pluginPath))
            {
                Console.WriteLine($"plugin load already exist {pluginPath}");
                return 1;
            }

            var loader = new AssemblyLoadContext(pluginPath, true);

            Stream fs = File.OpenRead(pluginPath);
            Assembly asm = loader.LoadFromStream(fs);
            fs.Close();
            if (asm == null)
            {
                Console.WriteLine("plugin load fail");
                return 1;
            }
            Type? type = asm.GetType(PluginClass);
            if (type == null)
            {
                Console.WriteLine("plugin type not found");
                return 1;
            }

            Object? ins = Activator.CreateInstance(type);

            var data = new PluginData();
            data.path = pluginPath;
            data.loader = loader;
            data.ins = ins;
            data.type = type;
            md.Add(pluginPath, data);

            Console.WriteLine("plugin load success!");

            return 0;
        }

        public static int Unload(IntPtr arg, int argLength)
        {
            if (argLength < System.Runtime.InteropServices.Marshal.SizeOf(typeof(PluginArgs)))
            {
                Console.WriteLine($"plugin load invalid args");
                return 1;
            }
            PluginArgs args = Marshal.PtrToStructure<PluginArgs>(arg);
            string pluginPath = Marshal.PtrToStringUTF8(args.PluginPath) ?? "";

            Console.WriteLine($"plugin unload from {pluginPath}");

            PluginData data;
            if (!md.TryGetValue(pluginPath, out data))
            {
                Console.WriteLine($"plugin unload no such plugin loaded {pluginPath}");
                return 1;
            }
            data.loader.Unload();
            md.Remove(pluginPath);
            return 0;
        }

        public delegate long CustomEntryPointDelegate(PluginArgs args);
        public static long Call(PluginArgs args)
        {
            string pluginPath = Marshal.PtrToStringUTF8(args.PluginPath) ?? "";
            string methodName = Marshal.PtrToStringUTF8(args.p1) ?? "";

            Console.WriteLine($"call {pluginPath} {methodName}");
            PluginData data;
            if (!md.TryGetValue(pluginPath, out data))
            {
                Console.WriteLine($"plugin call no such plugin loaded {pluginPath}");
                return 1;
            }

            var mi = data.type.GetMethod(methodName);
            if (mi == null)
            {
                Console.WriteLine($"plugin call no such method {pluginPath} {methodName}");
                return 1;
            }

            var r = mi.Invoke(data.ins, null);
            if (r == null)
            {
                return 0;
            }
            return (long)r;
        }

    }
}
