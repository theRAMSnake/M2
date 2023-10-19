#include "ScriptRunner.hpp"
#include "ObjectManager.hpp"
#include "Expressions2.hpp"
#include <iostream>
#include <Python.h>

namespace materia
{

// Make sure this is getting accessed from the single thread only
ObjectManager* gOmPtr = nullptr;
static PyObject* MateriaObjectType = nullptr;

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
            if(obj[std::string(key_str)].isReadonly())
            {
                continue;
            }
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
    char* parent_id = nullptr;

    if (!PyArg_ParseTuple(args, "ssO|s", &id, &typename_, &pyObject, &parent_id)) {
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

        if (parent_id) {
            Id final_parent_id = Id(std::string(parent_id));
            gOmPtr->getConnections().create(final_parent_id, Id(result), ConnectionType::Hierarchy);
        }

        return PyUnicode_FromString(result.c_str());
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}

static PyObject* py_modify(PyObject* self, PyObject* args) {
    char* id;
    PyObject* pyObject;

    // We expect two arguments: an ID and a python object
    if (!PyArg_ParseTuple(args, "sO", &id, &pyObject)) {
        return nullptr;
    }

    // Ensure ID is non-empty
    if (std::string(id).size() == 0) {
        PyErr_SetString(PyExc_ValueError, "ID cannot be an empty string");
        return nullptr;
    }

    PythonValueProvider prov(pyObject);

    try {
        gOmPtr->modify(Id(id), prov);
        Py_RETURN_NONE;  // Return 'None' in Python to signify successful completion
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}

static PyObject* py_erase(PyObject* self, PyObject* args) {
    char* id;

    // We expect a single argument: an ID
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return nullptr;
    }

    // Ensure ID is non-empty
    if (std::string(id).size() == 0) {
        PyErr_SetString(PyExc_ValueError, "ID cannot be an empty string");
        return nullptr;
    }

    try {
        gOmPtr->destroy(Id(id));
        Py_RETURN_NONE;  // Return 'None' in Python to signify successful completion
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}

static PyObject* py_query_ids(PyObject* self, PyObject* args) {
    PyObject* pyIdList;

    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &pyIdList)) {
        return nullptr;
    }

    int listSize = PyList_Size(pyIdList);
    std::vector<Id> ids;
    for (int i = 0; i < listSize; i++) {
        PyObject* pyId = PyList_GetItem(pyIdList, i);
        if (PyUnicode_Check(pyId)) {
            ids.push_back(Id(PyUnicode_AsUTF8(pyId)));
        } else {
            PyErr_SetString(PyExc_TypeError, "All elements in the list must be strings.");
            return nullptr;
        }
    }

    try {
        std::vector<Object> results = gOmPtr->query(ids);

        PyObject* pyResults = PyList_New(results.size());
        for (size_t i = 0; i < results.size(); ++i) {
            PyObject* pyObjInstance = PyObject_CallObject(MateriaObjectType, nullptr);  // Create an instance of MateriaObject
            for (const auto& field : results[i]) {
                PyObject_SetAttrString(pyObjInstance, field.getName().c_str(), PyUnicode_FromString(field.get<Type::String>().c_str()));
            }
            PyList_SetItem(pyResults, i, pyObjInstance);
        }

        return pyResults;
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}

static PyObject* py_query_expr(PyObject* self, PyObject* args) {
    char* filter_expr;

    if (!PyArg_ParseTuple(args, "s", &filter_expr)) {
        return nullptr;
    }

    try {
        auto expr = v2::parseExpression(filter_expr);
        if(!expr)
        {
            throw std::runtime_error("Failed to parse expression");
        }
        std::vector<Object> results = gOmPtr->query(*expr);

        PyObject* pyResults = PyList_New(results.size());
        for (size_t i = 0; i < results.size(); ++i) {
            PyObject* pyObjInstance = PyObject_CallObject(MateriaObjectType, nullptr);  // Create an instance of MateriaObject
            for (const auto& field : results[i]) {
                PyObject_SetAttrString(pyObjInstance, field.getName().c_str(), PyUnicode_FromString(field.get<Type::String>().c_str()));
            }
            PyList_SetItem(pyResults, i, pyObjInstance);
        }
        return pyResults;

    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}

static PyMethodDef methods[] = {
    {"create", py_create, METH_VARARGS, "Create function"},
    {"modify", py_modify, METH_VARARGS, "Modify function"},
    {"erase", py_erase, METH_VARARGS, "Erase function"},
    {"query_ids", py_query_ids, METH_VARARGS, "Query by ids function"},
    {"query_expr", py_query_expr, METH_VARARGS, "Query by expression function"},
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


void InitializeMateriaObjectClass(PyObject* m4Module) {
    // Define the MateriaObject class.
    const char* materiaObjectClassCode = R"(
class MateriaObject:
    def __init__(self):
        pass
)";
    PyObject* pNamespace = PyDict_New();
    PyObject* pCode = PyRun_String(materiaObjectClassCode, Py_file_input, pNamespace, pNamespace);
    if (!pCode) {
        PyErr_Print();
        throw std::runtime_error("Failed to run MateriaObject class code.");
    }
    Py_XDECREF(pCode);

    MateriaObjectType = PyDict_GetItemString(pNamespace, "MateriaObject");
    if (!MateriaObjectType) {
        PyErr_Print();
        throw std::runtime_error("MateriaObject class not found in namespace.");
    }
    Py_INCREF(MateriaObjectType);

    if (!m4Module) {
        PyErr_Print();
        throw std::runtime_error("m4Module is not initialized.");
    }
    if (PyModule_AddObject(m4Module, "MateriaObject", MateriaObjectType) < 0) {
        PyErr_Print();
        throw std::runtime_error("Failed to add MateriaObject to m4 module.");
    }

    Py_XDECREF(pNamespace);
}


std::string runScript(const std::string& code, ObjectManager& om)
{
    gOmPtr = &om;

    if (!Py_IsInitialized()) {
        PyImport_AppendInittab("m4", &PyInit_cpp_module);
        Py_Initialize();
        auto m4Module = PyImport_ImportModule("m4");
        if (!m4Module) {
            PyErr_Print();
            throw std::runtime_error("Failed to import m4 module.");
        }

        InitializeMateriaObjectClass(m4Module);
        // Add the 'bin/library' directory to sys.path
        PyObject* sys = PyImport_ImportModule("sys");
        PyObject* path = PyObject_GetAttrString(sys, "path");
        PyObject* dirToAdd = PyUnicode_FromString("../Core/library");
        PyList_Append(path, dirToAdd);

        Py_XDECREF(dirToAdd);
        Py_XDECREF(path);
        Py_XDECREF(sys);
        Py_XDECREF(m4Module);
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

    //Do not uncomment - will crash
    //Py_DECREF(pResult);

    return run_result;
}

}
