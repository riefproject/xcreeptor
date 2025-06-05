#define RAYGUI_IMPLEMENTATION
#include "mainWindow.hpp"
#include "envmgr.hpp"
#include "encrypt.hpp"
#include "decrypt.hpp"
#include "keyManager.hpp"
#include "utils.hpp"
#include "account.hpp"
#include "raylib.h"
#include "raygui.h"
#include <fstream>
#include <filesystem>
#include <cstring>

MainWindow::MainWindow()
    : currentScreen(AppScreen::LOGIN),
    currentPage(AppPage::HOME),
    shouldClose(false),
    keyFile("assets/key.dat"),
    passwordLength(12),
    inputTextBoxActive(false),
    outputTextBoxActive(false),
    showRegenerateModal(false) {

    // Load environment variables first
    EnvManager::load();

    // Then get the values
    // You should set  these environment variables in your .env file or system environment
    keyPassword = EnvManager::get("XCREEPTOR_PASS_KEY");
    aesKey = EnvManager::get("XCREEPTOR_AES_KEY");
    iv = EnvManager::get("XCREEPTOR_VI_KEY");

    // Clear text buffers
    memset(inputBuffer, 0, sizeof(inputBuffer));
    memset(outputBuffer, 0, sizeof(outputBuffer));
    memset(usernameBuffer, 0, sizeof(usernameBuffer));
    memset(pinBuffer, 0, sizeof(pinBuffer));
    memset(passwordLengthBuffer, 0, sizeof(passwordLengthBuffer));
    strcpy(passwordLengthBuffer, "12");

    // Initialize Raylib
    InitWindow(900, 650, "Xcreeptor - Encryption Tool");
    SetTargetFPS(60);

    // Setup UI layout
    setupUI();

    // Initialize key
    initializeKey();

    // Handle authentication
    handleAuthentication();
}

MainWindow::~MainWindow() {
    CloseWindow();
}

void MainWindow::run() {
    while (!WindowShouldClose() && !shouldClose) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen) {
        case AppScreen::LOGIN:
            renderLoginScreen();
            break;
        case AppScreen::CREATE_ACCOUNT:
            renderCreateAccountScreen();
            break;
        case AppScreen::MAIN_APP:
            renderMainApp();
            break;
        }

        // Render modal if needed
        if (showRegenerateModal) {
            renderRegenerateModal();
        }

        EndDrawing();
    }
}

void MainWindow::setupUI() {
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();

    // Sidebar
    sidebar = { 0, 0, 200, (float)windowHeight };

    // Main content area
    contentArea = { 200, 0, (float)(windowWidth - 200), (float)windowHeight };

    // Setup page-specific UI elements
    setupHomeUI();
    setupEncryptUI();
    setupDecryptUI();
    setupGeneratePasswordUI();
}

void MainWindow::setupHomeUI() {
    float cardWidth = 150;
    float cardHeight = 120;
    float margin = 20;

    // Calculate positions for 2x2 grid
    float startX = contentArea.x + (contentArea.width - (cardWidth * 2 + margin)) / 2;
    float startY = contentArea.y + 100;

    homeCards[0] = { startX, startY, cardWidth, cardHeight }; // Encrypt
    homeCards[1] = { startX + cardWidth + margin, startY, cardWidth, cardHeight }; // Decrypt
    homeCards[2] = { startX, startY + cardHeight + margin, cardWidth, cardHeight }; // Generate Password
    homeCards[3] = { startX + cardWidth + margin, startY + cardHeight + margin, cardWidth, cardHeight }; // Regenerate Key
}

void MainWindow::setupEncryptUI() {
    float margin = 20;
    float textBoxHeight = 200;

    encryptInputBox = { contentArea.x + margin, contentArea.y + 80,
                       contentArea.width - margin * 2, textBoxHeight };

    encryptOutputBox = { contentArea.x + margin, contentArea.y + 320,
                        contentArea.width - margin * 2, textBoxHeight };

    encryptButton = { contentArea.x + margin, contentArea.y + 540, 120, 30 };
    encryptCopyButton = { contentArea.x + 160, contentArea.y + 540, 80, 30 };
}

void MainWindow::setupDecryptUI() {
    float margin = 20;
    float textBoxHeight = 200;

    decryptInputBox = { contentArea.x + margin, contentArea.y + 80,
                       contentArea.width - margin * 2, textBoxHeight };

    decryptOutputBox = { contentArea.x + margin, contentArea.y + 320,
                        contentArea.width - margin * 2, textBoxHeight };

    decryptButton = { contentArea.x + margin, contentArea.y + 540, 120, 30 };
    decryptCopyButton = { contentArea.x + 160, contentArea.y + 540, 80, 30 };
}

