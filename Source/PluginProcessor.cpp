// Vocal Epic Pro 2.0 - Cadena vocal profesional (VST3)
// Autor: Louis Butrón
// Copyright (c) 2026 Louis Butrón. Todos los derechos reservados.

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    // Mismo orden que ParamIndex
    const char* const paramIds[kNumParams] =
        { "input", "warmth", "presence", "air", "comp", "deess", "drive", "delay", "reverb", "mix", "output" };

    struct Preset { const char* name; float v[kNumParams]; };

    //                                          ent   calid pres  aire  comp  sibil satur eco   reverb mezcla sal
    const Preset presets[] =
    {
        { "Reggaetón - Voz Principal",            { 0.f,  1.5f,  2.5f, 3.0f, 0.55f, 0.45f, 0.20f, 0.12f, 0.18f, 1.f,  0.f } },
        { "Reggaetón - Segunda Voz",              { 0.f,  2.0f,  0.5f, 2.0f, 0.65f, 0.50f, 0.15f, 0.15f, 0.30f, 1.f, -2.f } },
        { "Reggaetón - Coros",                    { 0.f,  1.5f, -0.5f, 3.5f, 0.60f, 0.40f, 0.05f, 0.20f, 0.50f, 1.f, -3.f } },
        { "Trap - Voz Principal",                 { 0.f, -1.0f,  3.5f, 2.0f, 0.70f, 0.50f, 0.40f, 0.08f, 0.12f, 1.f, -1.f } },
        { "Trap - Segunda Voz (Ad-libs)",         { 0.f,  0.0f,  2.0f, 3.0f, 0.60f, 0.40f, 0.30f, 0.35f, 0.35f, 1.f, -2.f } },
        { "Afrobeats - Voz Principal",            { 0.f,  2.0f,  1.5f, 2.5f, 0.40f, 0.35f, 0.10f, 0.20f, 0.30f, 1.f,  0.f } },
        { "Afrobeats - Coros",                    { 0.f,  2.0f,  0.0f, 4.0f, 0.50f, 0.35f, 0.05f, 0.25f, 0.50f, 1.f, -3.f } },
        { "Deep House - Voz Principal",           { 0.f,  0.5f,  1.0f, 5.0f, 0.35f, 0.40f, 0.05f, 0.25f, 0.45f, 1.f,  0.f } },
        { "Deep House - Coros Etéreos",           { 0.f,  0.0f, -1.0f, 6.0f, 0.40f, 0.40f, 0.00f, 0.40f, 0.70f, 1.f, -3.f } },
        { "Segundas Voces - General",             { 0.f,  1.0f,  0.0f, 2.0f, 0.60f, 0.45f, 0.10f, 0.10f, 0.30f, 1.f, -2.f } },
        { "Coros - General",                      { 0.f,  1.0f, -1.0f, 3.0f, 0.60f, 0.40f, 0.05f, 0.15f, 0.55f, 1.f, -3.f } },
        { "Voz Limpia Natural",                   { 0.f,  0.0f,  0.5f, 1.0f, 0.30f, 0.30f, 0.00f, 0.00f, 0.10f, 1.f,  0.f } }
    };
}

juce::AudioProcessorValueTreeState::ParameterLayout VozProProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto dbText = [] (float v, int) { return String (v, 1) + " dB"; };
    auto pctText = [] (float v, int) { return String (roundToInt (v * 100.0f)) + " %"; };

    auto addDb = [&] (const char* id, const juce::String& name, float lo, float hi, float def)
    {
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { id, 1 }, name, NormalisableRange<float> (lo, hi, 0.1f), def,
            AudioParameterFloatAttributes().withStringFromValueFunction (dbText)));
    };
    auto addPct = [&] (const char* id, const juce::String& name, float def)
    {
        layout.add (std::make_unique<AudioParameterFloat> (
            ParameterID { id, 1 }, name, NormalisableRange<float> (0.0f, 1.0f, 0.001f), def,
            AudioParameterFloatAttributes().withStringFromValueFunction (pctText)));
    };

    addDb  ("input",    juce::String::fromUTF8 ("Entrada"),    -12.0f, 12.0f, 0.0f);
    addDb  ("warmth",   juce::String::fromUTF8 ("Calidez"),    -6.0f,  6.0f, 0.0f);
    addDb  ("presence", juce::String::fromUTF8 ("Presencia"),  -6.0f,  6.0f, 0.0f);
    addDb  ("air",      juce::String::fromUTF8 ("Aire"),        0.0f,  8.0f, 0.0f);
    addPct ("comp",     juce::String::fromUTF8 ("Compresión"),  0.40f);
    addPct ("deess",    juce::String::fromUTF8 ("Anti-sibilancia"),    0.30f);
    addPct ("drive",    juce::String::fromUTF8 ("Saturación"),     0.10f);
    addPct ("delay",    juce::String::fromUTF8 ("Eco"),     0.10f);
    addPct ("reverb",   juce::String::fromUTF8 ("Reverb"),    0.15f);
    addPct ("mix",      juce::String::fromUTF8 ("Mezcla"),       1.00f);
    addDb  ("output",   juce::String::fromUTF8 ("Salida"),   -12.0f, 12.0f, 0.0f);

    return layout;
}

