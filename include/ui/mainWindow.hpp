#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <map>
#include <string>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void encryptText();
    void decryptText();
    void regenerateKey();
    void generatePassword();

private:
    void setupUI();
    void initializeKey();

    QTextEdit* inputText;
    QTextEdit* outputText;
    QSpinBox* passwordLength;
    QPushButton* encryptButton;
    QPushButton* decryptButton;
    QPushButton* regenerateKeyButton;
    QPushButton* generatePasswordButton;

    std::map<char, char> charMapping;
    std::string keyFile;
    std::string keyPassword;
    std::string aesKey;
    std::string iv;

    bool verifyPin(const QString& message);
    QString currentUsername;
    QString currentPin;

    bool handleAuthentication();
    bool showLoginDialog();
    bool showCreateAccountDialog();
};

#endif