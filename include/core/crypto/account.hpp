#pragma once
#include <string>

class Account {
public:
    static bool verifyAccount(const std::string& username, const std::string& pin);
    static bool createAccount(const std::string& username, const std::string& pin);
    static std::string loadAccount();
private:
    static const std::string accountFile;
};