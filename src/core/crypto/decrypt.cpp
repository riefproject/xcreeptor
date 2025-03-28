#include "decrypt.hpp"
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <cstring>
#include <iostream>
using namespace std;

string Decrypt::decryptString(const map<char, char>& charMapping, const string& encrypted) {
    string decrypted;
    for (char c : encrypted) {
        for (const auto& pair : charMapping) {
            if (pair.second == c) {
                decrypted += pair.first;
                break;
            }
        }
    }
    return decrypted;
}

string Decrypt::decryptAES(const string& ciphertext, const string& key, const string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    unsigned char outbuf[1024];
    int outlen;

    string plaintext;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str());
    EVP_DecryptUpdate(ctx, outbuf, &outlen, (unsigned char*)ciphertext.c_str(), ciphertext.size());
    plaintext.assign((char*)outbuf, outlen);

    if (EVP_DecryptFinal_ex(ctx, outbuf, &outlen) > 0) {
        plaintext.append((char*)outbuf, outlen);
    }
    else {
        cerr << "Error: AES decryption failed." << endl;
    }

    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

string Decrypt::base64Decode(const string& input) {
    BIO* bio, * b64;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.data(), input.size());
    BIO_push(b64, bio);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    int decodedLength = BIO_read(b64, buffer, sizeof(buffer));
    if (decodedLength <= 0) {
        cerr << "Error: Base64 Decoding failed. Input: [" << input << "]" << endl;
    }
    BIO_free_all(b64);

    return string(buffer, decodedLength);
}

string Decrypt::decryptLayered(const map<char, char>& charMapping, const string& encrypted, const string& aesKey, const string& iv) {
    // Decode base64
    string decoded = base64Decode(encrypted);

    // Layer 2: Decrypt AES
    string layer2 = decryptAES(decoded, aesKey, iv);

    // Layer 1: Decrypt substitution
    return decryptString(charMapping, layer2);
}