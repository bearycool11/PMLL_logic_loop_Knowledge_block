#include "ears.h"
#include <string.h>
#include <float.h> // For DBL_MAX

// Memory Management
int init_audio_data(AudioData **audio_data) {
    if (!*audio_data) {
        *audio_data = (AudioData*)malloc(sizeof(AudioData));
        if (!*audio_data) {
            fprintf(stderr, "Failed to allocate memory for audio data structure\n");
            return EARS_MEMORY_ERROR;
        }

        (*audio_data)->time_domain = (double*)calloc(FFT_SIZE, sizeof(double));
        (*audio_data)->frequency_domain = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (FFT_SIZE/2 + 1)); // Correct size for complex output
        if (!(*audio_data)->time_domain || !(*audio_data)->frequency_domain) {
            fprintf(stderr, "Failed to allocate memory for audio data arrays\n");
            free_audio_data(*audio_data);
            return EARS_MEMORY_ERROR;
        }

        (*audio_data)->plan = fftw_plan_dft_r2c_1d(FFT_SIZE, (*audio_data)->time_domain, 
                                                    (*audio_data)->frequency_domain, FFTW_ESTIMATE);
    }
    return EARS_SUCCESS;
}

void free_audio_data(AudioData *audio_data) {
    if (audio_data) {
        if (audio_data->time_domain) free(audio_data->time_domain);
        if (audio_data->frequency_domain) fftw_free(audio_data->frequency_domain);
        if (audio_data->plan) fftw_destroy_plan(audio_data->plan);
        free(audio_data);
    }
}

// Signal Processing
int perform_fourier_transform(AudioData *audio_data) {
    if (!audio_data || !audio_data->plan) {
        return EARS_MEMORY_ERROR;
    }
    fftw_execute(audio_data->plan);
    return EARS_SUCCESS;
}

void perform_inverse_fft(AudioData *audio_data) {
    if (audio_data && audio_data->plan) {
        fftw_plan inverse_plan = fftw_plan_dft_c2r_1d(FFT_SIZE, 
            audio_data->frequency_domain, audio_data->time_domain, FFTW_ESTIMATE);
        fftw_execute(inverse_plan);
        for (int i = 0; i < FFT_SIZE; i++) {
            audio_data->time_domain[i] /= FFT_SIZE; // Normalize
        }
        fftw_destroy_plan(inverse_plan);
    }
}

void apply_cochlear_filter(AudioData *audio_data, InnerEar *inner_ear) {
    if (!audio_data || !inner_ear || inner_ear->cochlea_frequency <= 0) {
        return;
    }
    for (int i = 0; i < FFT_SIZE/2 + 1; i++) {  
        double freq = (i * SAMPLING_RATE) / (double)FFT_SIZE;
        if (freq <= 0) continue;  // Protect against log(0)
        double weight = exp(-pow((log(freq) - log(inner_ear->cochlea_frequency)) / 0.5, 2));
        audio_data->frequency_domain[i][0] *= weight; // Real part
        audio_data->frequency_domain[i][1] *= weight; // Imaginary part
    }
}

void apply_window_function(AudioData *audio_data) {
    for (int i = 0; i < FFT_SIZE; i++) {
        double window = 0.5 * (1 - cos(2 * M_PI * i / (FFT_SIZE - 1)));
        audio_data->time_domain[i] *= window;
    }
}

void apply_convolution(AudioData *audio_data, const double *kernel, int kernel_size) {
    double *convolved = (double*)calloc(FFT_SIZE, sizeof(double));
    if (!convolved) {
        fprintf(stderr, "Memory allocation failed for convolution result\n");
        return;
    }

    for (int n = 0; n < FFT_SIZE; ++n) {
        for (int k = 0; k < kernel_size && k <= n; ++k) {
            convolved[n] += audio_data->time_domain[n - k] * kernel[k];
        }
    }

    memcpy(audio_data->time_domain, convolved, FFT_SIZE * sizeof(double));
    free(convolved);

    perform_fourier_transform(audio_data);
}

void apply_frequency_domain_convolution(AudioData *audio_data, const double *kernel, int kernel_size) {
    double *kernel_time = fftw_alloc_real(FFT_SIZE);
    fftw_complex *kernel_freq = fftw_alloc_complex(FFT_SIZE/2 + 1);

    memset(kernel_time, 0, FFT_SIZE * sizeof(double));
    memcpy(kernel_time, kernel, kernel_size * sizeof(double));

    fftw_plan kernel_plan = fftw_plan_dft_r2c_1d(FFT_SIZE, kernel_time, kernel_freq, FFTW_ESTIMATE);
    fftw_execute(kernel_plan);

    for (int i = 0; i < FFT_SIZE/2 + 1; i++) {
        double real = audio_data->frequency_domain[i][0] * kernel_freq[i][0] - 
                     audio_data->frequency_domain[i][1] * kernel_freq[i][1];
        double imag = audio_data->frequency_domain[i][0] * kernel_freq[i][1] + 
                     audio_data->frequency_domain[i][1] * kernel_freq[i][0];
        audio_data->frequency_domain[i][0] = real;
        audio_data->frequency_domain[i][1] = imag;
    }

    fftw_destroy_plan(kernel_plan);
    fftw_free(kernel_time);
    fftw_free(kernel_freq);
}

