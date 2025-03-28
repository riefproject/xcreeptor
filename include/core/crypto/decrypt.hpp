#ifndef DECRYPT_HPP
#define DECRYPT_HPP

#include <string>
#include <map>

class Decrypt {
public:
    static std::string decryptString(const std::map<char, char>& charMapping, const std::string& encrypted);
    static std::string decryptAES(const std::string& ciphertext, const std::string& key, const std::string& iv);
    static std::string base64Decode(const std::string& input);
    static std::string decryptLayered(const std::map<char, char>& charMapping, const std::string& encrypted, const std::string& aesKey, const std::string& iv);
};

#endif