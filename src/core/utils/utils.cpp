#include "utils.hpp"
#include <iostream>
#include <random>
using namespace std;
namespace Utils {
    string generateRandomString(int length) {
        const string characters =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;:,.<>?";
        string randomString;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, characters.size() - 1);

        for (int i = 0; i < length; ++i) {
            randomString += characters[dist(gen)];
        }
        return randomString;
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        size_t last = str.find_last_not_of(" \t\n\r");
        return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
    }
}