void process_with_overlap_add(AudioData *audio_data, const double *input, int input_length, int hop_size) {
    double *output = (double*)calloc(input_length + FFT_SIZE, sizeof(double));
    double *window = (double*)malloc(FFT_SIZE * sizeof(double));

    for (int i = 0; i < FFT_SIZE; i++) {
        window[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (FFT_SIZE - 1)));
    }

    for (int i = 0; i < input_length; i += hop_size) {
        for (int j = 0; j < FFT_SIZE; j++) {
            audio_data->time_domain[j] = (i + j < input_length) ? input[i + j] * window[j] : 0.0;
        }

        perform_fourier_transform(audio_data);
        apply_cochlear_filter(audio_data, NULL); // Using NULL for default cochlear filter
        perform_inverse_fft(audio_data);

        for (int j = 0; j < FFT_SIZE; j++) {
            if (i + j < input_length) {
                output[i + j] += audio_data->time_domain[j];
            }
        }
    }

    memcpy(audio_data->time_domain, output, input_length * sizeof(double));

    free(output);
    free(window);
}

// Signal Generation and Analysis
void generate_fibonacci_sequence(double *sequence, int *length) {
    const double MAX_SAFE_SUM = DBL_MAX / 2;  // Prevent overflow
    sequence[0] = MIN_FREQ;
    sequence[1] = MIN_FREQ + 1;
    int i = 2;
    while (sequence[i-1] < MAX_FREQ && i < 128) { 
        if (sequence[i-1] > MAX_SAFE_SUM - sequence[i-2]) {
            break;  // Prevent overflow
        }
        sequence[i] = sequence[i-1] + sequence[i-2];
        if (sequence[i] > MAX_FREQ) break;
        i++;
    }
    *length = i;
}

void simulate_octave_range(InnerEar *inner_ear) {
    double fibonacci_sequence[128];
    int sequence_length;
    generate_fibonacci_sequence(fibonacci_sequence, &sequence_length);

    MusicalNote octave_notes[] = {
        {"A", 440.0}, {"B", 493.88}, {"C", 523.25}, {"D", 587.33}, {"E", 659.26}, {"F", 698.46}, {"G", 783.99}
    };

    for (int i = 0; i < sequence_length; i++) {
        double freq = fibonacci_sequence[i];
        for (int oct = -2; oct <= 2; oct++) { 
            double adjusted_freq = freq * pow(2, oct);
            if (adjusted_freq >= MIN_FREQ && adjusted_freq <= MAX_FREQ) {
                for (int j = 0; j < NOTES_PER_OCTAVE; j++) {
                    if (fabs(adjusted_freq - octave_notes[j].frequency) < 20) { 
                        printf("Perceived note: %s at frequency %.2f Hz\n", octave_notes[j].note, adjusted_freq);
                        break;
                    }
                }
            }
        }
    }
}

void populate_time_domain_signal(AudioData *audio_data, double frequency, double duration) {
    if (!audio_data || frequency <= 0 || duration <= 0) {
        return;
    }
    memset(audio_data->time_domain, 0, FFT_SIZE * sizeof(double));
    for (int i = 0; i < FFT_SIZE; i++) {
        double time = i / (double)SAMPLING_RATE;
        if (time > duration) break;
        audio_data->time_domain[i] = sin(2 * M_PI * frequency * time);
    }
}

// Main Simulation Function
void simulate_cochlear_response(InnerEar *inner_ear, double frequency) {
    AudioData *audio_data = NULL;
    if (init_audio_data(&audio_data) != EARS_SUCCESS) {
        fprintf(stderr, "Failed to initialize audio data\n");
        return;
    }

    int input_length = FFT_SIZE * 10;
    double *input = (double*)malloc(input_length * sizeof(double));
    if (!input) {
        fprintf(stderr, "Failed to allocate memory for input signal\n");
        free_audio_data(audio_data);
        return;
    }
    for (int i = 0; i < input_length; i++) {
        input[i] = sin(2 * M_PI * frequency * i / SAMPLING_RATE);
    }

    process_with_overlap_add(audio_data, input, input_length, FFT_SIZE / 2); // Half overlap

    // Example kernel for convolution in frequency domain
    double kernel[] = {0.1, 0.2, 0.4, 0.2, 0.1};
    int kernel_size = sizeof(kernel) / sizeof(kernel[0]);
    apply_frequency_domain_convolution(audio_data, kernel, kernel_size);

    simulate_octave_range(inner_ear);

    free_audio_data(audio_data);
    free(input);
}
