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

def sortByTimestamp(x):
    return x["timestamp"]

def updateTargets(currencyList):
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
    #Note: salary is stored in money_v1, i.e. amount of euro cents
    req = {
        "operation": "query",
        "ids": ["financial_report"]
    }
    resp = materiaReq(req)
    salaryPerDay = Money("{}EUR".format(float(resp["object_list"][0]["Salary"]["total"]) / 100), currencyList) / 365

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
    stocksHaveTmp = resp["object_list"]
    stocksHave = {}
    for x in stocksHaveTmp:
        stocksHave[x["ticker"]] = x

    for i, (ticker, stock) in enumerate(stocksHave.items()):
        if(int(stock["amount"]) != 0):
            print("{}: {}".format(ticker, stock["amount"]))

    print("---")

    req = {
        "operation": "query",
        "ids": ["portfolio_goal"]
    }
    resp = materiaReq(req)
    stocksGoalTmp = list(resp["object_list"][0].values())
    stocksGoal = {}
    for x in stocksGoalTmp:
        #Skip incompatible fields
        if x == "object" or x == "portfolio_goal":
            continue

        stocksGoal[x["ticker"]] = x
        stocksGoal[x["ticker"]]["priority"] = int(x["id"])

    for i, (ticker, stock) in enumerate(stocksGoal.items()):
        print("{}: {}".format(ticker, stock["amount"]))

    # Determine stocks to sell
    stocksToSell = []
    for i, (ticker, stock) in enumerate(stocksHave.items()):
        stockCurrentAmount = int(stock["amount"])
        if stockCurrentAmount == 0:
            continue
        if not ticker in stocksGoal:
            stocksToSell.append((ticker, stockCurrentAmount))
        else:
            stockTargetAmount = int(float(stocksGoal[ticker]["amount"])) + 1
            if stockTargetAmount < stockCurrentAmount:
                stocksToSell.append((ticker, stockCurrentAmount - stockTargetAmount))

    # Determine stocks to buy
    stocksToBuy = []
    for i, (ticker, stock) in enumerate(stocksGoal.items()):
        stockTargetAmount = int(float(stock["amount"]))
        if (stockTargetAmount < 1):
            stockTargetAmount = 1
        if not ticker in stocksHave:
            stocksToBuy.append((ticker, stockTargetAmount, stock["priority"]))
        else:
            stockCurrentAmount = int(stocksHave[ticker]["amount"])
            if stockTargetAmount > stockCurrentAmount:
                stocksToBuy.append((ticker, stockTargetAmount - stockCurrentAmount, stock["priority"]))

    # Save usefull info
    discrepancies = {}
    for (ticker,count,priority) in stocksToBuy:
        discrepancies[ticker] = count
    for (ticker,count) in stocksToSell:
        discrepancies[ticker] = -count

    for i, (ticker, count) in enumerate(discrepancies.items()):
        req = {
            "operation": "query",
            "filter": "IS(finance_stock) AND .ticker = \"{}\"".format(ticker)
        }
        resp = materiaReq(req)
        obj = resp["object_list"][0]
        obj["discrepancy"] = count
        if ticker in stocksGoal:
            obj["snp_priority"] = stocksGoal[ticker]["priority"]
        else:
            obj["snp_priority"] = ""
        upd = {
            "operation": "modify",
            "id": obj["id"],
            "params": obj
        }
        updateResp = materiaReq(upd)

    # Sort buy list according to priorities
    stocksToBuy.sort(key=lambda y: y[2])

    # Try to cover each item with as less sells as possible
    itemsToBuy=[]
    itemsToSell=[]
    for (ticker,count) in stocksToSell:
        for i in range (count):
            itemsToSell.append((ticker, Money(stocksHave[ticker]["lastKnownPrice"], currencyList)))

    # Sort sell list, cheapest first
    itemsToSell.sort(key=lambda y: y[1])

    for (ticker,count,priority) in stocksToBuy:
        for i in range (count):
            itemsToBuy.append((ticker, Money(stocksHave[ticker]["lastKnownPrice"], currencyList)))

    posBuy = -1
    posSell = -1
    moneyToBuy = Money("0.00USD", currencyList)
    for (tickerBuy, valueBuy) in itemsToBuy: 
        # Try to cover one extra position
        covered = False

        moneyToBuy = moneyToBuy + valueBuy
        moneyToSell = Money("0.00USD", currencyList)

        curSell = -1
        for (tickerSell, valueSell) in itemsToSell: 
            curSell = curSell + 1
            moneyToSell = moneyToSell + valueSell
            if moneyToSell > moneyToBuy:
                covered = True
                posSell = curSell
                break

        if not covered:
            break
        else:
            posBuy = posBuy + 1

    # Check if we have a transaction proposal, commit if we do
    if posBuy != -1 and posSell != -1:
        buyTickers = [x[0] for x in itemsToBuy[0:posBuy]]
        sellTickers = [x[0] for x in itemsToSell[0:posSell]]
        task = "Sell: {}, buy: {}".format(buyTickers, sellTickers)
        cre = {
            "operation": "create",
            "typename": "calendar_item",
            "params": {
                "timestamp": int(time.time()),
                "entityTypeChoice": "Task",
                "urgencyChoice": "Urgent"
                }
        }
        creResp = materiaReq(cre)

    # Clean up stocks not in snp and for which there are no holdings
    for i, (ticker, stock) in enumerate(stocksHave.items()):
        if not ticker in stocksGoal:
            if int(stock["amount"]) == 0:
                delete = {
                    "operation": "destroy",
                    "id": stock["id"]
                }
                delResp = materiaReq(delete)

def main():
    req = {
        "operation": "query",
        "filter": "IS(currency)"
    }
    currencyResp = materiaReq(req)
    currencyList = currencyResp["object_list"]

    #calculatePEandAddDataPoint(currencyList)
    #updateTargets(currencyList)
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
