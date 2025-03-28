#ifndef ENCRYPT_HPP
#define ENCRYPT_HPP

#include <string>
#include <map>

class Encrypt {
public:
    static std::string encryptString(const std::map<char, char>& charMapping, const std::string& input);
    static std::string encryptAES(const std::string& plaintext, const std::string& key, const std::string& iv);
    static std::string base64Encode(const std::string& input);
    static std::string encryptLayered(const std::map<char, char>& charMapping, const std::string& input, const std::string& aesKey, const std::string& iv);
};

#endif