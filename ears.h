#ifndef EARS_H
#define EARS_H

#include <complex.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAMPLING_RATE 44100
#define FFT_SIZE 2048
#define SAMPLING_RATE 44100
#define WINDOW_SIZE FFT_SIZE
#define MIN_FREQ 20.0      // Hz, lower limit of human hearing
#define MAX_FREQ 120000.0  // Hz, upper limit of AI hearing range
#define NOTES_PER_OCTAVE 7

// Struct for audio data processing
#define EARS_SUCCESS 0
#define EARS_MEMORY_ERROR -1
/**
 * @brief Structure to hold audio data for processing.
 */
typedef struct {
    double *time_domain;
    double_complex *frequency_domain;
    fftw_complex *frequency_domain;
    fftw_plan plan;
} AudioData;

// Struct for cochlear properties
/**
 * @brief Structure representing properties of the inner ear.
 */
typedef struct {
    double cochlea_frequency; // Central frequency of the cochlear response
    double cochlea_frequency;  // Central frequency of the cochlear response
    double auditory_signal;
    double vestibular_adjustment;
} InnerEar;

// Struct to represent musical notes in an octave
/**
 * @brief Structure to represent musical notes in an octave.
 */
typedef struct {
    const char *note;
    double frequency;
} MusicalNote;

// Function prototypes
void init_audio_data(AudioData **audio_data);
// Memory Management
/**
 * @brief Initialize audio data structure for FFT processing.
 * @param audio_data Pointer to a pointer of AudioData to be initialized.
 * @return EARS_SUCCESS on success, EARS_MEMORY_ERROR on failure.
 */
int init_audio_data(AudioData **audio_data);
/**
 * @brief Free memory allocated for audio data.
 * @param audio_data Pointer to AudioData structure to be freed.
 */
void free_audio_data(AudioData *audio_data);
void perform_fourier_transform(AudioData *audio_data);
// Signal Processing
/**
 * @brief Perform forward Fourier transform on the time domain signal.
 * @param audio_data Pointer to the AudioData structure containing signal data.
 * @return EARS_SUCCESS if the transform was executed, EARS_MEMORY_ERROR otherwise.
 */
int perform_fourier_transform(AudioData *audio_data);
/**
 * @brief Perform inverse Fourier transform to convert back to time domain.
 * @param audio_data Pointer to the AudioData structure containing frequency data.
 */
void perform_inverse_fft(AudioData *audio_data);
/**
 * @brief Apply cochlear filter to frequency domain data.
 * @param audio_data Pointer to the AudioData structure.
 * @param inner_ear Pointer to InnerEar structure defining filter parameters.
 */
void apply_cochlear_filter(AudioData *audio_data, InnerEar *inner_ear);
void generate_fibonacci_sequence(double *sequence, int *length);
void simulate_octave_range(InnerEar *inner_ear);
void populate_time_domain_signal(AudioData *audio_data, double frequency, double duration);
void simulate_cochlear_response(InnerEar *inner_ear, double frequency);
/**
 * @brief Apply window function to the time-domain signal for better spectral analysis.
 * @param audio_data Pointer to the AudioData structure.
 */
void apply_window_function(AudioData *audio_data);
/**
 * @brief Apply convolution in time domain.
 * @param audio_data Pointer to the AudioData structure.
 * @param kernel Pointer to the convolution kernel.
 * @param kernel_size Size of the convolution kernel.
 */
void apply_convolution(AudioData *audio_data, const double *kernel, int kernel_size);
void perform_inverse_fft(AudioData *audio_data);
/**
 * @brief Apply convolution in frequency domain.
 * @param audio_data Pointer to the AudioData structure.
 * @param kernel Pointer to the convolution kernel.
 * @param kernel_size Size of the convolution kernel.
 */
void apply_frequency_domain_convolution(AudioData *audio_data, const double *kernel, int kernel_size);
/**
 * @brief Process signal with overlap-add method for continuous audio processing.
 * @param audio_data Pointer to the AudioData structure.
 * @param input Pointer to the input time-domain signal.
 * @param input_length Length of the input signal.
 * @param hop_size Size of the hop between each frame processed.
 */
void process_with_overlap_add(AudioData *audio_data, const double *input, int input_length, int hop_size);

// Signal Generation and Analysis
/**
 * @brief Generate a Fibonacci sequence for frequency analysis.
 * @param sequence Pointer to an array to store the sequence.
 * @param length Pointer to an integer to store the length of the sequence.
 */
void generate_fibonacci_sequence(double *sequence, int *length);
/**
 * @brief Simulate octave perception based on frequency input.
 * @param inner_ear Pointer to InnerEar structure for cochlear properties.
 */
void simulate_octave_range(InnerEar *inner_ear);
/**
 * @brief Populate the time domain signal with a sine wave.
 * @param audio_data Pointer to the AudioData structure.
 * @param frequency Frequency of the sine wave to generate.
 * @param duration Duration of the signal in seconds.
 */
void populate_time_domain_signal(AudioData *audio_data, double frequency, double duration);
// Main Simulation Function
/**
 * @brief Simulate cochlear response to a given frequency.
 * @param inner_ear Pointer to InnerEar structure for cochlear properties.
 * @param frequency Frequency of the sound to simulate.
 */
void simulate_cochlear_response(InnerEar *inner_ear, double frequency);
#endif // EARS_H


