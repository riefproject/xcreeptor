#include "keyManager.hpp"
#include "encrypt.hpp"
#include "decrypt.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>

using namespace std;

const string KeyManager::keyboardChars = "`1234567890-=~!@#$%^&*()_+[]{}|;:,./<>?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

string KeyManager::encryptKeyData(const string& data, const string& password) {
    // Derive a 32-byte key and 16-byte IV from password
    string key = password;
    key.resize(32, 'x');  // Pad to 32 bytes
    string iv = password.substr(0, 16);
    iv.resize(16, 'x');   // Pad to 16 bytes

    // Encrypt the data
    return Encrypt::encryptAES(data, key, iv);
}

string KeyManager::decryptKeyData(const string& data, const string& password) {
    // Derive same key and IV
    string key = password;
    key.resize(32, 'x');
    string iv = password.substr(0, 16);
    iv.resize(16, 'x');

    // Decrypt the data
    string decoded = Decrypt::base64Decode(data);
    return Decrypt::decryptAES(decoded, key, iv);
}

void KeyManager::saveKeyToFile(const map<char, char>& key, const string& filename, const string& password) {
    try {
        // Create full directory path if needed
        filesystem::path filePath(filename);
        filesystem::create_directories(filePath.parent_path());

        string data;
        for (const auto& pair : key) {
            data.push_back(pair.first);
            data.push_back(pair.second);
        }

        // Open with explicit binary mode
        ofstream file(filePath, ios::binary | ios::out);
        if (!file) {
            throw runtime_error("Could not open file: " + filePath.string());
        }

        string encrypted = encryptKeyData(data, password);
        file.write(encrypted.data(), encrypted.size());
        file.close();
    }
    catch (const exception& e) {
        cerr << "Error saving key: " << e.what() << endl;
        throw;
    }
}

// In KeyManager::loadKeyFromFile
map<char, char> KeyManager::loadKeyFromFile(const string& filename, const string& password) {
    map<char, char> key;
    try {
        // Open with explicit binary mode
        ifstream file(filename, ios::binary | ios::in);
        if (!file) {
            throw runtime_error("Could not open file: " + string(filename));
        }

        // Read binary data properly
        vector<char> buffer(filesystem::file_size(filename));
        file.read(buffer.data(), buffer.size());
        file.close();

        string encrypted(buffer.begin(), buffer.end());
        string decrypted = decryptKeyData(encrypted, password);

        // Parse decrypted data
        if (decrypted.length() % 2 != 0) {
            throw runtime_error("Corrupted key data");
        }

        for (size_t i = 0; i < decrypted.length(); i += 2) {
            key[decrypted[i]] = decrypted[i + 1];
        }

        // Validate mapping
        if (key.size() != keyboardChars.length()) {
            throw runtime_error("Invalid key mapping size");
        }
    }
    catch (const exception& e) {
        cerr << "Error loading key: " << e.what() << endl;
        key = generateKey();
        saveKeyToFile(key, filename, password);
    }
    return key;
}

map<char, char> KeyManager::generateKey() {
    vector<char> chars(keyboardChars.begin(), keyboardChars.end());
    random_device rd;
    mt19937 gen(rd());
    shuffle(chars.begin(), chars.end(), gen);

    map<char, char> key;
    for (size_t i = 0; i < keyboardChars.size(); ++i) {
        key[keyboardChars[i]] = chars[i];
    }
    return key;
}