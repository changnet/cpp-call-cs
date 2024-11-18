// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// Standard headers
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Provided by the AppHost NuGet package and installed as an SDK pack
#include <nethost.h>

// Header files copied from https://github.com/dotnet/core-setup
#include <coreclr_delegates.h>
#include <hostfxr.h>

#ifdef WINDOWS
#include <Windows.h>

#define STR(s) L ## s
#define CH(c) L ## c
#define DIR_SEPARATOR L'\\'

#define string_compare wcscmp

#else
#include <dlfcn.h>
#include <limits.h>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#define string_compare strcmp

#endif

extern "C" {
	#include "malloc_hook.h"
}

using string_t = std::basic_string<char_t>;

namespace
{
    // Globals to hold hostfxr exports
    hostfxr_initialize_for_dotnet_command_line_fn init_for_cmd_line_fptr;
    hostfxr_initialize_for_runtime_config_fn init_for_config_fptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr;
    hostfxr_run_app_fn run_app_fptr;
    hostfxr_close_fn close_fptr;

    // Forward declarations
    bool load_hostfxr(const char_t *app);
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *assembly);

    int run_component_example(const string_t& root_path);
}

#if defined(WINDOWS)
int __cdecl wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    // Get the current executable's directory
    // This sample assumes the managed assembly to load and its runtime configuration file are next to the host
    char_t host_path[MAX_PATH];
#if WINDOWS
    auto size = ::GetFullPathNameW(argv[0], sizeof(host_path) / sizeof(char_t), host_path, nullptr);
    assert(size != 0);
#else
    auto resolved = realpath(argv[0], host_path);
    assert(resolved != nullptr);
#endif

    string_t root_path = host_path;
    auto pos = root_path.find_last_of(DIR_SEPARATOR);
    assert(pos != string_t::npos);
    root_path = root_path.substr(0, pos + 1);

    return run_component_example(root_path);
}

namespace
{
    int run_component_example(const string_t& root_path)
    {
        //
        // STEP 1: Load HostFxr and get exported hosting functions
        //
        if (!load_hostfxr(nullptr))
        {
            assert(false && "Failure: load_hostfxr()");
            return EXIT_FAILURE;
        }

        //
        // STEP 2: Initialize and start the .NET Core runtime
        //
        const string_t config_path = root_path + STR("Loader.runtimeconfig.json");
        load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
        load_assembly_and_get_function_pointer = get_dotnet_load_assembly(config_path.c_str());
        assert(load_assembly_and_get_function_pointer != nullptr && "Failure: get_dotnet_load_assembly()");

        //
        // STEP 3: Load managed assembly and get function pointer to a managed method
        //
        const string_t dotnetlib_path = root_path + STR("Loader.dll");
        const char_t *dotnet_type = STR("Loader.HotLoader, Loader");
		
		struct PluginArgs
        {
            const char *PluginPath;
            const char *p1;
			const char *p2;
        };
		typedef int64_t (CORECLR_DELEGATE_CALLTYPE* custom_entry_point_fn)(PluginArgs args);

        // <SnippetLoadAndGet>
        // Function pointer to managed delegate
        component_entry_point_fn load = nullptr;
        const char_t* load_method = STR("Load");
        int rc = load_assembly_and_get_function_pointer(
            dotnetlib_path.c_str(),
            dotnet_type,
            load_method,
            nullptr /*delegate_type_name*/,
            nullptr,
            (void**)&load);
        // </SnippetLoadAndGet>
        assert(rc == 0 && load != nullptr && "Failure: load_assembly_and_get_function_pointer()");

        component_entry_point_fn unload = nullptr;
        const char_t* unload_method = STR("Unload");
        rc = load_assembly_and_get_function_pointer(
            dotnetlib_path.c_str(),
            dotnet_type,
            unload_method,
            nullptr /*delegate_type_name*/,
            nullptr,
            (void**)&unload);
        // </SnippetLoadAndGet>
        assert(rc == 0 && unload != nullptr && "Failure: load_assembly_and_get_function_pointer()");

        custom_entry_point_fn call = nullptr;
        const char_t* call_method = STR("Call");
        rc = load_assembly_and_get_function_pointer(
            dotnetlib_path.c_str(),
            dotnet_type,
            call_method,
            STR("Loader.HotLoader+CustomEntryPointDelegate, Loader") /*delegate_type_name*/,
            nullptr,
            (void**)&call);
        // </SnippetLoadAndGet>
        assert(rc == 0 && call != nullptr && "Failure: load_assembly_and_get_function_pointer()");

        //
        // STEP 4: Run managed code
        //
        PluginArgs args
        {
            "TestPlugin.dll",
            "TestPlugin.Test",
			""
        };
        PluginArgs cargs
        {
            "TestPlugin.dll",
            "RunTest",
			"ttttttttttttttttttt"
        };
		PluginArgs gcargs
        {
            "TestPlugin.dll",
            "RunGcTest",
			"foo"
        };

        load((void *)&args, sizeof(PluginArgs));
		std::cout << "wait 10s to call test" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));
		call(gcargs);
        call(cargs);
        unload((void*)&args, sizeof(PluginArgs));

		std::cout << "wait 20s to update plugin" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20 * 1000));

        load((void*)&args, sizeof(PluginArgs));
        call(cargs);
		call(gcargs);
        unload((void*)&args, sizeof(PluginArgs));
 
        return EXIT_SUCCESS;
    }
}


