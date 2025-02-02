#ifndef BUGWATCHER_H
#define BUGWATCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>
#include <stdbool.h>

// Constants
#define MAX_BLACKLIST 1024
#define MAX_BOTS 1000  // Maximum number of bots to track
#define MAX_BOT_TYPES 10  // Maximum number of bot types
#define MAX_PLATFORMS 5  // Maximum number of platforms

// Structures
typedef struct {
    int id;
    char bot_type[50];
    char platform[50];
    bool active;
} Bot;

typedef struct {
    Bot bots[MAX_BOTS];
    int count;
    int neutralized;
    int bot_types_count[MAX_BOT_TYPES];
    int platforms_count[MAX_PLATFORMS];
} Botswarm;

// Global whitelist and blacklist
extern char *whitelist[];
extern int whitelist_count;
extern char *blacklist[];
extern int blacklist_count;

// Function declarations

// Initialization
void watcher_init();

// Log Monitoring
void monitor_logs();
void monitor_logs_dynamic();

// Dynamic Response Handling
void handle_failed_attempt(const char *ip);
void handle_dynamic_response(const char *ip, const char *log_entry);

// API Communication
void send_api_request(const char *url, const char *api_token, const char *payload);

// Data Snooping and Input Recording
void snoop_data_streams(const char *ip);
void record_user_input(const char *ip);

// Logging
void log_activity(const char *type, const char *message);

// Whitelist and Blacklist Management
int is_whitelisted(const char *ip);
int should_blacklist(const char *ip);
int recent_failed_attempts(const char *ip);
void add_to_blacklist(const char *ip);
void save_blacklist();

// Ethical Review
void ethical_review(const char *ip);

// Bot Swarm Management
void init_botswarm(Botswarm *swarm);
void add_bot(Botswarm *swarm, int id, const char *bot_type, const char *platform);
void neutralize_bot(Botswarm *swarm, int id);
void print_botswarm_status(const Botswarm *swarm);
int count_active_bots(const Botswarm *swarm);
void update_stats(Botswarm *swarm);
void manage_botswarm(const char *ip, const char *log_entry);  // Modified to include parameters

#endif // BUGWATCHER_H
