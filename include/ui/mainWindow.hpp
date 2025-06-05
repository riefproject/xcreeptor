#pragma once
#include "raylib.h"
#include <string>
#include <map>

enum class AppScreen {
    LOGIN,
    CREATE_ACCOUNT,
    MAIN_APP
};

enum class AppPage {
    HOME,
    ENCRYPT,
    DECRYPT,
    GENERATE_PASSWORD
};

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    void run();

private:
    // Core state
    AppScreen currentScreen;
    AppPage currentPage;
    bool shouldClose;
    bool showRegenerateModal;

    // UI Layout
    Rectangle sidebar;
    Rectangle contentArea;
    Rectangle homeCards[4];

    // Encrypt page UI
    Rectangle encryptInputBox;
    Rectangle encryptOutputBox;
    Rectangle encryptButton;
    Rectangle encryptCopyButton;

    // Decrypt page UI
    Rectangle decryptInputBox;
    Rectangle decryptOutputBox;
    Rectangle decryptButton;
    Rectangle decryptCopyButton;

    // Generate password page UI
    Rectangle passwordLengthBox;
    Rectangle generateButton;
    Rectangle passwordOutputBox;
    Rectangle passwordCopyButton;

    // Input states
    bool inputTextBoxActive;
    bool outputTextBoxActive;

    // Buffers
    char inputBuffer[4096];
    char outputBuffer[4096];
    char usernameBuffer[256];
    char pinBuffer[8];
    char passwordLengthBuffer[4];

    // Crypto data
    std::string keyFile;
    std::string keyPassword;
    std::string aesKey;
    std::string iv;
    std::map<char, char> charMapping;  // Changed from unordered_map to map
    int passwordLength;

    // Authentication
    std::string currentUsername;
    std::string currentPin;

    // UI Setup methods
    void setupUI();
    void setupHomeUI();
    void setupEncryptUI();
    void setupDecryptUI();
    void setupGeneratePasswordUI();

    // Render methods
    void renderMainApp();
    void renderSidebar();
    void renderContent();
    void renderHomePage();
    void renderEncryptPage();
    void renderDecryptPage();
    void renderGeneratePasswordPage();
    void renderRegenerateModal();
    void renderOutputArea(Rectangle outputBox);

    // Action methods
    void performEncryption();
    void performDecryption();
    void performPasswordGeneration();
    void handleTextBoxFocus(Rectangle textBox);
    void clearBuffers();
    void copyToClipboard();

    // Authentication methods
    void renderLoginScreen();
    void renderCreateAccountScreen();
    bool handleAuthentication();
    bool processLogin(const char* pin);
    bool processCreateAccount(const char* username, const char* pin);

    // Crypto methods
    void initializeKey();
};