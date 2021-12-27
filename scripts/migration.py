#!/bin/python

import subprocess
import json

def materiaReq(r):
    p = subprocess.Popen(["/home/snake/m4tools", json.dumps(r)], stdout=subprocess.PIPE)
    res = p.stdout.read()
    return json.loads(res)

class Migration:
    def __init__(s, query, ftor):
        s._query = query
        s._ftor = ftor
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
            upd = {
                "operation": "modify",
                "id": x["id"],
                "params": x
            }
            updateResp = materiaReq(upd)
            print(updateResp)

def calendarUpgrade(obj):
    s = obj["reccurencyTypeChoice"] 
    r = ""
    if s == "Weekly":
        r = "7d"
    if s == "Monthly":
        r = "1m"
    if s == "Quarterly":
        r = "3m"
    if s == "Yearly":
        r = "1y"
    if s == "Bi-daily":
        r = "2d"
    if s == "Daily":
        r = "1d"
    
    obj["recurrency"] = r
    return obj

def main():
    m = Migration("IS(calendar_item)", calendarUpgrade)
    m.prepare()
    m.print()
    result = input('Apply changes? y/n: ')
    if result == "y":
        m.apply()

if __name__ == "__main__":
    main()
