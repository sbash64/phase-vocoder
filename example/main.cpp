#include "FftwTransform.hpp"
#include <phase-vocoder/PhaseVocoder.hpp>
#include <portaudio.h>
#include <pa_linux_alsa.h>
#include <gsl/gsl>
#include <algorithm>

static void zero(gsl::span<float> x) { std::fill(begin(x), end(x), 0.F); }

static void copy(gsl::span<const float> source, gsl::span<float> destination) {
    std::copy(begin(source), end(source), begin(destination));
}

static auto vocode(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags, void *userData) -> int {
    auto *out = static_cast<float *>(outputBuffer);
    const auto *in = static_cast<const float *>(inputBuffer);
    if (in == nullptr)
        zero({out, framesPerBuffer});
    else
        copy({in, framesPerBuffer}, {out, framesPerBuffer});
    static_cast<phase_vocoder::PhaseVocoder<float> *>(userData)->vocode(
        {out, framesPerBuffer});

    return paContinue;
}

int main() {
    phase_vocoder::FftwTransformer<float>::FftwFactory factory;
    phase_vocoder::PhaseVocoder<float> vocoder{3, 2, 1024, factory};

    if (Pa_Initialize() != paNoError)
        return -1;

    PaStreamParameters inputParameters{};
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        return -1;
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;

    PaStreamParameters outputParameters{};
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default output device.\n");
        return -1;
    }
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paFloat32;
    PaStream *stream{};

    if (Pa_OpenStream(&stream, &inputParameters, &outputParameters, 48000, 1024,
            paNoFlag, vocode, &vocoder) != paNoError)
        return -1;
    PaAlsa_EnableRealtimeScheduling(stream, 1);
    if (Pa_StartStream(stream) != paNoError)
        return -1;

    printf("Hit ENTER to stop program.\n");
    getchar();
    if (Pa_CloseStream(stream) != paNoError)
        return -1;

    Pa_Terminate();
}
