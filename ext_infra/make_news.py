import requests
import json
import os
import praw
import time
import random

class Item():
    pass

def loadPage(uri):
    HEADERS = {
    'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.87 Safari/537.36',
    }
    return requests.get(uri, headers = HEADERS).text

def loadItem(id):
    page = loadPage(f"https://hacker-news.firebaseio.com/v0/item/{id}.json?")
    return json.loads(page)

def composeHtmlList(name, values):
   result = f"<h1>{name}</h1>"

   for x in values:
      result += f"<li>{x}</li>"

   return result

def genHackernewsContent():
    allItems = []

    page = loadPage("https://hacker-news.firebaseio.com/v0/newstories.json?")
    x = f"{{\"value\":{page}}}"
    items = json.loads(x)
    for x in items["value"]:
        try:
            item = loadItem(x)
            if item["score"] > 100:
                if "url" in item:
                    print("+")
                    url = item["url"]
                    title = item["title"]
                    allItems.append(f"<a href=\"{url}\">{title}</a>")
                    if len(allItems) > 25:
                        break
        except:
            pass

    return composeHtmlList("HackerNews", allItems)

def genSnpContent():
    result = []
    page = loadPage("https://www.slickcharts.com/sp500")
    idx = 0
    start = 0
    to = 0

    for i in range(1, 21):
        search = f"<td>{i}</td>"
        idx = page.index(search, idx)

        start = page.index("\">", idx)
        to = page.index("</a></td>", start)
        company = page[start + 2: to]
        idx = to

        start = page.index("\">", idx)
        to = page.index("</a></td>", start)
        ticker = page[start + 2: to]
        idx = to

        start = page.index("<td>", idx)
        to = page.index("</td>", start)
        weight = float(page[start + 4: to].replace(",", ""))
        idx = to

        start = page.index("&nbsp;&nbsp;", idx)
        to = page.index("</td>", start)
        price = float(page[start + 12: to].replace(",", ""))
        idx = to
        
        item = Item()
        item.company = company
        item.ticker = ticker
        item.weight = weight
        item.price = price
        result.append(item)

    return result

def publishSnpContent(content, mr):
    contentJson = "{"
    i = 0
    for x in content:
        if i != 0:
            contentJson += ","

        contentJson += f"\"{i}\": {{"
        contentJson += f"\"id\": \"{i}\","
        contentJson += f"\"company\": \"{x.company}\","
        contentJson += f"\"ticker\": \"{x.ticker}\","
        contentJson += f"\"weight\": {x.weight},"
        contentJson += f"\"price\": {x.price}"
        contentJson += "}"

        i = i + 1

    contentJson += "}"

    op = "{\"operation\":\"destroy\", \"id\":\"data.snp\"}"
    mr.request(op)

    op = f"{{\"operation\":\"create\", \"typename\":\"object\", \"defined_id\":\"data.snp\", \"params\":{contentJson}}}"
    mr.request(op)

def parseNltimesPage(page):
    result = []
    count = 0
    curPos = 0

    while True:
        start = page.find("news-card__title", curPos)

        if start == -1:
            break

        startIndex = page.find("<a href=\"", start)
        endIndex = page.find("</a>", start)

        ref = page[startIndex: endIndex + 4]
        result.append(ref)

        curPos = endIndex + 1
        count = count + 1
        if count > 10:
            break

    return result

def genNlTimesContent():
    r = loadPage("https://nltimes.nl")
    allEntries = parseNltimesPage(r)
    return composeHtmlList("NL News", allEntries) 

def genContentForSubreddit(name, reddit):
    sr = reddit.subreddit(name)
    items = sr.new()
    result = []
    for x in items:
        if time.time() - x.created_utc < 86400:
            result.append(x)

    result.sort(reverse=True, key=lambda x: x.score)
    result = result[:25]

    return composeHtmlList(name, map(lambda x: f"<a href=\"{x.url}\">{x.title}</a>", result))

def loadSubreddits(mr):
    op = "{\"operation\":\"query\", \"ids\":[\"config.news\"]}"
    res = json.loads(mr.request(op))

    return res["object_list"][0]["reddit"].split(";")

def genRedditContent(mr):
   reddit = praw.Reddit(
    client_id="DVOqwIAoJxN5NA",
    client_secret="1pfVk4fZ1mwSct2fL-72GbMpF-E",
    password="rtff6#yo",
    user_agent="snake.materia.newsreader",
    username="theramsnake",
   )

   subreddits = loadSubreddits(mr)

   result = ""
   for x in subreddits:
     result += genContentForSubreddit(x, reddit)   

   return result

def extractGames(page):
    result = []

    beginPos = page.find("search_result_container")
    endPos = page.find("search_pagination\">")

    curPos = beginPos
    while True:
        newIndex = page.find("<a href=\"https://store.steampowered.com/app", curPos)

        if newIndex > endPos or newIndex == -1:
            break

        appIndex = page.find("app/", newIndex)
        endAppIndex = page.find("/?", newIndex)

        title = page[appIndex + 4 : endAppIndex]
        result.append(page[newIndex: page.find("\"", newIndex + 20) + 1] + f">{title}</a>")

        curPos = newIndex + 1

    return result

def genNewVrGamesContent():
    allNewVrGames = []
    for x in range(5):
        r = loadPage("https://store.steampowered.com/search/?sort_by=Released_DESC&vrsupport=402&page=$x")
        allNewVrGames.extend(extractGames(r))
    
    random.shuffle(allNewVrGames)
    return composeHtmlList("New VR games", allNewVrGames[:5]) 

def genTopVrGamesContent():
    allVrGames = []
    for x in range(5):
        r = loadPage("https://store.steampowered.com/search/?vrsupport=401&filter=topsellers&page=$x")
        allVrGames.extend(extractGames(r))
    
    random.shuffle(allVrGames)
    return composeHtmlList("Top VR games", allVrGames[:5]) 

def genNewGamesContent():
    allVrGames = []
    r = loadPage("https://store.steampowered.com/search/?sort_by=Released_DESC&os=win&filter=popularnew&page=1")
    allVrGames.extend(extractGames(r))
    
    random.shuffle(allVrGames)
    return composeHtmlList("Top nonVR games", allVrGames[:3]) 

def genSteamContent():
    result = ""

    result += genNewVrGamesContent()
    result += genTopVrGamesContent()
    result += genNewGamesContent()
    
    return result

def genNewsFile(mr):
    filename = "news.tmp"

    filecontent = "<head><style>a { text-decoration: none;} </style> </head><body>"

    filecontent += genNlTimesContent()
    filecontent += genRedditContent(mr)
    filecontent += genHackernewsContent()
    filecontent += genSteamContent()

    filecontent += "</body>"

    file = open(filename, "w", encoding='utf8') 
    file.write(filecontent) 
    file.close()

    return filename

def publishFile(filename, mr):
    file = open(filename, mode='r', encoding='utf8')
    lines = file.read()
    file.close()

    lines = lines.replace("\"", "\\\"")

    op = "{\"operation\":\"destroy\", \"id\":\"newsfeed\"}"
    mr.request(op)

    op = f"{{\"operation\":\"create\", \"typename\":\"object\", \"defined_id\":\"newsfeed\", \"params\":{{\"content\":\"{lines}\"}}}}"
    mr.request(op)

    os.remove(filename)

def do(mr):
    filename = genNewsFile(mr)
    publishFile(filename, mr)

    snpContent = genSnpContent()
    publishSnpContent(snpContent, mr)
