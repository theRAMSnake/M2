import collection
import json
import m4

def create_project(name):
    prj = m4.MateriaObject()
    prj.name = name
    i = collection.Collection("projects").add(prj)

    uscript = m4.MateriaObject()
    uscript.code = ""
    m4.create("", "object", uscript, i);

def bind_collection(projectName, collectionName):
    for p in collection.Collection("projects"):
        if p.name == projectName:
            binding = m4.MateriaObject()
            binding.name = collectionName
            binding.elementType = "binding"
            m4.create("", "object", binding, p.id);
            return

    raise ValueError("Project not found")

def project_collections(projectName):
    result = []
    for p in collection.Collection("projects"):
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "binding":
                    result.append(c.name)
            return json.dumps(result)

    raise ValueError("Project not found")

def unbind_collection(projectName, colName):
    result = []
    for p in collection.Collection("projects"):
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "binding" and c.name == colName:
                    m4.erase(c.id)
                    return

    raise ValueError("Project not found")