void MainWindow::setupGeneratePasswordUI() {
    float margin = 20;

    passwordLengthBox = { contentArea.x + margin, contentArea.y + 100, 150, 30 };
    generateButton = { contentArea.x + margin, contentArea.y + 150, 150, 30 };

    passwordOutputBox = { contentArea.x + margin, contentArea.y + 200,
                         contentArea.width - margin * 2, 200 };

    passwordCopyButton = { contentArea.x + margin, contentArea.y + 420, 100, 30 };
}

void MainWindow::renderMainApp() {
    renderSidebar();
    renderContent();
}

void MainWindow::renderSidebar() {
    // Sidebar background
    DrawRectangleRec(sidebar, LIGHTGRAY);
    DrawRectangleLinesEx(sidebar, 1, GRAY);

    // Title
    GuiLabel({ sidebar.x + 10, sidebar.y + 20, 180, 30 }, "Xcreeptor");

    // Navigation buttons
    Rectangle homeBtn = { sidebar.x + 10, sidebar.y + 70, 180, 40 };
    Rectangle encryptBtn = { sidebar.x + 10, sidebar.y + 120, 180, 40 };
    Rectangle decryptBtn = { sidebar.x + 10, sidebar.y + 170, 180, 40 };
    Rectangle genPassBtn = { sidebar.x + 10, sidebar.y + 220, 180, 40 };
    Rectangle regenKeyBtn = { sidebar.x + 10, sidebar.y + 270, 180, 40 };

    if (GuiButton(homeBtn, "Home")) {
        currentPage = AppPage::HOME;
        clearBuffers();
    }

    if (GuiButton(encryptBtn, "Encrypt")) {
        currentPage = AppPage::ENCRYPT;
        clearBuffers();
    }

    if (GuiButton(decryptBtn, "Decrypt")) {
        currentPage = AppPage::DECRYPT;
        clearBuffers();
    }

    if (GuiButton(genPassBtn, "Generate Password")) {
        currentPage = AppPage::GENERATE_PASSWORD;
        clearBuffers();
    }

    if (GuiButton(regenKeyBtn, "Regenerate Key")) {
        showRegenerateModal = true;
    }

    // User info at bottom
    std::string userInfo = "User: " + currentUsername;
    GuiLabel({ sidebar.x + 10, sidebar.y + sidebar.height - 50, 180, 30 }, userInfo.c_str());
}

void MainWindow::renderContent() {
    // Content area background
    DrawRectangleRec(contentArea, RAYWHITE);

    switch (currentPage) {
    case AppPage::HOME:
        renderHomePage();
        break;
    case AppPage::ENCRYPT:
        renderEncryptPage();
        break;
    case AppPage::DECRYPT:
        renderDecryptPage();
        break;
    case AppPage::GENERATE_PASSWORD:
        renderGeneratePasswordPage();
        break;
    }
}

void MainWindow::renderHomePage() {
    // Page title
    GuiLabel({ contentArea.x + 20, contentArea.y + 20, 400, 40 }, "Welcome to Xcreeptor");
    GuiLabel({ contentArea.x + 20, contentArea.y + 50, 500, 30 }, "Choose a feature to get started:");

    // Feature cards
    const char* cardTitles[] = { "Encrypt Text", "Decrypt Text", "Generate Password", "Regenerate Key" };
    const char* cardDescriptions[] = {
        "Encrypt your text\nwith advanced\nencryption",
        "Decrypt your\nencrypted text\nback to original",
        "Generate secure\nrandom passwords\nwith custom length",
        "Generate new\nencryption key\n(WARNING: Old data\nbecomes unreadable)"
    };

    for (int i = 0; i < 4; i++) {
        // Card background
        DrawRectangleRec(homeCards[i], WHITE);
        DrawRectangleLinesEx(homeCards[i], 2, DARKGRAY);

        // Card title
        Rectangle titleRect = { homeCards[i].x + 5, homeCards[i].y + 5, homeCards[i].width - 10, 25 };
        GuiLabel(titleRect, cardTitles[i]);

        // Card description
        Rectangle descRect = { homeCards[i].x + 5, homeCards[i].y + 30, homeCards[i].width - 10, 60 };
        GuiLabel(descRect, cardDescriptions[i]);

        // Check for card clicks
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, homeCards[i])) {
                switch (i) {
                case 0: currentPage = AppPage::ENCRYPT; break;
                case 1: currentPage = AppPage::DECRYPT; break;
                case 2: currentPage = AppPage::GENERATE_PASSWORD; break;
                case 3: showRegenerateModal = true; break;
                }
                clearBuffers();
            }
        }
    }
}

