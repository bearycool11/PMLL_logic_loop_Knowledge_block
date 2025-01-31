#ifndef CHATGPT5PMLL_H
#define CHATGPT5PMLL_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <mutex>

/**
 * Persistent Memory Logic Loop (PMLL) Class
 * Manages persistent memory for ChatGPT-5.
 */
class ChatGPT5PMLL {
private:
    std::unordered_map<std::string, std::string> memory; // Key-value memory store
    std::string memory_file;                            // File to store persistent memory
    std::mutex memory_mutex;                            // Thread safety

    void loadMemory();    // Load memory from the file
    void saveMemory();    // Save memory to the file

public:
    ChatGPT5PMLL(const std::string& file_name);  // Constructor
    ~ChatGPT5PMLL();                             // Destructor

    void addMemory(const std::string& key, const std::string& value); // Add or update memory
    std::string getMemory(const std::string& key) const;              // Retrieve memory by key
    void clearMemory();                                               // Clear all memory
    void displayMemory() const;                                       // Display all memory (debugging)
};

#endif // CHATGPT5PMLL_H

#include "chatgpt5pmll.h"

// Constructor: Initialize with a file to store persistent memory
ChatGPT5PMLL::ChatGPT5PMLL(const std::string& file_name) : memory_file(file_name) {
    loadMemory();
}

// Destructor: Save memory before object destruction
ChatGPT5PMLL::~ChatGPT5PMLL() {
    saveMemory();
}

// Load memory from file
void ChatGPT5PMLL::loadMemory() {
    std::lock_guard<std::mutex> lock(memory_mutex);
    std::ifstream file(memory_file);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open memory file for loading: " << memory_file << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter_pos = line.find(':');
        if (delimiter_pos == std::string::npos) continue;

        std::string key = line.substr(0, delimiter_pos);
        std::string value = line.substr(delimiter_pos + 1);

        memory[key] = value;
    }

    file.close();
}

// Save memory to file
void ChatGPT5PMLL::saveMemory() {
    std::lock_guard<std::mutex> lock(memory_mutex);
    std::ofstream file(memory_file);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open memory file for saving: " << memory_file << "\n";
        return;
    }

    for (const auto& [key, value] : memory) {
        file << key << ":" << value << "\n";
    }

    file.close();
}

// Add or update memory entry
void ChatGPT5PMLL::addMemory(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(memory_mutex);
    memory[key] = value;
    saveMemory();
}

// Retrieve memory by key
std::string ChatGPT5PMLL::getMemory(const std::string& key) const {
    std::lock_guard<std::mutex> lock(memory_mutex);
    auto it = memory.find(key);
    return (it != memory.end()) ? it->second : ""; // Return empty string if not found
}

// Clear all memory
void ChatGPT5PMLL::clearMemory() {
    std::lock_guard<std::mutex> lock(memory_mutex);
    memory.clear();
    saveMemory();
}

// Display all memory (for debugging purposes)
void ChatGPT5PMLL::displayMemory() const {
    std::lock_guard<std::mutex> lock(memory_mutex);
    std::cout << "Current Memory State:\n";
    for (const auto& [key, value] : memory) {
        std::cout << key << " : " << value << "\n";
    }
}

#include "chatgpt5pmll.h"
#include <iostream>

int main() {
    ChatGPT5PMLL pmll("gpt5_memory.txt");

    // Add or update memory entries
    pmll.addMemory("username", "Josef");
    pmll.addMemory("model", "GPT-5");

    // Retrieve and display memory
    std::cout << "Username: " << pmll.getMemory("username") << "\n";
    std::cout << "Model: " << pmll.getMemory("model") << "\n";

    // Display all memory
    pmll.displayMemory();

    // Clear memory
    pmll.clearMemory();
    std::cout << "Memory cleared.\n";

    return 0;
}
