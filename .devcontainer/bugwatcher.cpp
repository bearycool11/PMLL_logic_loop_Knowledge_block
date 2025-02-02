#include "bugwatcher.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

// Global whitelist and blacklist
char *whitelist[] = {"62.0.0.1", "62.0.0.2"};
int whitelist_count = 2;
char *blacklist[MAX_BLACKLIST];
int blacklist_count = 0;

// Initialization function
void watcher_init() {
    std::cout << "[Watcher Init] Initializing BugWatcher...\n";
    memset(blacklist, 0, sizeof(blacklist));
    blacklist_count = 0;
}

// Function to monitor logs dynamically
void monitor_logs_dynamic() {
    std::cout << "[Log Monitor] Monitoring logs dynamically...\n";
    // Simulated log entry
    std::string log_entry = "bot detected on 192.168.0.10";

    // Handle dynamic responses based on the log entry
    handle_dynamic_response("192.168.0.10", log_entry.c_str());
}

// Handle failed attempts
void handle_failed_attempt(const char *ip) {
    std::cout << "[BugWatcher] Handling failed attempt from IP: " << ip << std::endl;

    if (should_blacklist(ip)) {
        add_to_blacklist(ip);
        log_activity("Security Alert", "Blacklisted IP detected.");
    }
}

// Add IP to blacklist
void add_to_blacklist(const char *ip) {
    if (blacklist_count < MAX_BLACKLIST) {
        blacklist[blacklist_count++] = strdup(ip);
        std::cout << "[Blacklist] Added IP: " << ip << std::endl;
    }
}

// Validate if an IP is whitelisted
int is_whitelisted(const char *ip) {
    for (int i = 0; i < whitelist_count; i++) {
        if (strcmp(whitelist[i], ip) == 0) {
            return 1; // True
        }
    }
    return 0; // False
}

// Check if an IP should be blacklisted
int should_blacklist(const char *ip) {
    return !is_whitelisted(ip) && recent_failed_attempts(ip) > 5;
}

// Count recent failed attempts
int recent_failed_attempts(const char *ip) {
    // Simulated count of failed attempts
    return 6;
}

// Handle dynamic responses
void handle_dynamic_response(const char *ip, const char *log_entry) {
    if (is_whitelisted(ip)) return;

    if (strstr(log_entry, "bot")) {
        manage_botswarm(ip, log_entry);
    } else {
        handle_failed_attempt(ip);
    }
}

// Deobfuscation functions
bool bugwatcher_check_line(const std::string &line) {
    return line.find("suspicious") == std::string::npos;
}

std::string bugwatcher_deobfuscate(const std::string &line) {
    std::string deobfuscated_line = line;
    std::replace(deobfuscated_line.begin(), deobfuscated_line.end(), '*', ' ');
    return deobfuscated_line;
}