/********************************************************************************************
 * Function used to load and activate .NET Core
 ********************************************************************************************/

namespace
{
    // Forward declarations
    void *load_library(const char_t *);
    void *get_export(void *, const char *);

#ifdef WINDOWS
    void *load_library(const char_t *path)
    {
        HMODULE h = ::LoadLibraryW(path);
        assert(h != nullptr);
        return (void*)h;
    }
    void *get_export(void *h, const char *name)
    {
        void *f = ::GetProcAddress((HMODULE)h, name);
        assert(f != nullptr);
        return f;
    }
#else
    void *load_library(const char_t *path)
    {
        void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
        assert(h != nullptr);
        return h;
    }
    void *get_export(void *h, const char *name)
    {
        void *f = dlsym(h, name);
        assert(f != nullptr);
        return f;
    }
#endif

    // <SnippetLoadHostFxr>
    // Using the nethost library, discover the location of hostfxr and get exports
    bool load_hostfxr(const char_t *assembly_path)
    {
        get_hostfxr_parameters params { sizeof(get_hostfxr_parameters), assembly_path, nullptr };
        // Pre-allocate a large buffer for the path to hostfxr
        char_t buffer[MAX_PATH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);
        int rc = get_hostfxr_path(buffer, &buffer_size, &params);
        if (rc != 0)
            return false;

        // Load hostfxr and get desired exports
        // NOTE: The .NET Runtime does not support unloading any of its native libraries. Running
        // dlclose/FreeLibrary on any .NET libraries produces undefined behavior.
        void *lib = load_library(buffer);
        init_for_cmd_line_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)get_export(lib, "hostfxr_initialize_for_dotnet_command_line");
        init_for_config_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
        get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
        run_app_fptr = (hostfxr_run_app_fn)get_export(lib, "hostfxr_run_app");
        close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

        return (init_for_config_fptr && get_delegate_fptr && close_fptr);
    }
    // </SnippetLoadHostFxr>

    // <SnippetInitialize>
    // Load and initialize .NET Core and get desired function pointer for scenario
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path)
    {
        // Load .NET Core
        void *load_assembly_and_get_function_pointer = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = init_for_config_fptr(config_path, nullptr, &cxt);
        if (rc != 0 || cxt == nullptr)
        {
            std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            close_fptr(cxt);
            return nullptr;
        }

        // Get the load assembly function pointer
        rc = get_delegate_fptr(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            &load_assembly_and_get_function_pointer);
        if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
            std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

        close_fptr(cxt);
        return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
    }
    // </SnippetInitialize>
}
