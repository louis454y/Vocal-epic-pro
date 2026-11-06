#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

// Orden de los parametros (el mismo orden se usa en presets y en la interfaz)
enum ParamIndex
{
    kInput = 0, kWarmth, kPresence, kAir,
    kComp, kDeess, kDrive,
    kDelay, kReverb, kMix, kOutput,
    kNumParams
};

class VozProProcessor : public juce::AudioProcessor
{
public:
    VozProProcessor();
    ~VozProProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal Epic Pro"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::AudioProcessorValueTreeState apvts;

    // Niveles para los medidores de la interfaz
    std::atomic<float> inputLevel  { 0.0f };
    std::atomic<float> outputLevel { 0.0f };

private:
    using Filter     = juce::dsp::IIR::Filter<float>;
    using Coeffs     = juce::dsp::IIR::Coefficients<float>;
    using Duplicator = juce::dsp::ProcessorDuplicator<Filter, Coeffs>;

    Duplicator hpf, lowShelf, peakBand, highShelf, sibilance;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::Reverb reverb;

    juce::AudioBuffer<float> dryBuffer, scBuffer, reverbBuffer;
    std::array<std::atomic<float>*, kNumParams> params {};

    double sr = 44100.0;
    float lastInGain = 1.0f, lastOutGain = 1.0f;
    float deessEnv = 0.0f;
    float delayLp[2] { 0.0f, 0.0f };
    int currentPreset = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VozProProcessor)
};
