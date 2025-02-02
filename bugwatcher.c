#include "BugWatcher.h"
#include <curl/curl.h>

// Global whitelist and blacklist
char *whitelist[] = {"62.0.0.1", "62.0.0.2"};
int whitelist_count = 2;
char *blacklist[MAX_BLACKLIST];
int blacklist_count = 0;

// ... (all previous functions from the watcher system)

// Bot Swarm Management functions

void init_botswarm(Botswarm *swarm) {
    swarm->count = 0;
    swarm->neutralized = 0;
    memset(swarm->bot_types_count, 0, sizeof(swarm->bot_types_count));
    memset(swarm->platforms_count, 0, sizeof(swarm->platforms_count));
    memset(swarm->bots, 0, sizeof(swarm->bots));
}

void add_bot(Botswarm *swarm, int id, const char *bot_type, const char *platform) {
    if (swarm->count < MAX_BOTS) {
        Bot *bot = &swarm->bots[swarm->count++];
        bot->id = id;
        strncpy(bot->bot_type, bot_type, sizeof(bot->bot_type) - 1);
        strncpy(bot->platform, platform, sizeof(bot->platform) - 1);
        bot->active = true;
        
        // Update bot type and platform counts
        for (int i = 0; i < MAX_BOT_TYPES; i++) {
            if (strcmp(swarm->bots[i].bot_type, bot_type) == 0 || swarm->bots[i].bot_type[0] == '\0') {
                swarm->bot_types_count[i]++;
                break;
            }
        }
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (strcmp(swarm->bots[i].platform, platform) == 0 || swarm->bots[i].platform[0] == '\0') {
                swarm->platforms_count[i]++;
                break;
            }
        }
    }
}

void neutralize_bot(Botswarm *swarm, int id) {
    for (int i = 0; i < swarm->count; i++) {
        if (swarm->bots[i].id == id && swarm->bots[i].active) {
            swarm->bots[i].active = false;
            swarm->neutralized++;
            update_stats(swarm);
            return;
        }
    }
}

void print_botswarm_status(const Botswarm *swarm) {
    printf("Botswarm Status:\n");
    printf("Total Bots: %d\n", swarm->count);
    printf("Active Bots: %d\n", count_active_bots(swarm));
    printf("Neutralized Bots: %d\n", swarm->neutralized);

    printf("Bot Types:\n");
    for (int i = 0; i < MAX_BOT_TYPES && swarm->bots[i].bot_type[0] != '\0'; i++) {
        printf("%s: %d\n", swarm->bots[i].bot_type, swarm->bot_types_count[i]);
    }

    printf("Platforms:\n");
    for (int i = 0; i < MAX_PLATFORMS && swarm->bots[i].platform[0] != '\0'; i++) {
        printf("%s: %d\n", swarm->bots[i].platform, swarm->platforms_count[i]);
    }
}

int count_active_bots(const Botswarm *swarm) {
    int active = 0;
    for (int i = 0; i < swarm->count; i++) {
        if (swarm->bots[i].active) {
            active++;
        }
    }
    return active;
}

void update_stats(Botswarm *swarm) {
    memset(swarm->bot_types_count, 0, sizeof(swarm->bot_types_count));
    memset(swarm->platforms_count, 0, sizeof(swarm->platforms_count));

    for (int i = 0; i < swarm->count; i++) {
        if (swarm->bots[i].active) {
            for (int j = 0; j < MAX_BOT_TYPES; j++) {
                if (strcmp(swarm->bots[i].bot_type, swarm->bots[j].bot_type) == 0 || swarm->bots[j].bot_type[0] == '\0') {
                    swarm->bot_types_count[j]++;
                    break;
                }
            }
            for (int j = 0; j < MAX_PLATFORMS; j++) {
                if (strcmp(swarm->bots[i].platform, swarm->bots[j].platform) == 0 || swarm->bots[j].platform[0] == '\0') {
                    swarm->platforms_count[j]++;
                    break;
                }
            }
        }
    }
}

void manage_botswarm(const char *ip, const char *log_entry) {
    static Botswarm swarm; // Static to maintain state across calls
    static bool initialized = false;

    if (!initialized) {
        init_botswarm(&swarm);
        initialized = true;
    }

    // Example action based on log entry
    if (strstr(log_entry, "bot detected")) {
        add_bot(&swarm, swarm.count + 1, "Unknown", ip);
    } else if (strstr(log_entry, "bot neutralized")) {
        neutralize_bot(&swarm, swarm.count);
    }

    print_botswarm_status(&swarm);
}

void handle_dynamic_response(const char *ip, const char *log_entry) {
    if (is_whitelisted(ip)) return;

    // ... (existing code for handling threats)

    // Check if this could involve bot activity
    if (strstr(log_entry, "bot") || strstr(log_entry, "swarm")) {
        manage_botswarm(ip, log_entry);  // Manage bot swarm based on log entry
    }
}

int main() {
    watcher_init();
    monitor_logs_dynamic(); 
    // manage_botswarm() could be called here if needed outside of dynamic response

    return 0;
}
