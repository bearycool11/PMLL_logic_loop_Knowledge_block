#ifndef PMLL_H
#define PMLL_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <mutex>

class PMLL {
private:
    std::unordered_map<std::string, std::string> memory; // Key-value memory store
    std::string memory_file;                            // File to store persistent memory
    std::mutex memory_mutex;                            // Thread safety

    void loadMemory();    // Load memory from the file
    void saveMemory();    // Save memory to the file

public:
    PMLL(const std::string& file_name);  // Constructor
    ~PMLL();                             // Destructor

    void addMemory(const std::string& key, const std::string& value); // Add or update memory
    std::string getMemory(const std::string& key) const;              // Retrieve memory by key
    void clearMemory();                                               // Clear all memory
    void displayMemory() const;                                       // Display all memory (debugging)
};

#endif // PMLL_H
