using Microsoft.Win32;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

static class Program
{
#if DEBUG
    const string PythonDll = "python39_d.dll";
#else
    const string PythonDll = "python39.dll";
#endif

    const int METH_VARARGS = 0x0001;
    const int METH_O = 0x0008;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    delegate IntPtr PyCFunction(IntPtr self, IntPtr args);

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    struct PyMethodDef
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string ml_name;

        public PyCFunction ml_meth;

        public int ml_flags;

        [MarshalAs(UnmanagedType.LPStr)]
        public string ml_doc;
    }

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
    static extern void Py_SetProgramName(string s);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern void Py_Initialize();

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern void Py_Finalize();

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    static extern int PyRun_SimpleStringFlags(string s, IntPtr flags);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    static extern IntPtr PyImport_ImportModule(string name);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern void Py_IncRef(IntPtr obj);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern void Py_DecRef(IntPtr obj);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    static extern IntPtr PyObject_GetAttrString(IntPtr obj, string name);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    static extern int PyObject_SetAttrString(IntPtr obj, string name, IntPtr value);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr PyObject_CallObject(IntPtr callable_object, IntPtr args);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    static extern IntPtr PyObject_CallMethod(IntPtr callable_object, string method, string format, __arglist);

    [DllImport(PythonDll, CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr PyCFunction_NewEx(IntPtr methodDef, IntPtr self, IntPtr flags);

    static void Main(string[] args)
    {
        Console.WriteLine("Use Debug -> Attach to attach with Managed + Native + Python code type, and press any key to continue ...");
        Console.ReadKey();

        var pythonDir = Registry.GetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\3.9\InstallPath", "", null);
        Trace.Assert(pythonDir != null);
        Environment.SetEnvironmentVariable("PATH", Environment.GetEnvironmentVariable("PATH") + ";" + pythonDir);

        // Note that the working directory is set to TestApp, so this is relative to that.
        Environment.SetEnvironmentVariable("PYTHONPATH", @".;..\Debug\x64");

        Py_SetProgramName(typeof(Program).Assembly.ManifestModule.Name);
        Py_Initialize();

        var methodDef = new PyMethodDef { ml_meth = Callback, ml_name = "Callback", ml_flags = METH_O };
        var methodDefPtr = Marshal.AllocHGlobal(Marshal.SizeOf(methodDef));
        Marshal.StructureToPtr(methodDef, methodDefPtr, false);
        IntPtr callback = PyCFunction_NewEx(methodDefPtr, IntPtr.Zero, IntPtr.Zero);
        Trace.Assert(callback != IntPtr.Zero);

        IntPtr mainModule = PyImport_ImportModule("main");
        Trace.Assert(mainModule != IntPtr.Zero);

        PyObject_SetAttrString(mainModule, "dotnet_callback", callback);

        IntPtr python_calling_dotnet = PyObject_GetAttrString(mainModule, "python_calling_dotnet");
        Trace.Assert(python_calling_dotnet != IntPtr.Zero);
        PyObject_CallObject(python_calling_dotnet, IntPtr.Zero);

        Py_Finalize();
    }

    static IntPtr Callback(IntPtr self, IntPtr arg)
    {
        Console.WriteLine(".NET called from Python");
        PyObject_CallObject(arg, IntPtr.Zero);
        Py_IncRef(arg);
        return arg;
    }
}
