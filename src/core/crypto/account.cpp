#include "account.hpp"
#include "utils.hpp"
#include <openssl/sha.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
namespace fs = filesystem;

const string Account::accountFile = "assets/account.dat";
string hashPin(const string& pin) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)pin.c_str(), pin.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    // Debugging: Print the generated hash
    cout << "Hash for PIN '" << pin << "': " << ss.str() << endl;

    return ss.str();
}

bool Account::createAccount(const string& username, const string& pin) {
    try {
        fs::create_directories("assets");

        // Hash the PIN
        string hashedPin = hashPin(pin);

        // Format: hashedPin\nusername
        ofstream file(accountFile);
        if (!file) {
            cerr << "Failed to open account file for writing" << endl;
            return false;
        }

        file << hashedPin << "\n" << username;
        file.close();

        return true;
    }
    catch (const exception& e) {
        cerr << "Create account error: " << e.what() << endl;
        return false;
    }
}
bool Account::verifyAccount(const string& username, const string& pin) {
    try {
        ifstream file(accountFile);
        if (!file) {
            cerr << "Failed to open account file for reading" << endl;
            return false;
        }

        string storedHashedPin, storedUser;
        getline(file, storedHashedPin);
        getline(file, storedUser);
        file.close();

        // Trim the username read from the file
        storedUser = Utils::trim(storedUser);

        // Debugging: Print the data read from the file
        cout << "Stored Hashed PIN from file: " << storedHashedPin << endl;
        cout << "Stored Username from file (trimmed): " << storedUser << endl;

        // Hash the entered PIN
        string inputHashedPin = hashPin(pin);

        // Debugging: Print the hash of the entered PIN
        cout << "Input Hashed PIN: " << inputHashedPin << endl;

        // Verification
        bool isValid = (username == storedUser && inputHashedPin == storedHashedPin);
        cout << "Verification result: " << (isValid ? "SUCCESS" : "FAILURE") << endl;

        return isValid;
    }
    catch (const exception& e) {
        cerr << "Verify account error: " << e.what() << endl;
        return false;
    }
}

string Account::loadAccount() {
    ifstream file(accountFile);
    if (!file) throw runtime_error("No account found");

    string storedHashedPin, storedUser;
    getline(file, storedHashedPin);
    getline(file, storedUser);
    file.close();

    return storedUser; // Only return the username
}