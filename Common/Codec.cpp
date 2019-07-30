#include "Codec.hpp"
#include <iomanip>

using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

Codec::Codec(const std::string password)
{
   EVP_add_cipher(EVP_aes_256_cbc());

   auto hash = sha256(password);
   for(unsigned int i = 0; i < KEY_SIZE; ++i)
   {
      mKeyBuf[i] = hash[i];
   }

   for(unsigned int i = 0; i < BLOCK_SIZE; ++i)
   {
      mIv[i] = hash[i];
   }
}

std::string Codec::encrypt(const std::string& source)
{
   std::string result;

   EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
   int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, mKeyBuf, mIv);
   if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

   // Recovered text expands upto BLOCK_SIZE
   result.resize(source.size() + BLOCK_SIZE);
   int out_len1 = (int)source.size();

   rc = EVP_EncryptUpdate(ctx.get(), (byte*)&result[0], &out_len1, (const byte*)&source[0], (int)source.size());
   if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");

   int out_len2 = (int)result.size() - out_len1;
   rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&result[0]+out_len1, &out_len2);
   if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

   // Set cipher text size now that we know it
   result.resize(out_len1 + out_len2);

   return result;
}

std::string Codec::decrypt(const std::string& source)
{
   std::string result;

   EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
   int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, mKeyBuf, mIv);
   if (rc != 1)
      throw std::runtime_error("EVP_DecryptInit_ex failed");

   // Recovered text contracts upto BLOCK_SIZE
   result.resize(source.size());
   int out_len1 = (int)result.size();

   rc = EVP_DecryptUpdate(ctx.get(), (byte*)&result[0], &out_len1, (const byte*)&source[0], (int)source.size());
   if (rc != 1)
   throw std::runtime_error("EVP_DecryptUpdate failed");

   int out_len2 = (int)result.size() - out_len1;
   rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&result[0]+out_len1, &out_len2);
   if (rc != 1)
   throw std::runtime_error("EVP_DecryptFinal_ex failed");

   // Set recovered text size now that we know it
   result.resize(out_len1 + out_len2);

   return result;
}