#include <Python.h>


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
    {"system",  spam_test, METH_VARARGS,
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

    //PyRun_SimpleString("import spam;print(dir(spam));spam.system(\"ls\")");
    //PyRun_SimpleString("import noddy;print(noddy.Noddy())");

    PyObject *arglist;

    //call_py(arglist);
    Py_Finalize();
}

