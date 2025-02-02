#ifndef WATCHER_H
#define WATCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Constants
#define MAX_BLACKLIST 1024

// Function declarations
void watcher_init();
void monitor_logs();
void handle_failed_attempt(const char *ip);
void snoop_data_streams(const char *ip);
void record_user_input(const char *ip);
void add_to_blacklist(const char *ip);
int is_whitelisted(const char *ip);

#endif // WATCHER_H
