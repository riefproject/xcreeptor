# Xcreeptor

Xcreeptor is a Qt-based encryption and decryption application designed to provide a secure and user-friendly interface for managing sensitive data. It combines multiple encryption layers, including substitution ciphers and AES encryption, to ensure robust data protection. The application also includes features for account management, key regeneration, and password generation.

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

-   Built using **Qt Framework**, providing a modern and responsive user interface.
-   Includes input/output text areas, buttons for encryption/decryption, and password generation tools.

## Installation

### Prerequisites

1. **Qt Framework**: Ensure you have Qt5 installed on your system.
2. **OpenSSL**: Required for AES encryption and decryption.
3. **C++ Compiler**: A C++17-compatible compiler (e.g., GCC, MSVC).
4. **CMake or Make**: For building the project.

### Steps

1. Clone the repository:

    ```bash
    git clone https://github.com/your/riedproject/xcreeptor.git
    cd xcreeptor
    ```

2. Install dependencies:

    - Ensure `pkg-config` is installed for Qt5 detection.
    - Install OpenSSL development libraries.

3. Build the project:

    ```bash
    make
    ```

4. Run the application:
    ```bash
    ./bin/decoder
    ```

## Usage

### Running the Application

1. Launch the application:
    ```bash
    ./bin/decoder
    ```
2. Log in or create an account.
3. Use the interface to encrypt/decrypt text, generate passwords, or manage keys.

### Key Features

-   **Encrypt Text**: Enter text in the input area and click "Encrypt" to secure it.
-   **Decrypt Text**: Paste encrypted text in the input area and click "Decrypt" to retrieve the original text.
-   **Generate Password**: Specify the desired password length and click "Generate Password."
-   **Regenerate Key**: Click "Regenerate Key" to create a new substitution cipher key.

## Project Structure

```
xcreeptor/
├── assets/                # Stores account and key files
├── bin/                   # Output directory for the compiled binary
├── build/                 # Build directory for object files
├── include/               # Header files
│   ├── core/crypto/       # Encryption and decryption logic
│   ├── core/utils/        # Utility functions
│   └── ui/                # UI-related headers
├── src/                   # Source files
│   ├── core/crypto/       # Encryption and decryption implementations
│   ├── core/utils/        # Utility implementations
│   └── ui/                # UI logic
├── .vscode/               # VSCode configuration files
├── .github/               # GitHub-specific files
├── LICENSE                # License file
├── Makefile               # Build instructions
└── README.md              # Project documentation
```

## Development

### Build Instructions

1. Modify the `Makefile` if necessary to match your system's configuration.
2. Run `make` to build the project.
3. Use `make clean` to remove build artifacts.

### Debugging

-   Enable debug logs by reviewing the `std::cout` statements in the source files.
-   Use a debugger like `gdb` or Qt Creator's built-in debugger for step-by-step execution.

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

-   **Qt Framework**: For providing a robust GUI framework.
-   **OpenSSL**: For cryptographic functions.
-   **C++ Community**: For libraries and tools that made this project possible.

## Contact

For questions or support, please contact [Arief F-sa Wijaya](https://github.com/riefproject) via GitHub.
