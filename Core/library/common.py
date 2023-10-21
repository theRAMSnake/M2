import m4
from datetime import datetime

def queryChildren(parent_id):
    filter_expr = f"ChildOf({parent_id})"
    return m4.query_expr(filter_expr)

def get_or_create(oid):
    objs = m4.query_ids([f"{oid}"])
    if len(objs) == 0:
        newObj = m4.MateriaObject()
        m4.create(oid, "object", newObj)
    return m4.query_ids([f"{oid}"])[0]

def str_to_date(string):
    return datetime.strptime(string, "%Y-%m-%d %H:%M:%S.%f")
