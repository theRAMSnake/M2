import collection
import m4

def create_project(name):
    prj = m4.MateriaObject()
    prj.name = name
    i = collection.Collection("projects").add(prj)

    uscript = m4.MateriaObject()
    uscript.code = ""
    m4.create("", "object", uscript, i);
