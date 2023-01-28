import hashlib
import requests
import base64
import json
from codec import *

class MateriaConnection:
    def __init__(s, passwd):
        s.codec = AESCipher(passwd)
        s.url = "http://ramsnake.net:5754/api"

    def request(s, req):
        encoded = s.codec.encrypt(req)
        x = requests.post(s.url, data = encoded, headers = { 'Content-Type': 'text/html', 'Content-Length': str(len(encoded)) })
        decoded = s.codec.decrypt(x.content)
        return decoded

    def requestJson(s, req):
        return json.loads(s.request(json.dumps(req)))
