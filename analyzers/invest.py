#!/bin/python

import subprocess
import json
import traceback
import sys
import copy
import time

#propose trades

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

def materiaReq(r):
    p = subprocess.Popen(["/home/snake/m4tools", json.dumps(r)], stdout=subprocess.PIPE)
    res = p.stdout.read()
    return json.loads(res)

def first(predicate, seq):
    for i in seq:
        if predicate(i): return i
    return None

class Money:
    def __init__(s, initStr, curList):
        s.currency = initStr[-3:]
        if s.currency != "EUR":
            s.valueMod = float(first(lambda x: x["name"] == s.currency, curList)["conversionRateToEur"]);
        else:
            s.valueMod = 1.0
        s.valueInEUR = float(initStr[:-3]) / s.valueMod

    def __add__(s, o):
        r = copy.copy(s)
        r.valueInEUR = s.valueInEUR + o.valueInEUR
        return r

    def __sub__(s, o):
        r = copy.copy(s)
        r.valueInEUR = s.valueInEUR - o.valueInEUR
        return r

    def __mul__(s, num):
        r = copy.copy(s)
        r.valueInEUR = s.valueInEUR * num
        return r

    def __lt__(s, o):
        return s.valueInEUR < o.valueInEUR

    def __truediv__(s, div):
        if type(div) == int:
            r = copy.copy(s)
            r.valueInEUR = s.valueInEUR / div
            return r
        if type(div) == Money:
            #return ratio instead
            return s.valueInEUR / div.valueInEUR

    def __str__(s):
        return "%.2f%s" % ((s.valueInEUR * s.valueMod), s.currency)

def calculatePEandAddDataPoint(currencyList):
    totalValue = Money("0.00EUR", currencyList)

    req = {
        "operation": "query",
        "filter": "IS(finance_stock)"
    }
    resp = materiaReq(req)
    for c in resp["object_list"]:
        if c["amount"] != "0":
            curValue = Money(c["lastKnownPrice"], currencyList);
            totalValue = totalValue + curValue * int(c["amount"])

    cre = {
        "operation": "create",
        "typename": "finance_dataPoint",
        "params": {
            "timestamp": int(time.time()),
            "totalPortfolioValue": str(totalValue)
            }
    }
    creResp = materiaReq(cre)
    return totalValue

def sortByTimestamp(x):
    return x["timestamp"]

def updateTargets(currencyList, totalValue):
    #1. Calculate average daily PE
    secondsInYear = 31557600
    currentTime = int(time.time())
    oneYearAgo = currentTime - secondsInYear
    req = {
        "operation": "query",
        "filter": "IS(finance_dataPoint) AND .timestamp > {}".format(oneYearAgo)
    }
    resp = materiaReq(req)
    dataPoints = resp["object_list"] 
    dataPoints.sort(reverse=False, key=sortByTimestamp)
    fullYearlyPE = Money(dataPoints[-1]["totalPortfolioValue"], currencyList) - Money(dataPoints[0]["totalPortfolioValue"], currencyList)

    req = {
        "operation": "query",
        "filter": "IS(finance_investmentAction) AND .timestamp > {}".format(oneYearAgo)
    }
    resp = materiaReq(req)
    invPerYear = Money("0.00EUR", currencyList)
    for c in resp["object_list"]:
        invPerYear = invPerYear + Money(c["value"], currencyList)

    avgPerDay = (fullYearlyPE - invPerYear) / 365

    #2. Calculate percentage of salary
    req = {
        "operation": "query",
        "ids": ["financial_report"]
    }
    resp = materiaReq(req)
    salaryPerDay = Money(resp["object_list"][0]["Salary"]["total"], currencyList) / 365

    #3. Update total invested value of all time
    req = {
        "operation": "query",
        "filter": "IS(finance_investmentAction)"
    }
    resp = materiaReq(req)
    invTotal = Money("0.00EUR", currencyList)
    for c in resp["object_list"]:
        invTotal = invTotal + Money(c["value"], currencyList)

    req = {
        "operation": "query",
        "ids": ["invest.cb"]
    }
    resp = materiaReq(req)
    cb = resp["object_list"][0]
    cb["percentOfSalary"] = avgPerDay / salaryPerDay
    cb["avgPEperDay"] = str(avgPerDay)
    cb["valueInvested"] = str(invTotal)
    cb["totalValue"] = str(totalValue)

    upd = {
        "operation": "modify",
        "id": "invest.cb",
        "params": cb
    }
    updateResp = materiaReq(upd)