void MainWindow::renderEncryptPage() {
    // Page title
    GuiLabel({ contentArea.x + 20, contentArea.y + 20, 300, 30 }, "Encrypt Text");

    // Handle focus management
    handleTextBoxFocus(encryptInputBox);

    // Input text box
    GuiLabel({ encryptInputBox.x, encryptInputBox.y - 25, 200, 20 }, "Input Text:");
    if (GuiTextBox(encryptInputBox, inputBuffer, sizeof(inputBuffer), inputTextBoxActive)) {
        inputTextBoxActive = true;
    }

    // Output area
    GuiLabel({ encryptOutputBox.x, encryptOutputBox.y - 25, 200, 20 }, "Encrypted Text:");
    renderOutputArea(encryptOutputBox);

    // Action buttons
    if (GuiButton(encryptButton, "Encrypt")) {
        performEncryption();
    }

    if (GuiButton(encryptCopyButton, "Copy")) {
        copyToClipboard();
    }
}

void MainWindow::renderDecryptPage() {
    // Page title
    GuiLabel({ contentArea.x + 20, contentArea.y + 20, 300, 30 }, "Decrypt Text");

    // Handle focus management
    handleTextBoxFocus(decryptInputBox);

    // Input text box
    GuiLabel({ decryptInputBox.x, decryptInputBox.y - 25, 200, 20 }, "Encrypted Text:");
    if (GuiTextBox(decryptInputBox, inputBuffer, sizeof(inputBuffer), inputTextBoxActive)) {
        inputTextBoxActive = true;
    }

    // Output area
    GuiLabel({ decryptOutputBox.x, decryptOutputBox.y - 25, 200, 20 }, "Decrypted Text:");
    renderOutputArea(decryptOutputBox);

    // Action buttons
    if (GuiButton(decryptButton, "Decrypt")) {
        performDecryption();
    }

    if (GuiButton(decryptCopyButton, "Copy")) {
        copyToClipboard();
    }
}

void MainWindow::renderGeneratePasswordPage() {
    // Page title
    GuiLabel({ contentArea.x + 20, contentArea.y + 20, 300, 30 }, "Generate Password");

    // Password length input
    GuiLabel({ passwordLengthBox.x, passwordLengthBox.y - 25, 200, 20 }, "Password Length:");
    if (GuiTextBox(passwordLengthBox, passwordLengthBuffer, sizeof(passwordLengthBuffer), false)) {
        // Update password length from buffer
        passwordLength = atoi(passwordLengthBuffer);
        if (passwordLength < 4) passwordLength = 4;
        if (passwordLength > 128) passwordLength = 128;
        sprintf(passwordLengthBuffer, "%d", passwordLength);
    }

    // Generate button
    if (GuiButton(generateButton, "Generate Password")) {
        performPasswordGeneration();
    }

    // Output area
    GuiLabel({ passwordOutputBox.x, passwordOutputBox.y - 25, 200, 20 }, "Generated Password:");
    renderOutputArea(passwordOutputBox);

    // Copy button
    if (GuiButton(passwordCopyButton, "Copy")) {
        copyToClipboard();
    }
}

void MainWindow::handleTextBoxFocus(Rectangle textBox) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        inputTextBoxActive = CheckCollisionPointRec(mousePos, textBox);
    }
}

void MainWindow::renderOutputArea(Rectangle outputBox) {
    // Draw read-only textbox background
    DrawRectangleRec(outputBox, LIGHTGRAY);
    DrawRectangleLinesEx(outputBox, 1, GRAY);

    // Draw text with proper padding
    Vector2 textPos = { outputBox.x + 10, outputBox.y + 10 };
    int fontSize = 10;

    // Simple text rendering with line wrapping
    std::string text(outputBuffer);
    float maxWidth = outputBox.width - 20;
    float currentY = textPos.y;
    float lineHeight = fontSize + 2;

    size_t pos = 0;
    while (pos < text.length() && currentY + lineHeight < outputBox.y + outputBox.height - 10) {
        std::string line;
        size_t lineEnd = text.find('\n', pos);
        if (lineEnd == std::string::npos) lineEnd = text.length();

        line = text.substr(pos, lineEnd - pos);

        // Simple word wrapping
        while (!line.empty() && currentY + lineHeight < outputBox.y + outputBox.height - 10) {
            std::string segment = line;
            while (MeasureText(segment.c_str(), fontSize) > maxWidth && segment.length() > 1) {
                segment = segment.substr(0, segment.length() - 1);
            }

            DrawText(segment.c_str(), (int)textPos.x, (int)currentY, fontSize, BLACK);
            currentY += lineHeight;

            if (segment.length() < line.length()) {
                line = line.substr(segment.length());
            }
            else {
                break;
            }
        }

        pos = lineEnd + 1;
    }
}

