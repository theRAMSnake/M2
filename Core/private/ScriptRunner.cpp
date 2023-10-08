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
        if (!PyObject_HasAttrString(mSrc, "__dict__")) {
            throw std::runtime_error("Expected an object with attributes");
        }

        PyObject* attributes = PyObject_GetAttrString(mSrc, "__dict__");
        if (!PyDict_Check(attributes)) {
            throw std::runtime_error("Object attributes are not a dictionary");
        }

        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(attributes, &pos, &key, &value)) {
            const char* key_str = PyUnicode_AsUTF8(key);
            PyObject* value_str_object = PyObject_Str(value);  // Convert value to string
            const char* value_str = PyUnicode_AsUTF8(value_str_object);

            obj[key_str] = value_str;

            Py_DECREF(value_str_object);
        }
        Py_DECREF(attributes);
    }

private:
    PyObject* mSrc;
};

static PyObject* py_create(PyObject* self, PyObject* args) {
    char* id;
    char* typename_;
    PyObject* pyObject;

    if (!PyArg_ParseTuple(args, "ssO", &id, &typename_, &pyObject)) {
        return nullptr;
    }

    PythonValueProvider prov(pyObject);

    std::optional<Id> new_id;
    if(std::string(id).size() != 0)
    {
        new_id = std::string(id);
    }

    try {
        std::string result = gOmPtr->create(new_id, typename_, prov).getId();
        return PyUnicode_FromString(result.c_str());
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
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
    {"create", py_create, METH_VARARGS, "Create function"},
    {"modify", py_modify, METH_VARARGS, "Modify function"},
    {"erase", py_erase, METH_VARARGS, "Erase function"},
    {"query_ids", py_query_ids, METH_VARARGS, "Query by ids function"},
    {"query_expr", py_query_expr, METH_VARARGS, "Query by expression function"},
    {"reward", py_reward, METH_VARARGS, "Reward function"},
    {nullptr, nullptr, 0, nullptr}  // Sentinel
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "m4",  // module name
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
        PyImport_AppendInittab("m4", &PyInit_cpp_module);
        Py_Initialize();
    }

    std::string run_result;

    PyObject* global_dict = PyDict_New();

    PyRun_String("import m4", Py_file_input, global_dict, global_dict);
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