def proposeTrades(currencyList):
    req = {
        "operation": "query",
        "filter": "IS(finance_stock) AND .domain = \"US\""
    }
    resp = materiaReq(req)
    
    allstocks = {}

    for x in resp["object_list"]:
        allstocks[x["ticker"]] = {
                "ticker": x["ticker"],
                "amount": int(x["amount"]),
                "price": Money(x["lastKnownPrice"], currencyList),
                "goal": 0,
                "priority": -1,
                "id": x["id"],
                "new": False
                } 

    req = {
        "operation": "query",
        "ids": ["portfolio_goal"]
    }
    resp = materiaReq(req)
    stocksGoal = list(resp["object_list"][0].values())
    for x in stocksGoal:
        #Skip incompatible fields
        print(x)
        if not "id" in x:
            continue
        print("+")

        tick = x["ticker"]
        goal = int(float(x["amount"]))
        if goal == 0:
            goal = 1

        if tick in allstocks:
            allstocks[tick]["goal"] = goal
            allstocks[tick]["priority"] = int(x["id"])
        else:
            allstocks[tick] = {
                    "ticker": tick,
                    "amount": 0,
                    "price": Money(x["price"] + "USD", currencyList),
                    "goal": goal,
                    "priority": int(x["id"]),
                    "new": True
                    }

    for i, (ticker, stock) in enumerate(allstocks.items()):
        print("{}: {}".format(ticker, stock))

    # Clean up stocks not in snp and for which there are no holdings
    for i, (ticker, stock) in enumerate(allstocks.items()):
        if stock["amount"] == 0 and stock["goal"] == 0:
            delete = {
                "operation": "destroy",
                "id": stock["id"]
            }
            delResp = materiaReq(delete)

    # Update stock infos
    for i, (ticker, stock) in enumerate(allstocks.items()):
        if stock["new"]:
            continue
        if stock["amount"] > 0 or stock["goal"] > 0:
            req = {
                "operation": "query",
                "filter": "IS(finance_stock) AND .ticker = \"{}\"".format(ticker)
            }
            resp = materiaReq(req)
            obj = resp["object_list"][0]
            obj["goal"] = stock["goal"]
            if stock["goal"] > 0:
                obj["snp_priority"] = stock["priority"]
            else:
                obj["snp_priority"] = ""
            upd = {
                "operation": "modify",
                "id": obj["id"],
                "params": obj
            }
            updateResp = materiaReq(upd)
            
    transactionString = "SELL: "

    # Calculate total amount of money if we sell
    totalSell = Money("0.00USD", currencyList)
    totalBuy = Money("0.00USD", currencyList)
    for i, (ticker, stock) in enumerate(allstocks.items()):
        if stock["new"]:
            continue
        sellCount = stock["amount"] > stock["goal"]
        if sellCount > 0:
            totalSell = totalSell + stock["price"] * (sellCount)
            transactionString += (str(sellCount) + "x" + stock["ticker"] + " ")

    # Find what we can buy on those money
    allTickers = filter(lambda x: allstocks[x]["priority"] >= 0, allstocks.keys())
    allTickers = sorted(allTickers, key=lambda x: allstocks[x]["priority"])

    anyBuys = False
    for t in allTickers:
        stock = allstocks[t]
        discrepancy = stock["goal"] - stock["amount"]
        print("Discrepancy of " + t + " = " + str(discrepancy))
        while discrepancy > 0:
            if (totalBuy + stock["price"]) < totalSell:
                if anyBuys == False:
                    anyBuys = True
                    transactionString += " BUY: "
                transactionString += t
                discrepancy -= 1
                totalBuy = totalBuy + stock["price"]
            else:
                break

    # Check if we have a sell proposal, commit if we do
    if anyBuys:
        cre = {
            "operation": "create",
            "typename": "calendar_item",
            "params": {
                "text": transactionString,
                "timestamp": int(time.time()),
                "entityTypeChoice": "Task",
                "urgencyChoice": "Urgent"
                }
        }
        creResp = materiaReq(cre)

def main():
    req = {
        "operation": "query",
        "filter": "IS(currency)"
    }
    currencyResp = materiaReq(req)
    currencyList = currencyResp["object_list"]

    total = calculatePEandAddDataPoint(currencyList)
    updateTargets(currencyList, total)
    proposeTrades(currencyList)

if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        req = {
            "operation": "push",
            "listId": "inbox",
            "value": "An error occured during analisys of 'invest': {}".format(format_exception(error))
        }
        materiaReq(req)
        raise
