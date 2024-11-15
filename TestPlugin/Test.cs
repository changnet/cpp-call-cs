namespace TestPlugin
{
    public class Test
    {
        public string RunTest(string foo)
        {
            Console.WriteLine("plugin RunTest... {0}", foo);
            return "OK";
        }

        public string RunGcTest(string foo)
        {
            Console.WriteLine("plugin RunGcTest...");
            string ss = "";
            for (int i = 0; i < 10; i++)
            {
                string s = "gcgggggggggggggggggggggg" + i.ToString();
                ss = s;
            }
            GC.Collect();
            return ss;
        }
    }
}
