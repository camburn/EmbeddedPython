/*
This is an example C application that has an embedded python interpreter.
It provides C function calls that be called from Python and
Python function calls that can be called from C.
*/
#include <Python.h>

#include "python_class.h"

/* BEGIN - C API to present to our Python instance */
// Methods
static PyObject*
emb_testfunction(PyObject *self, PyObject *args) {
    double a, b, c;
    if (!PyArg_ParseTuple(args, "ddd", &a, &b, &c)) {
        return NULL;
    }
    return Py_BuildValue("d", a * b * c);
}

static PyObject*
emb_stringfunc(PyObject *self, PyObject *args) {
    char* str_data;
    if (!PyArg_ParseTuple(args, "s", &str_data)) {
        return NULL;
    }
    fprintf(stdout, "COUT: %s\n", str_data);
    return Py_BuildValue("");
}

// Create a method list to give to our module
static PyMethodDef EmbMethods[] = {
    {"testfunction", emb_testfunction, METH_VARARGS, "Multiply args."},
    {"stringfunc", emb_stringfunc, METH_VARARGS, "C print a python str."},
    {NULL, NULL, 0, NULL}
};

// Create a python module
static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_emb(void) {
    return PyModule_Create(&EmbModule);
}
/* END - C API */

int
main(int argc, char *argv[]) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0] (locale)\n");
        exit(1);
    }
    // Create program
    Py_SetProgramName(program);
    // Import custom methods
    PyImport_AppendInittab("emb", &PyInit_emb);
    // Import custom class
    PyImport_AppendInittab("custom2", &PyInit_custom2);
    Py_Initialize();
    // Add our current directory to path - can also use Py_SetPath
    PyRun_SimpleString(
        "import sys\n"
        "sys.path.append('./')\n"
    );
    // Import our our python module
    pName = PyUnicode_FromString("console.sample");
    pModule = PyImport_Import(pName);
    if (pModule != NULL) {
        // Get our function from the module
        pFunc = PyObject_GetAttrString(pModule, "print_random_word");
        if (pFunc && PyCallable_Check(pFunc)) {
            // Call the function with the appropriate arguments
            pArgs = PyTuple_Pack(1, Py_BuildValue("i", 10));
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            Py_DECREF(pValue);
        } else {
            PyErr_Print();
            fprintf(stderr, "Failed to find function - %s", "print_random_word");
            return 1;
        }

        // Call a python Class from C
        // Get the class from the module
        PyObject *script_class = PyObject_GetAttrString(pModule, "Console");
        // Initialise and intance of the class 
        PyObject* script_instance = PyObject_CallFunction(script_class, "f", 25.5f);
        // Call a method of the class
        PyObject_CallMethod(script_instance, "write", "f", 25.5f);

        // Run create an instance a class defined within C from python
        PyObject *custom_method = PyObject_GetAttrString(pModule, "run_custom_c_module");
        PyObject_CallFunction(custom_method, "");

        // Load an object from our C defined class (note using the defined module)
        PyObject *custom_class = PyObject_GetAttrString(PyInit_custom2(), "Custom");
        PyObject *args = Py_BuildValue("(ssi)", "Mal", "Brookes", 66);
        PyObject *custom_instance = PyObject_CallObject(custom_class, args);
        PyObject *pValue = PyObject_CallMethod(custom_instance, "name", "");

        PyObject* temp = PyUnicode_AsASCIIString(pValue);
        fprintf(stdout, "C Result: %s\n", PyBytes_AsString(temp));

        // NOTE: All object above should be cleaned up with Py_DECREF

    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load module - %s", "console.sample");
        return 1;
    }


    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    // Run arbitary python code through the interpreter
    PyRun_SimpleString(
        "from time import time\n"
        "the_time = time()\n"
        "print(f'Time is {the_time}')\n"
    );
    PyRun_SimpleString(
        "import emb\n"
        "print('Result: {}'.format(emb.testfunction(2, 2, 2)))\n"
        "emb.stringfunc('A test c print')\n"
    );
    if (Py_FinalizeEx() < 0) {
        exit(120);
    };
    PyMem_RawFree(program);
    return 0;
}
