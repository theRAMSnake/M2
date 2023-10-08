#include "ScriptRunner.hpp"
#include "ObjectManager.hpp"
#include <Python.h>

namespace materia
{

// Make sure this is getting accessed from the single thread only
ObjectManager* gOmPtr = nullptr;

class PythonValueProvider : public IValueProvider
{
public:
    PythonValueProvider(PyObject* pyObject)
        : mSrc(pyObject)
    {
    }

    void populate(Object& obj) const override
    {
        int size = PyList_Size(mSrc);
        for (int i = 0; i < size; i++) {
            PyObject* pair = PyList_GetItem(mSrc, i);
            char* first;
            char* second;
            if (PyArg_ParseTuple(pair, "ss", &first, &second)) {
                obj[first] = second;
            }
        }

    }

private:
    PyObject* mSrc;
};

static PyObject* py_create(PyObject* self, PyObject* args) {
    char* id;
    char* typename_;
    PyObject* paramsList;

    if (!PyArg_ParseTuple(args, "ssO", &id, &typename_, &paramsList)) {
        return nullptr;
    }

    PythonValueProvider prov(paramsList);

    std::optional<Id> new_id;
    if(std::string(id).size() != 0)
    {
        new_id = std::string(id);
    }
    std::string result = gOmPtr->create(new_id, typename_, prov).getId();
    return PyUnicode_FromString(result.c_str());
}

static PyObject* py_modify(PyObject* self, PyObject* args) {
    // ... Code for the py_modify function as shown above ...
}

static PyObject* py_erase(PyObject* self, PyObject* args) {
    // ... Code for the py_erase function as shown above ...
}

static PyObject* py_query_ids(PyObject* self, PyObject* args) {
    // ... Code for the py_query_ids function as shown above ...
}

static PyObject* py_query_expr(PyObject* self, PyObject* args) {
    // ... Code for the py_query_expr function as shown above ...
}

static PyObject* py_reward(PyObject* self, PyObject* args) {
    // ... Code for the py_reward function as shown above ...
}

static PyMethodDef methods[] = {
    {"m4_create", py_create, METH_VARARGS, "Create function"},
    {"m4_modify", py_modify, METH_VARARGS, "Modify function"},
    {"m4_erase", py_erase, METH_VARARGS, "Erase function"},
    {"m4_query_ids", py_query_ids, METH_VARARGS, "Query by ids function"},
    {"m4_query_expr", py_query_expr, METH_VARARGS, "Query by expression function"},
    {"m4_reward", py_reward, METH_VARARGS, "Reward function"},
    {nullptr, nullptr, 0, nullptr}  // Sentinel
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "cpp_module",  // module name
    nullptr,       // module documentation
    -1,            // module state size
    methods        // module methods
};

PyMODINIT_FUNC PyInit_cpp_module() {
    return PyModule_Create(&module);
}

std::string fetchPythonError() {
    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyObject* error_string = PyObject_Repr(pvalue);
    const char* c_error_string = PyUnicode_AsUTF8(error_string);
    std::string result(c_error_string);
    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);
    Py_XDECREF(error_string);
    return result;
}

std::string runScript(const std::string& code, ObjectManager& om)
{
    gOmPtr = &om;

    if (!Py_IsInitialized()) {
        PyImport_AppendInittab("cpp_module", &PyInit_cpp_module);
        Py_Initialize();
    }

    std::string run_result;

    PyObject* global_dict = PyDict_New();

    PyObject* result = PyRun_String(code.c_str(), Py_file_input, global_dict, global_dict);

    if (result == nullptr) {
        throw std::runtime_error(fetchPythonError());
    }

    Py_XDECREF(result);
    PyObject* pResult = PyDict_GetItemString(global_dict, "result");
    if (pResult) {
        PyObject* pResultStr = PyObject_Str(pResult);
        if (pResultStr) {
            run_result = PyUnicode_AsUTF8(pResultStr);
            Py_DECREF(pResultStr);
        }
    } else {
        throw std::runtime_error("No 'result' key found in global dictionary.");
    }

    Py_DECREF(pResult);

    return run_result;
}

}
