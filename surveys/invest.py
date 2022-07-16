#!/bin/python
import argparse
import subprocess
import json
import requests
import time
import traceback
import sys

def format_exception(e):
    exception_list = traceback.format_stack()
    exception_list = exception_list[:-2]
    exception_list.extend(traceback.format_tb(sys.exc_info()[2]))
    exception_list.extend(traceback.format_exception_only(sys.exc_info()[0], sys.exc_info()[1]))

    exception_str = "Traceback (most recent call last):\n"
    exception_str += "".join(exception_list)
    # Removing the last \n
    exception_str = exception_str[:-1]
    return exception_str

def alphaReq(r, key):
    #Try up to 5 times before giving up
    for i in range(5):
        try:
            #There is a limit of 5 requests per minute, so sleep(20)
            time.sleep(20)
            uri = "https://www.alphavantage.co/query?{}&apikey={}".format(r, key)
            return requests.get(uri).json()
        except:
            pass
    raise Exception("alpha req is unavailable")

def materiaReq(r):
    p = subprocess.Popen(["/home/snake/m4tools", json.dumps(r)], stdout=subprocess.PIPE)
    res = p.stdout.read()
    return json.loads(res)

def composeMateriaMoney(value, domain):
    if domain == "US":
        currency = "USD"
    if domain == "EUR":
        currency = "EUR"

    return value[:-2] + currency

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

    tickers = []
    req = {
        "operation": "query",
        "filter": "IS(finance_stock)"
    }
    resp = materiaReq(req);
    for c in resp["object_list"]:
        tickers.append(c["ticker"])

    req = {
        "operation": "query",
        "ids": ["data.snp"]
    }
    resp = materiaReq(req);
    for i in range(20):
        c = resp["object_list"][0][str(i)]
        tickers.append(c["ticker"])

    for t in set(tickers):
        print(t)
        alphaResp = alphaReq("function=TIME_SERIES_DAILY&symbol={}".format(t), args.api_key)
        print(alphaResp)
        lastDate = alphaResp["Meta Data"]["3. Last Refreshed"]
        value = alphaResp["Time Series (Daily)"][lastDate]["4. close"]
        req = {
            "operation": "query",
            "filter": "IS(finance_stock) AND .ticker = \"{}\"".format(t)
        }
        resp = materiaReq(req);
        if "object_list" in resp and len(resp["object_list"]) > 0:
            obj = resp["object_list"][0]
            obj["lastKnownPrice"] = composeMateriaMoney(value, obj["domain"])
            upd = {
                "operation": "modify",
                "id": obj["id"],
                "params": obj
            }
            updateResp = materiaReq(upd)
        else:
            # If there is no stock it means a snp record
            cre = {
                "operation": "create",
                "typename": "finance_stock",
                "params": {
                    "lastKnownPrice": composeMateriaMoney(value, "US"),
                    "amount": 0,
                    "ticker": t,
                    "domain": "US"
                    }
            }
            creResp = materiaReq(cre)

if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        req = {
            "operation": "push",
            "listId": "inbox",
            "value": "An error occured during surveying 'invest': {}".format(format_exception(error))
        }
        materiaReq(req)
        raise

