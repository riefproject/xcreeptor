#ifndef ENVMANAGER_HPP
#define ENVMANAGER_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

/**
 * Class for managing environment variables and loading them from .env files
 */
class EnvManager {
public:
    // Don't allow instantiation
    EnvManager() = delete;
    ~EnvManager() = delete;

    // Configuration flags
    enum Options {
        DEFAULT = 0,
        PRESERVE_EXISTING = 1 << 0
    };

    /**
     * Load environment variables from file
     *
     * @param filepath Path to env file (defaults to ".env")
     */
    static void load(const char* filepath = ".env");

    /**
     * Load environment variables with specific options
     *
     * @param options Configuration options
     * @param filepath Path to env file (defaults to ".env")
     */
    static void load(int options, const char* filepath = ".env");

    /**
     * Get environment variable with fallback
     *
     * @param name Environment variable name
     * @param default_value Value to return if variable doesn't exist
     * @return Environment variable value or default
     */
    static std::string get(const char* name, const std::string& default_value = "");

private:
    // Internal implementation
    static void parse_file(int options, const char* filepath);

    // Search for the .env file in various locations
    static std::string find_env_file(const char* filepath);

    // String utilities
    static void trim_left(std::string& s);
    static void trim_right(std::string& s);
    static void trim_both(std::string& s);
    static std::string trim_copy(std::string s);
    static std::string remove_quotes(const std::string& s);

    // Variable expansion
    static std::pair<std::string, bool> expand_variables(const std::string& value, int line_number);
    static size_t find_variable_begin(const std::string& str, size_t pos, std::string& marker);
    static size_t find_variable_end(const std::string& str, size_t pos, const std::string& marker);
};

#endif // ENVMANAGER_HPP