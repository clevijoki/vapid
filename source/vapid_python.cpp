#include <Python.h>

static PyObject* vapid_compile(PyObject *self, PyObject *args)
{
	Py_RETURN_NONE;
}

static PyObject* vapid_link(PyObject *self, PyObject *args)
{
	Py_RETURN_NONE;
}


static PyMethodDef VapidMethods[] = {
    {"compile",  vapid_compile, METH_VARARGS, "Performs a compile step (many to many)"},
    {"link",  vapid_link, METH_VARARGS, "Performs a link step (many to one)"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef vapidmodule = {
    PyModuleDef_HEAD_INIT,
    "vapid",   /* name of module */
    "Vapid build system.", /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    VapidMethods
};

PyMODINIT_FUNC
PyInit_vapid(void)
{
    return PyModule_Create(&vapidmodule);
}