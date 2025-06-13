// aes.h
#ifndef AES_H
#define AES_H

#include "config.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>

class AES
{

public:
    AES();
    ~AES();

    void handleErrors();

    vector<unsigned char> encrypt(const std::string& plaintext, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

    string decrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

};

#endif // AES_H