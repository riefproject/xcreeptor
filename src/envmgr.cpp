#include "envmgr.hpp"
#include <filesystem>
#include <cstdlib>

// Platform-specific environment handling
#if defined(_MSC_VER) || defined(__MINGW32__)
inline int set_env_var(const char* name, const char* value, bool overwrite) {
    if (!overwrite) {
        if (std::getenv(name)) return 0;
    }
    return _putenv_s(name, value);
}
#else
inline int set_env_var(const char* name, const char* value, bool overwrite) {
    return setenv(name, value, overwrite ? 1 : 0);
}
#endif

void EnvManager::load(const char* filepath) {
    parse_file(DEFAULT, filepath);
}

void EnvManager::load(int options, const char* filepath) {
    parse_file(options, filepath);
}

std::string EnvManager::get(const char* name, const std::string& default_value) {
    const char* value = std::getenv(name);
    return value ? std::string(value) : default_value;
}

void EnvManager::trim_left(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
}

void EnvManager::trim_right(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

void EnvManager::trim_both(std::string& s) {
    trim_left(s);
    trim_right(s);
}

std::string EnvManager::trim_copy(std::string s) {
    trim_both(s);
    return s;
}

std::string EnvManager::remove_quotes(const std::string& s) {
    if (s.length() < 2) return s;

    char first = s.front();
    char last = s.back();

    if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
        return s.substr(1, s.length() - 2);
    }

    return s;
}

size_t EnvManager::find_variable_begin(const std::string& str, size_t pos, std::string& marker) {
    size_t dollar_pos = str.find('$', pos);
    size_t brace_pos = str.find("${", pos);
    size_t var_pos = std::min(dollar_pos, brace_pos);

    if (var_pos != std::string::npos) {
        marker = (var_pos == brace_pos) ? "${" : "$";
    }

    return var_pos;
}

size_t EnvManager::find_variable_end(const std::string& str, size_t pos, const std::string& marker) {
    if (marker == "${") {
        return str.find('}', pos);
    }
    else {
        // For $VAR format, find end by space or end of string
        size_t space_pos = str.find(' ', pos);
        return (space_pos != std::string::npos) ? space_pos : str.length();
    }
}

std::pair<std::string, bool> EnvManager::expand_variables(const std::string& value, int line_number) {
    std::string result;
    size_t current_pos = 0;
    size_t prev_pos = 0;
    int unresolved_vars = 0;

    while (true) {
        std::string marker;
        current_pos = find_variable_begin(value, current_pos, marker);

        if (current_pos == std::string::npos) {
            // No more variables found
            break;
        }

        // Add text before the variable
        result += value.substr(prev_pos, current_pos - prev_pos);

        // Get variable name
        size_t var_start = current_pos + marker.length();
        size_t var_end = find_variable_end(value, var_start, marker);

        if (var_end == std::string::npos) {
            std::cout << "Error at line " << line_number << ": Unterminated variable" << std::endl;
            unresolved_vars++;
            break;
        }

        size_t var_length = var_end - var_start;
        std::string var_name = value.substr(var_start, var_length);
        trim_both(var_name);

        // Expand variable
        const char* env_val = std::getenv(var_name.c_str());
        if (env_val) {
            result += env_val;
        }
        else {
            std::cout << "Warning at line " << line_number << ": Variable $"
                << var_name << " not found" << std::endl;
            unresolved_vars++;
        }

        // Move past the variable
        current_pos = var_end + (marker == "${" ? 1 : 0);
        prev_pos = current_pos;
    }

    // Add remaining text
    if (prev_pos < value.length()) {
        result += value.substr(prev_pos);
    }

    return std::make_pair(result, unresolved_vars == 0);
}

std::string EnvManager::find_env_file(const char* filepath) {
    namespace fs = std::filesystem;

    // Check if path is absolute
    fs::path path(filepath);
    if (path.is_absolute() && fs::exists(path)) {
        return path.string();
    }

    // Try current directory first
    fs::path current_path = fs::current_path() / filepath;
    if (fs::exists(current_path)) {
        return current_path.string();
    }

    // Try executable directory
    fs::path exe_path = fs::path(EnvManager::get("_", "")).parent_path();
    fs::path exe_env_path = exe_path / filepath;
    if (fs::exists(exe_env_path)) {
        return exe_env_path.string();
    }

    // Check parent directories (up to 3 levels)
    fs::path parent_path = fs::current_path();
    for (int i = 0; i < 3; i++) {
        parent_path = parent_path.parent_path();
        fs::path parent_env_path = parent_path / filepath;
        if (fs::exists(parent_env_path)) {
            return parent_env_path.string();
        }
    }

    // Default to the specified path even if it doesn't exist
    return filepath;
}

void EnvManager::parse_file(int options, const char* filepath) {
    std::string resolved_path = find_env_file(filepath);
    std::ifstream file(resolved_path);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open environment file: " << resolved_path << std::endl;
        return;
    }

    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        line_number++;

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Find key-value separator
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            std::cout << "Warning: Skipping invalid line " << line_number
                << " (missing '='): " << line << std::endl;
            continue;
        }

        // Extract and process key/value
        std::string key = trim_copy(line.substr(0, equals_pos));
        std::string raw_value = trim_copy(line.substr(equals_pos + 1));
        std::string value = remove_quotes(raw_value);

        // Expand any variables in the value
        auto [expanded_value, success] = expand_variables(value, line_number);

        if (success) {
            // Set environment variable if it should not be preserved or doesn't exist
            bool should_overwrite = !(options & PRESERVE_EXISTING);
            set_env_var(key.c_str(), expanded_value.c_str(), should_overwrite);
        }
        else {
            std::cout << "Warning: Could not fully resolve variables at line "
                << line_number << std::endl;
        }
    }
}