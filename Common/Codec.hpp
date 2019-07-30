#pragma once

#include <string>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

class Codec
{
public:
   Codec(const std::string password);

   std::string encrypt(const std::string& source);
   std::string decrypt(const std::string& source);

private:

   typedef unsigned char byte;
   static const unsigned int KEY_SIZE = 32;
   static const unsigned int BLOCK_SIZE = 16;

   byte mKeyBuf[KEY_SIZE];
   byte mIv[BLOCK_SIZE];
};