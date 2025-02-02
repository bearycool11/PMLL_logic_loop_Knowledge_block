#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <nlohmann/json.hpp>

// JSON data parser
using json = nlohmann::json;

// Function prototypes
void loadEnvironment();
void buildEngine();
void runTests();

int main() {
    std::cout << "Initializing YMLJSONImage Engine..." << std::endl;

    // Load and setup environment
    loadEnvironment();

    // Build the engine
    buildEngine();

    // Run tests
    runTests();

    std::cout << "Engine workflow completed successfully!" << std::endl;
    return 0;
}

void loadEnvironment() {
    std::cout << "[Setup] Loading environment configuration..." << std::endl;

    std::ifstream configFile("engine.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open engine.json!" << std::endl;
        exit(EXIT_FAILURE);
    }

    json config;
    configFile >> config;

    for (const auto &dependency : config["engine"]["environment"]["dependencies"]) {
        std::cout << "[Dependency] Installing: " << dependency["name"] 
                  << " version: " << dependency["version"] << std::endl;
        std::string command = "sudo apt-get install -y " + std::string(dependency["name"]);
        system(command.c_str());
    }
}

void buildEngine() {
    std::cout << "[Build] Compiling the engine..." << std::endl;

    std::ifstream configFile("engine.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open engine.json!" << std::endl;
        exit(EXIT_FAILURE);
    }

    json config;
    configFile >> config;

    std::string buildCommand = config["engine"]["workflow"]["build"]["commands"][0];
    int status = system(buildCommand.c_str());
    if (status != 0) {
        std::cerr << "[Build] Compilation failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "[Build] Engine compiled successfully!" << std::endl;
}

void runTests() {
    std::cout << "[Test] Running tests..." << std::endl;

    std::ifstream configFile("engine.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open engine.json!" << std::endl;
        exit(EXIT_FAILURE);
    }

    json config;
    configFile >> config;

    for (const auto &testCommand : config["engine"]["workflow"]["test"]["commands"]) {
        int status = system(testCommand.get<std::string>().c_str());
        if (status != 0) {
            std::cerr << "[Test] Test failed!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "[Test] All tests passed successfully!" << std::endl;
}
