import collection
import json
import m4

def create_project(name):
    prj = m4.MateriaObject()
    prj.name = name
    i = collection.Collection("projects").add(prj)

    uscript = m4.MateriaObject()
    uscript.elementType = "update_script"
    uscript.code = ""
    m4.create("", "object", uscript, i);

def bind_collection(projectName, collectionName):
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            binding = m4.MateriaObject()
            binding.name = collectionName
            binding.elementType = "binding"
            m4.create("", "object", binding, p.id);
            return

    raise ValueError("Project not found")

def project_collections(projectName):
    result = []
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "binding":
                    result.append(c.name)
            return json.dumps(result)

    raise ValueError("Project not found")

def unbind_collection(projectName, colName):
    result = []
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "binding" and c.name == colName:
                    m4.erase(c.id)
                    return

    raise ValueError("Project not found")

def update_project(name):
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "update_script":
                    exec(c.code)

def update_projects():
    events = collection.Collection("events")
    for p in collection.Collection("projects").get_items():
        try:
            update_project(p.name)
        except Exception as e:
            ev = m4.MateriaObject()
            ev.result = f"Project {p.name} failed: {e.text}"
            events.add(ev)