VozProProcessor::VozProProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    for (int i = 0; i < kNumParams; ++i)
        params[(size_t) i] = apvts.getRawParameterValue (paramIds[i]);
}

bool VozProProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return out == layouts.getMainInputChannelSet();
}

void VozProProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock,
                                  (juce::uint32) juce::jmax (1, getTotalNumOutputChannels()) };

    hpf.prepare (spec);  lowShelf.prepare (spec);  peakBand.prepare (spec);
    highShelf.prepare (spec);  sibilance.prepare (spec);
    compressor.prepare (spec);

    *hpf.state       = *Coeffs::makeHighPass (sampleRate, 80.0f);
    *sibilance.state = *Coeffs::makeHighPass (sampleRate, 5500.0f);
    *lowShelf.state  = *Coeffs::makeLowShelf (sampleRate, 200.0f, 0.7f, 1.0f);
    *peakBand.state  = *Coeffs::makePeakFilter (sampleRate, 3500.0f, 0.9f, 1.0f);
    *highShelf.state = *Coeffs::makeHighShelf (sampleRate, 11000.0f, 0.7f, 1.0f);

    delayLine.setMaximumDelayInSamples ((int) (sampleRate * 1.5));
    delayLine.prepare (spec);
    delayLine.reset();

    reverb.setSampleRate (sampleRate);
    reverb.reset();

    dryBuffer.setSize ((int) spec.numChannels, samplesPerBlock);
    scBuffer.setSize ((int) spec.numChannels, samplesPerBlock);
    reverbBuffer.setSize ((int) spec.numChannels, samplesPerBlock);

    lastInGain = lastOutGain = 1.0f;
    deessEnv = 0.0f;
    delayLp[0] = delayLp[1] = 0.0f;
}

void VozProProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = buffer.getNumChannels();
    const int n     = buffer.getNumSamples();

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, n);

    if (numCh == 0 || n == 0)
        return;

    auto P = [this] (int i) { return params[(size_t) i]->load(); };

    // ---------- 1. Ganancia de entrada ----------
    const float inG = juce::Decibels::decibelsToGain (P (kInput));
    buffer.applyGainRamp (0, n, lastInGain, inG);
    lastInGain = inG;

    {
        const float cur = inputLevel.load();
        inputLevel.store (juce::jmax (cur, buffer.getMagnitude (0, n)));
    }

    // Copia seca para el control Mix
    dryBuffer.setSize (numCh, n, false, false, true);
    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom (ch, 0, buffer, ch, 0, n);

    // ---------- 2. Filtro de graves + EQ de tono ----------
    *lowShelf.state  = *Coeffs::makeLowShelf   (sr, 200.0f,   0.7f, juce::Decibels::decibelsToGain (P (kWarmth)));
    *peakBand.state  = *Coeffs::makePeakFilter (sr, 3500.0f,  0.9f, juce::Decibels::decibelsToGain (P (kPresence)));
    *highShelf.state = *Coeffs::makeHighShelf  (sr, 11000.0f, 0.7f, juce::Decibels::decibelsToGain (P (kAir)));

    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        hpf.process (ctx);
        lowShelf.process (ctx);
        peakBand.process (ctx);
        highShelf.process (ctx);
    }

    // ---------- 3. De-esser ----------
    const float ds = P (kDeess);
    if (ds > 0.01f)
    {
        scBuffer.setSize (numCh, n, false, false, true);
        for (int ch = 0; ch < numCh; ++ch)
            scBuffer.copyFrom (ch, 0, buffer, ch, 0, n);

        juce::dsp::AudioBlock<float> scBlock (scBuffer);
        sibilance.process (juce::dsp::ProcessContextReplacing<float> (scBlock));

        const float thr = juce::Decibels::decibelsToGain (-12.0f - 26.0f * ds);
        const float att = std::exp (-1.0f / (0.0005f * (float) sr));
        const float rel = std::exp (-1.0f / (0.04f   * (float) sr));

        float* w[2] = { buffer.getWritePointer (0), numCh > 1 ? buffer.getWritePointer (1) : nullptr };
        const float* s[2] = { scBuffer.getReadPointer (0), numCh > 1 ? scBuffer.getReadPointer (1) : nullptr };

        for (int i = 0; i < n; ++i)
        {
            float det = std::abs (s[0][i]);
            if (s[1] != nullptr) det = juce::jmax (det, std::abs (s[1][i]));

            const float coef = det > deessEnv ? att : rel;
            deessEnv = coef * deessEnv + (1.0f - coef) * det;

            float g = 1.0f;
            if (deessEnv > thr)
                g = juce::jmax (0.35f, std::pow (thr / deessEnv, 0.75f));

            w[0][i] *= g;
            if (w[1] != nullptr) w[1][i] *= g;
        }
    }

    // ---------- 4. Compresor ----------
    const float cp = P (kComp);
    if (cp > 0.01f)
    {
        compressor.setThreshold (-6.0f - 28.0f * cp);
        compressor.setRatio (2.0f + 3.0f * cp);
        compressor.setAttack (8.0f);
        compressor.setRelease (90.0f);

        juce::dsp::AudioBlock<float> block (buffer);
        compressor.process (juce::dsp::ProcessContextReplacing<float> (block));

        buffer.applyGain (juce::Decibels::decibelsToGain (cp * 6.0f));
    }

    // ---------- 5. Saturacion ----------
    const float dr = P (kDrive);
    if (dr > 0.01f)
    {
        const float g = 1.0f + dr * 6.0f;
        for (int ch = 0; ch < numCh; ++ch)
        {
            float* d = buffer.getWritePointer (ch);
            for (int i = 0; i < n; ++i)
            {
                const float x = d[i];
                const float y = std::tanh (g * x) / g * (1.0f + dr * 0.8f);
                d[i] = x + (y - x) * dr;
            }
        }
    }

    // ---------- 6. Delay estereo ----------
    const float dl = P (kDelay);
    if (dl > 0.01f)
    {
        const float timeSec[2] = { 0.27f, 0.405f };
        for (int ch = 0; ch < numCh; ++ch)
        {
            float* d = buffer.getWritePointer (ch);
            const float delaySamples = timeSec[ch] * (float) sr;
            for (int i = 0; i < n; ++i)
            {
                const float in = d[i];
                float del = delayLine.popSample (ch, delaySamples);
                delayLp[ch] += 0.35f * (del - delayLp[ch]);   // oscurece las repeticiones
                del = delayLp[ch];
                delayLine.pushSample (ch, in + del * 0.35f);
                d[i] = in + del * dl * 0.6f;
            }
        }
    }

    // ---------- 7. Reverb ----------
    const float rv = P (kReverb);
    if (rv > 0.01f)
    {
        reverbBuffer.setSize (numCh, n, false, false, true);
        for (int ch = 0; ch < numCh; ++ch)
            reverbBuffer.copyFrom (ch, 0, buffer, ch, 0, n);

        juce::Reverb::Parameters rp;
        rp.roomSize = 0.75f;  rp.damping = 0.5f;  rp.wetLevel = 1.0f;
        rp.dryLevel = 0.0f;   rp.width = 1.0f;    rp.freezeMode = 0.0f;
        reverb.setParameters (rp);

        if (numCh >= 2)
            reverb.processStereo (reverbBuffer.getWritePointer (0), reverbBuffer.getWritePointer (1), n);
        else
            reverb.processMono (reverbBuffer.getWritePointer (0), n);

        for (int ch = 0; ch < numCh; ++ch)
            buffer.addFrom (ch, 0, reverbBuffer, ch, 0, n, rv * 0.22f);
    }

    // ---------- 8. Mix seco/mojado ----------
    const float mix = P (kMix);
    if (mix < 0.999f)
    {
        for (int ch = 0; ch < numCh; ++ch)
        {
            buffer.applyGain (ch, 0, n, mix);
            buffer.addFrom (ch, 0, dryBuffer, ch, 0, n, 1.0f - mix);
        }
    }

    // ---------- 9. Ganancia de salida ----------
    const float outG = juce::Decibels::decibelsToGain (P (kOutput));
    buffer.applyGainRamp (0, n, lastOutGain, outG);
    lastOutGain = outG;

    {
        const float cur = outputLevel.load();
        outputLevel.store (juce::jmax (cur, buffer.getMagnitude (0, n)));
    }
}

juce::AudioProcessorEditor* VozProProcessor::createEditor()
{
    return new VozProEditor (*this);
}

int VozProProcessor::getNumPrograms() { return (int) std::size (presets); }

const juce::String VozProProcessor::getProgramName (int index)
{
    if (index < 0 || index >= getNumPrograms()) return {};
    return juce::String::fromUTF8 (presets[index].name);
}

void VozProProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= getNumPrograms()) return;
    currentPreset = index;

    for (int k = 0; k < kNumParams; ++k)
        if (auto* prm = apvts.getParameter (paramIds[k]))
            prm->setValueNotifyingHost (prm->convertTo0to1 (presets[index].v[k]));
}

void VozProProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VozProProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VozProProcessor();
}
