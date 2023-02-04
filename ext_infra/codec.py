import base64
import hashlib
from Crypto import Random
from Crypto.Cipher import AES

class AESCipher(object):

    def __init__(self, key):
        self.bs = AES.block_size
        self.key = hashlib.sha256(key.encode('utf-8')).digest()

    def encrypt(self, raw):
        raw = self._pad(raw.encode('utf-8'))
        iv = self.key[:16]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return base64.b64encode(cipher.encrypt(raw))

    def decrypt(self, enc):
        enc = base64.b64decode(enc)
        iv = self.key[:16]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc)).decode('utf-8')

    def _pad(self, s):
        needPaddingBytes = (self.bs - len(s) % self.bs)
        if needPaddingBytes == 0:
            needPaddingBytes = AES.block_size
        return s + (needPaddingBytes * chr(needPaddingBytes)).encode('utf-8')

    @staticmethod
    def _unpad(s):
        return s[:-ord(s[len(s)-1:])]