void MainWindow::performEncryption() {
    std::string input(inputBuffer);
    if (input.empty()) return;

    std::string encrypted = Encrypt::encryptLayered(charMapping, input, aesKey, iv);
    strncpy(outputBuffer, encrypted.c_str(), sizeof(outputBuffer) - 1);
    outputBuffer[sizeof(outputBuffer) - 1] = '\0';
}

void MainWindow::performDecryption() {
    std::string input(inputBuffer);
    if (input.empty()) return;

    try {
        std::string decrypted = Decrypt::decryptLayered(charMapping, input, aesKey, iv);
        strncpy(outputBuffer, decrypted.c_str(), sizeof(outputBuffer) - 1);
        outputBuffer[sizeof(outputBuffer) - 1] = '\0';
    }
    catch (const std::exception& e) {
        std::string error = "Decryption failed: Invalid data or wrong key";
        strncpy(outputBuffer, error.c_str(), sizeof(outputBuffer) - 1);
        outputBuffer[sizeof(outputBuffer) - 1] = '\0';
    }
}

void MainWindow::performPasswordGeneration() {
    std::string password = Utils::generateRandomString(passwordLength);
    std::string output = "Generated Password: " + password + "\n\n";

    std::string substitutionEncrypted = Encrypt::encryptString(charMapping, password);
    output += "Substitution Encrypted: " + substitutionEncrypted + "\n\n";

    std::string aesEncrypted = Encrypt::encryptAES(password, aesKey, iv);
    aesEncrypted = Encrypt::base64Encode(aesEncrypted);
    output += "AES Encrypted: " + aesEncrypted;

    strncpy(outputBuffer, output.c_str(), sizeof(outputBuffer) - 1);
    outputBuffer[sizeof(outputBuffer) - 1] = '\0';
}

void MainWindow::clearBuffers() {
    memset(inputBuffer, 0, sizeof(inputBuffer));
    memset(outputBuffer, 0, sizeof(outputBuffer));
    inputTextBoxActive = false;
}

void MainWindow::copyToClipboard() {
    SetClipboardText(outputBuffer);
}

void MainWindow::renderRegenerateModal() {
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();

    // Semi-transparent background
    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));

    // Modal dialog
    Rectangle modalRect = { (float)(windowWidth / 2 - 250), (float)(windowHeight / 2 - 120), 500.0f, 240.0f };

    // Modal background
    DrawRectangleRec(modalRect, RAYWHITE);
    DrawRectangleLinesEx(modalRect, 2, BLACK);

    // Title
    GuiLabel({ modalRect.x + 20, modalRect.y + 20, 460, 30 }, "Regenerate Encryption Key");

    // Warning message
    const char* warningText = "This will generate a new encryption key.\n\nAll previously encrypted data will become unreadable.\n\nThis action cannot be undone.\n\nAre you sure you want to continue?";
    GuiLabel({ modalRect.x + 20, modalRect.y + 60, 460, 120 }, warningText);

    // Buttons
    Rectangle yesButton = { modalRect.x + 80, modalRect.y + 190, 150, 35 };
    Rectangle noButton = { modalRect.x + 270, modalRect.y + 190, 150, 35 };

    if (GuiButton(yesButton, "Yes, Regenerate")) {
        // Proceed with regeneration
        charMapping = KeyManager::generateKey();
        KeyManager::saveKeyToFile(charMapping, keyFile, keyPassword);

        // Show success message
        std::string successMsg = "Encryption key regenerated successfully!\nAll new encryptions will use the new key.\nPreviously encrypted data is now unreadable.";
        strncpy(outputBuffer, successMsg.c_str(), sizeof(outputBuffer) - 1);
        outputBuffer[sizeof(outputBuffer) - 1] = '\0';

        showRegenerateModal = false;
        currentPage = AppPage::HOME;
    }

    if (GuiButton(noButton, "Cancel")) {
        showRegenerateModal = false;
    }

    // Close modal on ESC key
    if (IsKeyPressed(KEY_ESCAPE)) {
        showRegenerateModal = false;
    }
}

