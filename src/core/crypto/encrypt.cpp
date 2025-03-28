#include "encrypt.hpp"
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
using namespace std;

string Encrypt::encryptString(const map<char, char>& charMapping, const string& input) {
    string encrypted;
    for (char c : input) {
        encrypted += charMapping.at(c);
    }
    return encrypted;
}

string Encrypt::encryptAES(const string& plaintext, const string& key, const string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    unsigned char outbuf[1024];
    int outlen;

    string ciphertext;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str());
    EVP_EncryptUpdate(ctx, outbuf, &outlen, (unsigned char*)plaintext.c_str(), plaintext.size());
    ciphertext.assign((char*)outbuf, outlen);

    EVP_EncryptFinal_ex(ctx, outbuf, &outlen);
    ciphertext.append((char*)outbuf, outlen);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

string Encrypt::base64Encode(const string& input) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_push(b64, bio);

    BIO_write(b64, input.data(), input.size());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bufferPtr);

    string output(bufferPtr->data, bufferPtr->length - 1);
    BIO_free_all(b64);

    return output;
}

string Encrypt::encryptLayered(const map<char, char>& charMapping, const string& input, const string& aesKey, const string& iv) {
    // Layer 1: Substitution cipher
    string layer1 = encryptString(charMapping, input);

    // Layer 2: AES encryption
    string layer2 = encryptAES(layer1, aesKey, iv);

    // Base64 encode the final result
    return base64Encode(layer2);
}