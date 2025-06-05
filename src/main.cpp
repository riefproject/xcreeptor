#include "mainWindow.hpp"
#include <iostream>
#include <filesystem>

int main() {
    try {
        // Check if assets directory exists
        if (!std::filesystem::exists("assets")) {
            std::filesystem::create_directories("assets");
        }

        std::cout << "Starting Xcreeptor..." << std::endl;

        MainWindow app;
        app.run();

        std::cout << "Application finished normally." << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 1;
    }
}