void MainWindow::renderLoginScreen() {
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();

    GuiLabel({ (float)(windowWidth / 2 - 150), 50.0f, 300.0f, 30.0f }, "Login");

    // Display username
    std::string usernameLabel = "User: " + currentUsername;
    GuiLabel({ (float)(windowWidth / 2 - 150), 100.0f, 300.0f, 30.0f }, usernameLabel.c_str());

    // PIN input
    GuiLabel({ (float)(windowWidth / 2 - 150), 140.0f, 300.0f, 30.0f }, "Enter PIN:");
    if (GuiTextBox({ (float)(windowWidth / 2 - 150), 170.0f, 300.0f, 30.0f }, pinBuffer, 7, true)) {
        // PIN input active
    }

    // Login button
    if (GuiButton({ (float)(windowWidth / 2 - 150), 220.0f, 140.0f, 30.0f }, "Login")) {
        if (processLogin(pinBuffer)) {
            currentScreen = AppScreen::MAIN_APP;
            memset(pinBuffer, 0, sizeof(pinBuffer));
        }
    }

    // Cancel button
    if (GuiButton({ (float)(windowWidth / 2 + 10), 220.0f, 140.0f, 30.0f }, "Cancel")) {
        shouldClose = true;
    }
}

void MainWindow::renderCreateAccountScreen() {
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();

    GuiLabel({ (float)(windowWidth / 2 - 150), 50.0f, 300.0f, 30.0f }, "Create Account");

    // Username input
    GuiLabel({ (float)(windowWidth / 2 - 150), 100.0f, 300.0f, 30.0f }, "Username:");
    if (GuiTextBox({ (float)(windowWidth / 2 - 150), 130.0f, 300.0f, 30.0f }, usernameBuffer, 256, false)) {
        // Username input active
    }

    // PIN input
    GuiLabel({ (float)(windowWidth / 2 - 150), 170.0f, 300.0f, 30.0f }, "PIN (6 digits):");
    if (GuiTextBox({ (float)(windowWidth / 2 - 150), 200.0f, 300.0f, 30.0f }, pinBuffer, 7, true)) {
        // PIN input active
    }

    // Create button
    if (GuiButton({ (float)(windowWidth / 2 - 150), 250.0f, 140.0f, 30.0f }, "Create")) {
        if (processCreateAccount(usernameBuffer, pinBuffer)) {
            currentScreen = AppScreen::MAIN_APP;
            memset(pinBuffer, 0, sizeof(pinBuffer));
            memset(usernameBuffer, 0, sizeof(usernameBuffer));
        }
    }

    // Cancel button
    if (GuiButton({ (float)(windowWidth / 2 + 10), 250.0f, 140.0f, 30.0f }, "Cancel")) {
        shouldClose = true;
    }
}

bool MainWindow::handleAuthentication() {
    if (!std::filesystem::exists("assets/account.dat")) {
        currentScreen = AppScreen::CREATE_ACCOUNT;
        return true;
    }

    try {
        currentUsername = Account::loadAccount();
        currentScreen = AppScreen::LOGIN;
        return true;
    }
    catch (...) {
        // Show error message
        return false;
    }
}

bool MainWindow::processLogin(const char* pin) {
    std::string pinStr(pin);

    if (pinStr.empty()) {
        // Show error message using a modal dialog in Raygui
        return false;
    }

    if (Account::verifyAccount(currentUsername, pinStr)) {
        currentPin = pinStr;
        return true;
    }

    // Show invalid PIN message
    return false;
}

bool MainWindow::processCreateAccount(const char* username, const char* pin) {
    std::string usernameStr(username);
    std::string pinStr(pin);

    // Validate PIN (6 digits)
    if (pinStr.length() != 6) {
        // Show error message
        return false;
    }

    for (char c : pinStr) {
        if (!isdigit(c)) {
            // Show error message
            return false;
        }
    }

    if (Account::createAccount(usernameStr, pinStr)) {
        currentUsername = usernameStr;
        currentPin = pinStr;
        return true;
    }

    // Show error message
    return false;
}

void MainWindow::initializeKey() {
    std::ifstream testFile(keyFile);
    if (!testFile) {
        charMapping = KeyManager::generateKey();
        KeyManager::saveKeyToFile(charMapping, keyFile, keyPassword);
    }
    else {
        charMapping = KeyManager::loadKeyFromFile(keyFile, keyPassword);
    }
}