#!/bin/python
import argparse
import subprocess
import json
import requests
import time

def alphaReq(r, key):
    #There is a limit of 5 requests per minute, so sleep(20)
    time.sleep(20)
    uri = "https://www.alphavantage.co/query?{}&apikey={}".format(r, key)
    return requests.get(uri).json()

def materiaReq(r):
    p = subprocess.Popen(["/home/snake/m4tools", json.dumps(r)], stdout=subprocess.PIPE)
    return json.loads(p.stdout.read())

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("api_key", type=str)
    args = parser.parse_args()

    req = {
        "operation": "query",
        "filter": "IS(currency)"
    }
    currencyResp = materiaReq(req);
    for c in currencyResp["object_list"]:
        if c["name"] != "EUR":
            alphaResp = alphaReq("function=CURRENCY_EXCHANGE_RATE&from_currency=EUR&to_currency={}".format(c["name"]), args.api_key)
            c["conversionRateToEur"] = alphaResp["Realtime Currency Exchange Rate"]["5. Exchange Rate"]
            upd = {
                "operation": "modify",
                "id": c["id"],
                "params": c
            }
            updateResp = materiaReq(upd)

if __name__ == "__main__":
    main()
