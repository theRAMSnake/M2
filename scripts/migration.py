#!/bin/python

import subprocess
import json

def materiaReq(r):
    p = subprocess.Popen(["/home/snake/m4tools", json.dumps(r)], stdout=subprocess.PIPE)
    res = p.stdout.read()
    return json.loads(res)

class Migration:
    def __init__(s, query, ftor, isCreate):
        s._query = query
        s._ftor = ftor
        s._isCreate = isCreate
        pass

    def prepare(s):
        req = {
            "operation": "query",
            "filter": s._query
        }
        resp = materiaReq(req)
        objects = resp["object_list"]
        s._resultItems = []
        for x in objects:
            s._resultItems.append(s._ftor(x))

    def print(s):
        for x in s._resultItems:
            print(x)

    def apply(s):
        for x in s._resultItems:
            if not s._isCreate:
                upd = {
                    "operation": "modify",
                    "id": x["id"],
                    "params": x
                }
                updateResp = materiaReq(upd)
                print(updateResp)
            else:
                crt = {
                    "operation": "create",
                    "typename": x["typename"],
                    "params": x
                }
                createResp = materiaReq(crt)
                print(createResp)


def slUpgrade(obj):
    newObj = {}
    newObj["typename"] = "connection"
    newObj["A"] = obj["parentNodeId"]
    newObj["B"] = obj["id"]
    newObj["type"] = "Hierarchy"
    return newObj

def main():
    m = Migration("IS(strategy_node) AND .parentNodeId contains \"-\"", slUpgrade, True)
    m.prepare()
    m.print()
    result = input('Apply changes? y/n: ')
    if result == "y":
        m.apply()

if __name__ == "__main__":
    main()
