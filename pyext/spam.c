#include <Python.h>

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    void* ptr_data;
} noddy_NoddyObject;

static PyObject *
Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    noddy_NoddyObject* noddy;
    noddy = (noddy_NoddyObject*)type->tp_alloc(type, 0);;
    noddy->ptr_data = (void*)11122334;
    printf("XXXXXXXXXX...\n");
    return (PyObject*)noddy;
}

static PyTypeObject noddy_NoddyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "noddy.Noddy",             /*tp_name*/
    sizeof(noddy_NoddyObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Noddy objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    Noddy_new,                 /* tp_new */
};

static PyMethodDef noddy_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initnoddy(void) 
{
    PyObject* m;

    noddy_NoddyType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&noddy_NoddyType) < 0)
    {
        printf("Oh failed\n");
        return;
    }

    m = Py_InitModule3("noddy", noddy_methods,
                       "Example module that creates an extension type.");

    Py_INCREF(&noddy_NoddyType);
    PyModule_AddObject(m, "Noddy", (PyObject *)&noddy_NoddyType);
}


static PyObject *
spam_system(PyObject *self, PyObject *args)
{
    noddy_NoddyObject *channel;

    if (!PyArg_ParseTuple(args, "O!", &noddy_NoddyType, channel))
        return NULL;
    printf("xx %p\n", channel->ptr_data);
    Py_DECREF(channel);
    return Py_BuildValue("i", 100);
}

static PyObject *
spam_test(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return Py_BuildValue("i", sts);
}


static PyObject *SpamError;

static PyMethodDef SpamMethods[] = {
    {"system",  spam_system, METH_VARARGS,
     "Execute a shell command."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initspam(void)
{
    PyObject *m;

    m = Py_InitModule("spam", SpamMethods);
    if (m == NULL)
        return;

    SpamError = PyErr_NewException("spam.error", NULL, NULL);
    Py_INCREF(SpamError);
    PyModule_AddObject(m, "error", SpamError);
}

int call_py(PyObject *arglist)
{
    PyObject *pName, *pModule, *pDict, *pFunc, *pRetVal;
    pName = PyString_FromString("pytest");
    pModule = PyImport_Import(pName);
    if ( !pModule )  
    {  
	printf("can't find pytest.py");  
	return -1;  
    }
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, "test");
    if ( !pFunc || !PyCallable_Check(pFunc) )  
    {  
	printf("can't find function [add]");  
	return -1;  
    }
    pRetVal = PyObject_CallObject(pFunc, arglist);
    printf("function return value : %ld\r\n", PyInt_AsLong(pRetVal));
    Py_DECREF(pName);
    Py_DECREF(arglist);
    Py_DECREF(pModule); 
    Py_DECREF(pRetVal);
    return 0;
}


int
main(int argc, char *argv[])
{
    Py_SetProgramName(argv[0]);

    Py_Initialize();

    initspam();
    initnoddy();

    noddy_NoddyObject* noddy;
    noddy = PyObject_New(noddy_NoddyObject, &noddy_NoddyType);
    noddy->ptr_data = NULL;

    //PyRun_SimpleString("import spam;print(dir(spam));spam.system(\"ls\")");
    //PyRun_SimpleString("import noddy;print(noddy.Noddy())");

    PyObject *arglist;
    arglist = Py_BuildValue("(O)", noddy);
    call_py(arglist);
    Py_Finalize();
}

