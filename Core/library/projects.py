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

    state = m4.MateriaObject()
    state.elementType = "state"
    m4.create("", "object", state, i);

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

def project_update_script(projectName):
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "update_script":
                    return c.code

    raise ValueError("Project not found")

def modify_project_update_script(projectName, newCode):
    for p in collection.Collection("projects").get_items():
        if p.name == projectName:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "update_script":
                    c.code = newCode
                    m4.modify(c.id, c)
                    return

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
    code_to_exec = ""
    state = None
    for p in collection.Collection("projects").get_items():
        if p.name == name:
            children = m4.query_expr(f'ChildOf("{p.id}")')
            for c in children:
                if c.elementType == "update_script":
                    code_to_exec = c.code
                if c.elementType == "state":
                    state = c

    exec(code_to_exec)
    m4.modify(state.id, state)

def update_projects():
    events = collection.Collection("events")
    for p in collection.Collection("projects").get_items():
        try:
            update_project(p.name)
        except Exception as e:
            ev = m4.MateriaObject()
            ev.result = f"Project {p.name} failed: {e}"
            events.add(ev)

def create_project_control(name, control_definition):
    for p in collection.Collection("projects").get_items():
        if p.name == name:
            control_definition.elementType = "control"
            return m4.create("", "object", control_definition, p.id);

    raise ValueError("Project not found")
