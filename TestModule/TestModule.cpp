#include "stdafx.h"

static PyObject* natmod_callback(PyObject* self, PyObject* arg) {
    PyObject* volatile result = PyObject_CallObject(arg, nullptr);
	return result;
}

static PyObject* natmod_debug_break(PyObject* self, PyObject* arg) {
    if (IsDebuggerPresent()) {
	    __debugbreak();
    }
    Py_RETURN_NONE;
}

static PyObject* natmod_crash(PyObject* self, PyObject* arg) {
	++*(volatile char*)nullptr;
	return nullptr;
}

struct dummy_struct {
	int x, y;
};

struct natmod_NatObjObject {
	PyObject_HEAD
	PyObject* o;
	int i;
	const char* s;
	dummy_struct ds;
};

static PyMemberDef natmod_NatObj_members[] = {
	{ "o", T_OBJECT, offsetof(natmod_NatObjObject, o), 0, nullptr },
	{ "i", T_INT, offsetof(natmod_NatObjObject, i), 0, nullptr },
	{ "s", T_STRING, offsetof(natmod_NatObjObject, s), 0, nullptr },
	{ nullptr }
};

static PyObject* natmod_NatObj_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
	auto self = (natmod_NatObjObject*)type->tp_alloc(type, 0);
	return (PyObject*)self;
}

static int natmod_NatObj_init(natmod_NatObjObject* self, PyObject* args, PyObject* kwds) {
	self->o = (PyObject*)self;
	Py_INCREF(self->o);
	self->i = 42;
	self->s = "NatObj";
	return 0;
}

static PyObject* natmod_NatObj_frob(natmod_NatObjObject* self, PyObject* args) {
	Py_INCREF(args);
	return args;
}

static PyObject* natmod_NatObj_prop_get(natmod_NatObjObject* self, void*) {
	Py_INCREF(self->o);
	return self->o;
}

static int natmod_NatObj_prop_set(natmod_NatObjObject* self, PyObject* value, void*) {
	Py_DECREF(self->o);
	self->o = value;
	return 0;
}

static PyMethodDef natmod_NatObj_methods[] = {
	{ "frob", (PyCFunction)natmod_NatObj_frob, METH_VARARGS, nullptr },
	{ nullptr }
};

static PyGetSetDef natmod_NatObj_getset[] = {
	{ "prop", (getter)natmod_NatObj_prop_get, (setter)natmod_NatObj_prop_set },
	{ nullptr }
};

static PyTypeObject natmod_NatObjType = {
	PyVarObject_HEAD_INIT(NULL, 0)
    "natmod.NatObj",               /* tp_name */
    sizeof(natmod_NatObjObject),   /* tp_basicsize */
    0,                             /* tp_itemsize */
    0,                             /* tp_dealloc */
    0,                             /* tp_print */
    0,                             /* tp_getattr */
    0,                             /* tp_setattr */
    0,                             /* tp_reserved */
    0,                             /* tp_repr */
    0,                             /* tp_as_number */
    0,                             /* tp_as_sequence */
    0,                             /* tp_as_mapping */
    0,                             /* tp_hash  */
    0,                             /* tp_call */
    0,                             /* tp_str */
    0,                             /* tp_getattro */
    0,                             /* tp_setattro */
    0,                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,            /* tp_flags */
    "NatObj objects",              /* tp_doc */
    0,                             /* tp_traverse */
    0,                             /* tp_clear */
    0,                             /* tp_richcompare */
    0,                             /* tp_weaklistoffset */
    0,                             /* tp_iter */
    0,                             /* tp_iternext */
	natmod_NatObj_methods,         /* tp_methods */
    natmod_NatObj_members,         /* tp_members */
    natmod_NatObj_getset,          /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    (initproc)natmod_NatObj_init,  /* tp_init */
    0,                             /* tp_alloc */
    natmod_NatObj_new,             /* tp_new */
};

static PyMethodDef natmod_methods[] = {
    { "callback", natmod_callback, METH_O, nullptr },
	{ "debug_break", natmod_debug_break, METH_NOARGS, nullptr },
	{ "crash", natmod_crash, METH_NOARGS, nullptr },
    { nullptr, nullptr, 0, nullptr }
};

#if PY_MAJOR_VERSION == 3

static PyModuleDef natmod_module = {
    PyModuleDef_HEAD_INIT,
    "natmod",
    nullptr,
    -1,
    natmod_methods
};

PyMODINIT_FUNC PyInit_natmod() {
    PyObject* natmod = PyModule_Create(&natmod_module);
	if (!natmod) {
		return nullptr;
	}

	natmod_NatObjType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&natmod_NatObjType) < 0) {
		return nullptr;
	}
	Py_INCREF(&natmod_NatObjType);
	PyModule_AddObject(natmod, "NatObj", (PyObject*)&natmod_NatObjType);

    return natmod;
}

#else

PyMODINIT_FUNC initnatmod() {
	PyObject* natmod = Py_InitModule("natmod", natmod_methods);
	if (!natmod) {
		return;
	}

	natmod_NatObjType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&natmod_NatObjType) < 0) {
		return;
	}
	Py_INCREF(&natmod_NatObjType);
	PyModule_AddObject(natmod, "NatObj", (PyObject*)&natmod_NatObjType);
}

#endif


extern "C" __declspec(dllexport)
int __stdcall frub(int x, int y) {
	return x + y;
}
