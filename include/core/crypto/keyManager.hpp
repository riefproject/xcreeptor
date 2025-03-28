#ifndef KEYMANAGER_HPP
#define KEYMANAGER_HPP

#include <map>
#include <string>

class KeyManager {
public:
    static void saveKeyToFile(const std::map<char, char>& key, const std::string& filename, const std::string& password);
    static std::map<char, char> loadKeyFromFile(const std::string& filename, const std::string& password);
    static std::map<char, char> generateKey();
private:
    static const std::string keyboardChars;
    static std::string encryptKeyData(const std::string& data, const std::string& password);
    static std::string decryptKeyData(const std::string& data, const std::string& password);
};

#endif