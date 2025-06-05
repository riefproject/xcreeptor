# Xcreeptor

Xcreeptor is a Raylib-based encryption and decryption application designed to provide a secure and user-friendly interface for managing sensitive data. It combines multiple encryption layers, including substitution ciphers and AES encryption, to ensure robust data protection. The application also includes features for account management, key regeneration, and password generation.

## Table of Contents

1. [Features](#features)
    - [Encryption and Decryption](#1-encryption-and-decryption)
    - [Account Management](#2-account-management)
    - [Key Management](#3-key-management)
    - [Password Generation](#4-password-generation)
    - [Cross-Platform GUI](#5-cross-platform-gui)
2. [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Steps](#steps)
3. [Usage](#usage)
    - [Running the Application](#running-the-application)
    - [Key Features](#key-features)
4. [Project Structure](#project-structure)
5. [Development](#development)
    - [Build Instructions](#build-instructions)
    - [Debugging](#debugging)
6. [Contributing](#contributing)
7. [License](#license)
8. [Acknowledgments](#acknowledgments)
9. [Contact](#contact)

## Features

### 1. **Encryption and Decryption**

-   **Substitution Cipher**: Encrypts text using a character mapping.
-   **AES Encryption**: Adds an additional layer of security using AES-256-CBC encryption.
-   **Layered Encryption**: Combines substitution cipher and AES encryption for enhanced security.
-   **Base64 Encoding**: Encodes the final encrypted data for safe storage or transmission.

### 2. **Account Management**

-   **Account Creation**: Users can create accounts with a username and a 6-digit PIN.
-   **Authentication**: Users must log in with their PIN to access the application.
-   **Secure PIN Storage**: PINs are hashed using SHA-256 and stored securely.

### 3. **Key Management**

-   **Key Generation**: Automatically generates a random substitution cipher key.
-   **Key Regeneration**: Allows users to regenerate the key, invalidating previously encrypted data.
-   **Key Storage**: Keys are encrypted and stored securely using a password.

### 4. **Password Generation**

-   **Random Passwords**: Generates secure random passwords of customizable lengths.
-   **Encryption Options**: Displays the generated password in plain text, substitution-encrypted, and AES-encrypted formats.

### 5. **Cross-Platform GUI**

-   Built using **Raylib** with **Raygui**, providing a lightweight and responsive user interface.
-   Includes input/output text areas, buttons for encryption/decryption, and password generation tools.
-   Features a modern sidebar navigation with home, encrypt, decrypt, and password generation pages.

## Installation

### Prerequisites

1. **Raylib**: Ensure you have Raylib installed on your system.
2. **OpenSSL**: Required for AES encryption and decryption.
3. **C++ Compiler**: A C++17-compatible compiler (e.g., GCC, MSVC).
4. **Make**: For building the project.

### Steps

1. Clone the repository:

    ```bash
    git clone https://github.com/riefproject/xcreeptor.git
    cd xcreeptor
    ```

2. Install dependencies:

    - Install Raylib development libraries.
    - Install OpenSSL development libraries.

3. Build the project:

    ```bash
    make
    ```

4. Run the application:
    ```bash
    ./bin/xreeptor.exe
    ```

## Usage

### Running the Application

1. Launch the application:
    ```bash
    ./bin/xreeptor.exe
    ```
2. Create an account if this is your first time, or log in with your existing PIN.
3. Use the sidebar navigation to access different features:
    - **Home**: Overview of available features
    - **Encrypt**: Encrypt text using the layered encryption system
    - **Decrypt**: Decrypt previously encrypted text
    - **Generate Password**: Create secure random passwords
    - **Regenerate Key**: Create a new encryption key (warning: invalidates old data)

### Key Features

-   **Encrypt Text**: Enter text in the input area and click "Encrypt" to secure it.
-   **Decrypt Text**: Paste encrypted text in the input area and click "Decrypt" to retrieve the original text.
-   **Generate Password**: Specify the desired password length and click "Generate Password."
-   **Regenerate Key**: Click "Regenerate Key" to create a new substitution cipher key with confirmation modal.
-   **Copy to Clipboard**: Easily copy encrypted/decrypted results or generated passwords.

## Project Structure

```
xcreeptor/
├── assets/                # Stores account and key files
│   ├── account.dat        # User account data
│   ├── key.dat           # Encryption key storage
│   └── resource.rc       # Windows resource file
├── bin/                   # Output directory for the compiled binary
│   └── xreeptor.exe  # Main executable
├── build/                 # Build directory for object files
├── include/               # Header files
│   ├── envmgr.hpp        # Environment manager
│   ├── core/crypto/      # Encryption and decryption logic
│   ├── core/utils/       # Utility functions
│   └── ui/               # UI-related headers
│       └── mainWindow.hpp # Main window class definition
├── lib/                   # External libraries
│   └── raylib/           # Raylib library files
├── src/                   # Source files
│   ├── envmgr.cpp        # Environment manager implementation
│   ├── main.cpp          # Application entry point
│   ├── core/crypto/      # Encryption and decryption implementations
│   ├── core/utils/       # Utility implementations
│   └── ui/               # UI logic
│       └── mainWindow.cpp # Main window implementation
├── tests/                 # Test files
├── .vscode/              # VSCode configuration files
├── .github/              # GitHub-specific files
├── build.sh              # Build script
├── LICENSE               # License file
├── Makefile              # Build instructions
└── README.md             # Project documentation
```

## Development

### Build Instructions

1. Modify the `Makefile` if necessary to match your system's configuration.
2. Run `make` to build the project.
3. Use `make clean` to remove build artifacts.
4. Use the provided `build.sh` script for alternative building.

### Debugging

-   Enable debug logs by reviewing the debug statements in the source files.
-   Use a debugger like `gdb` or Visual Studio Code's built-in debugger for step-by-step execution.
-   The application runs at 60 FPS and includes window management through Raylib.

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch:
    ```bash
    git checkout -b feature-name
    ```
3. Commit your changes:
    ```bash
    git commit -m "Add detailed description of your changes"
    ```
4. Push to your branch:
    ```bash
    git push origin feature-name
    ```
5. Open a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

-   **Raylib**: For providing a simple and efficient game development framework.
-   **Raygui**: For the immediate mode GUI system.
-   **OpenSSL**: For cryptographic functions.
-   **C++ Community**: For libraries and tools that made this project possible.

## Contact

For questions or support, please contact [Arief F-sa Wijaya](https://github.com/riefproject) via GitHub.
