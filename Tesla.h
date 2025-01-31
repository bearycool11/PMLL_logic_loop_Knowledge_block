#ifndef TESLA_H
#define TESLA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>  
#include <pthread.h>  
#include <cuda_runtime.h>  // Dojo AI Acceleration

#define STATE_FILE "tesla_state.dat"  // Persistent Neural Cache
#define LOG_FILE   "tesla_log.txt"    // AI Event Logging

// 🚗 Tesla’s Persistent Neural Memory for FSD AI
typedef struct {
    int iteration_count;             // Total learning iterations
    double last_steering_correction; // Adjustments to steering
    double last_brake_intensity;     // Adjustments to braking
    char last_scene_data[512];       // Stores last driving scenario
} TeslaMemoryState;

// 🚦 Tesla Sensor Data Structure
typedef struct {
    double steering_angle;   // Current steering angle
    double brake_force;      // Current brake force
    char scene_description[512];  // Processed scene data (AI output)
} TeslaSensorData;

// 🚀 AI Engine Function Prototypes
void initialize_tesla_memory(TeslaMemoryState *state);
void save_state(const TeslaMemoryState *state);
bool load_state(TeslaMemoryState *state);
void log_event(const char *message);

// 📡 Sensor Fusion & AI Processing
TeslaSensorData get_real_time_sensor_data();
void* sensor_fusion_thread(void* arg);

// 🧠 Adaptive AI Reinforcement Learning
bool NovelTopic(const char *input, TeslaMemoryState *state);
void NovelInput(const char *input, TeslaMemoryState *state);

// 🔄 Tesla AI Autonomy Loop
void tesla_logic_loop(TeslaMemoryState *state);

#endif // TESLA_H
