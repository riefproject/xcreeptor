#include "mainWindow.hpp"
#include "encrypt.hpp"
#include "decrypt.hpp"
#include "keyManager.hpp"
#include "utils.hpp"
#include "account.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QWidget>
#include <QInputDialog>
#include <fstream>
#include <filesystem>
using namespace std;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    keyFile("assets/key.dat"),
    keyPassword("YourSecretKeyPassword123"),
    aesKey("thisisaverysecurekey1234567890"),
    iv("thisisinitialvect") {

    // Check authentication first before doing anything else
    if (!handleAuthentication()) {
        // If authentication fails, close the application
        QTimer::singleShot(0, qApp, &QApplication::quit);
        return;
    }

    setupUI();
    initializeKey();
}

bool MainWindow::handleAuthentication() {
    if (!filesystem::exists("assets/account.dat")) {
        return showCreateAccountDialog();
    }
    return showLoginDialog();
}

bool MainWindow::showLoginDialog() {
    bool ok;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;

    // Load existing account data
    try {
        currentUsername = QString::fromStdString(Account::loadAccount());
    }
    catch (...) {
        QMessageBox::critical(this, "Error", "Failed to load account data");
        return false;
    }

    while (attempts < MAX_ATTEMPTS) {
        QString pin = QInputDialog::getText(this, "Login",
            "Enter PIN for user " + currentUsername + ":",
            QLineEdit::Password, "", &ok);

        if (!ok || pin.isEmpty()) {
            return false;
        }

        if (Account::verifyAccount(currentUsername.toStdString(), pin.toStdString())) {
            currentPin = pin;
            return true;
        }

        attempts++;
        if (attempts < MAX_ATTEMPTS) {
            QMessageBox::warning(this, "Error",
                QString("Invalid PIN. %1 attempts remaining").arg(MAX_ATTEMPTS - attempts));
        }
        else {
            QMessageBox::critical(this, "Error",
                "Too many failed attempts. Application will now exit.");
            return false;
        }
    }
    return false;
}

bool MainWindow::showCreateAccountDialog() {
    bool ok;
    QString username = QInputDialog::getText(this, "Create Account",
        "Enter username:", QLineEdit::Normal, "", &ok);

    if (!ok || username.isEmpty()) {
        return false;
    }

    QString pin;
    while (true) {
        pin = QInputDialog::getText(this, "Create Account",
            "Enter PIN (6 digits):", QLineEdit::Password, "", &ok);

        if (!ok) return false;

        bool validPin = true;
        if (pin.length() != 6) {
            validPin = false;
        }
        for (QChar c : pin) {
            if (!c.isDigit()) {
                validPin = false;
                break;
            }
        }

        if (validPin) break;

        QMessageBox::warning(this, "Invalid PIN",
            "PIN must be exactly 6 digits");
    }

    if (Account::createAccount(username.toStdString(), pin.toStdString())) {
        currentUsername = username;
        currentPin = pin;
        QMessageBox::information(this, "Success",
            "Account created successfully!");
        return true;
    }

    QMessageBox::critical(this, "Error",
        "Failed to create account");
    return false;
}

void MainWindow::regenerateKey() {
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Warning",
        "You will lose access to your previously encrypted passwords because the key will change.\n"
        "Do you want to continue?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bool ok;
        QString pin = QInputDialog::getText(this, "Verify",
            "Enter your PIN to confirm:", QLineEdit::Password, "", &ok);

        if (ok && pin == currentPin) {
            charMapping = KeyManager::generateKey();
            KeyManager::saveKeyToFile(charMapping, keyFile, keyPassword);
            QMessageBox::information(this, "Success", "Key mapping has been regenerated");
        }
        else {
            QMessageBox::critical(this, "Error", "Invalid PIN");
        }
    }
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Input area
    QLabel* inputLabel = new QLabel("Input Text:", this);
    inputText = new QTextEdit(this);
    mainLayout->addWidget(inputLabel);
    mainLayout->addWidget(inputText);

    // Output area
    QLabel* outputLabel = new QLabel("Output Text:", this);
    outputText = new QTextEdit(this);
    outputText->setReadOnly(true);
    mainLayout->addWidget(outputLabel);
    mainLayout->addWidget(outputText);

    // Password length input
    QHBoxLayout* passwordLayout = new QHBoxLayout();
    QLabel* lengthLabel = new QLabel("Password Length:", this);
    passwordLength = new QSpinBox(this);
    passwordLength->setRange(8, 32);
    passwordLength->setValue(12);
    passwordLayout->addWidget(lengthLabel);
    passwordLayout->addWidget(passwordLength);
    mainLayout->addLayout(passwordLayout);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    encryptButton = new QPushButton("Encrypt", this);
    decryptButton = new QPushButton("Decrypt", this);
    regenerateKeyButton = new QPushButton("Regenerate Key", this);
    generatePasswordButton = new QPushButton("Generate Password", this);

    buttonLayout->addWidget(encryptButton);
    buttonLayout->addWidget(decryptButton);
    buttonLayout->addWidget(regenerateKeyButton);
    buttonLayout->addWidget(generatePasswordButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(encryptButton, &QPushButton::clicked, this, &MainWindow::encryptText);
    connect(decryptButton, &QPushButton::clicked, this, &MainWindow::decryptText);
    connect(regenerateKeyButton, &QPushButton::clicked, this, &MainWindow::regenerateKey);
    connect(generatePasswordButton, &QPushButton::clicked, this, &MainWindow::generatePassword);

    setWindowTitle("Encoder/Decoder");
    resize(600, 400);
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

void MainWindow::encryptText() {
    std::string input = inputText->toPlainText().toStdString();
    if (input.empty()) {
        QMessageBox::warning(this, "Warning", "Please enter text to encrypt");
        return;
    }

    std::string encrypted = Encrypt::encryptLayered(charMapping, input, aesKey, iv);
    outputText->setText(QString::fromStdString(encrypted));
}

void MainWindow::decryptText() {
    std::string input = inputText->toPlainText().toStdString();
    if (input.empty()) {
        QMessageBox::warning(this, "Warning", "Please enter text to decrypt");
        return;
    }

    try {
        std::string decrypted = Decrypt::decryptLayered(charMapping, input, aesKey, iv);
        outputText->setText(QString::fromStdString(decrypted));
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", "Decryption failed. Invalid input.");
    }
}

void MainWindow::generatePassword() {
    int length = passwordLength->value();
    std::string password = Utils::generateRandomString(length);

    std::string output = "Generated Password: " + password + "\n\n";

    std::string substitutionEncrypted = Encrypt::encryptString(charMapping, password);
    output += "Substitution Encrypted: " + substitutionEncrypted + "\n\n";

    std::string aesEncrypted = Encrypt::encryptAES(password, aesKey, iv);
    aesEncrypted = Encrypt::base64Encode(aesEncrypted);
    output += "AES Encrypted: " + aesEncrypted;

    outputText->setText(QString::fromStdString(output));
}