from m4 import query_expr

def queryChildren(parent_id):
    filter_expr = f"ChildOf({parent_id})"
    return query_expr(filter_expr)

