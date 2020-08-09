#include "mono-live-vocoding.hpp"
#include "FftwTransform.hpp"
#include <phase-vocoder/PhaseVocoder.hpp>
#include <portaudio.h>
#include <gsl/gsl>
#include <algorithm>
#include <iostream>
#include <cstdio>

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

int mainMonoPhaseVocoding(PortAudioStreamModifier &streamModifier) {
    phase_vocoder::FftwTransformer<float>::FftwFactory factory;
    phase_vocoder::PhaseVocoder<float> vocoder{3, 2, 1024, factory};

    Pa_Initialize();

    PaStreamParameters inputParameters{};
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;

    PaStreamParameters outputParameters{};
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paFloat32;

    PaStream *stream{};

    Pa_OpenStream(&stream, &inputParameters, &outputParameters, 48000, 1024,
        paNoFlag, vocode, &vocoder);
    streamModifier.modify(stream);
    Pa_StartStream(stream);
    std::cout << "Press ENTER to exit: ";
    std::getchar();
    Pa_CloseStream(stream);
    Pa_Terminate();
}
