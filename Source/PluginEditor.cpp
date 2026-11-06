// Vocal Epic Pro 2.0 - Interfaz grafica
// Autor: Louis Butrón
// Copyright (c) 2026 Louis Butrón. Todos los derechos reservados.

#include "PluginEditor.h"

namespace
{
    struct KnobInfo { const char* id; const char* name; };

    // Mismo orden que ParamIndex
    const KnobInfo infos[kNumParams] =
    {
        { "input", "ENTRADA" }, { "warmth", "CALIDEZ" }, { "presence", "PRESENCIA" }, { "air", "AIRE" },
        { "comp", "COMPRESIÓN" }, { "deess", "ANTI-SIBILANCIA" }, { "drive", "SATURACIÓN" },
        { "delay", "ECO" }, { "reverb", "REVERB" }, { "mix", "MEZCLA" }, { "output", "SALIDA" }
    };

    // Filas de perillas: cuantas hay en cada seccion
    const int rowCounts[3]      = { 4, 3, 4 };
    const char* const rowNames[3] = { "TONO", "DINÁMICA", "ESPACIO  Y  SALIDA" };
    const int rowY[3]           = { 104, 264, 424 };

    constexpr int kCellW = 170, kCellH = 130, kStartX = 30;
    const juce::Rectangle<int> meterArea (725, 100, 90, 490);
}

VozProEditor::VozProEditor (VozProProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);

    for (int i = 0; i < kNumParams; ++i)
    {
        auto& k = knobs[(size_t) i];

        k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        k.slider.setTextBoxIsEditable (false);
        k.slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                      juce::MathConstants<float>::pi * 2.8f, true);
        k.slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffcfd8e6));
        k.slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible (k.slider);

        k.label.setText (juce::String::fromUTF8 (infos[i].name), juce::dontSendNotification);
        k.label.setJustificationType (juce::Justification::centred);
        k.label.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
        k.label.setColour (juce::Label::textColourId, juce::Colour (0xff8fa3c0));
        addAndMakeVisible (k.label);

        k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            proc.apvts, infos[i].id, k.slider);
    }

    for (int i = 0; i < proc.getNumPrograms(); ++i)
        presetBox.addItem (proc.getProgramName (i), i + 1);

    presetBox.setSelectedItemIndex (juce::jmax (0, proc.getCurrentProgram()), juce::dontSendNotification);
    presetBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff0d1220));
    presetBox.setColour (juce::ComboBox::textColourId, juce::Colours::white);
    presetBox.setColour (juce::ComboBox::outlineColourId, EpicLookAndFeel::cyan().withAlpha (0.6f));
    presetBox.setColour (juce::ComboBox::arrowColourId, EpicLookAndFeel::cyan());
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedItemIndex()); };
    addAndMakeVisible (presetBox);

    setSize (820, 636);
    startTimerHz (30);
}

VozProEditor::~VozProEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void VozProEditor::timerCallback()
{
    auto toDisplay = [] (float lin)
    {
        return juce::jlimit (0.0f, 1.0f, (juce::Decibels::gainToDecibels (lin, -60.0f) + 60.0f) / 60.0f);
    };

    const float i = toDisplay (proc.inputLevel.exchange (0.0f));
    const float o = toDisplay (proc.outputLevel.exchange (0.0f));

    inLevel  = i > inLevel  ? i : inLevel  * 0.88f + i * 0.12f;
    outLevel = o > outLevel ? o : outLevel * 0.88f + o * 0.12f;

    repaint (meterArea);
}

void VozProEditor::paint (juce::Graphics& g)
{
    const auto cyan = EpicLookAndFeel::cyan();

    // Fondo
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff05070d), 0.0f, 0.0f,
                                             juce::Colour (0xff121a2b), 0.0f, (float) getHeight(), false));
    g.fillAll();

    // Resplandor superior
    g.setGradientFill (juce::ColourGradient (cyan.withAlpha (0.14f), 160.0f, 0.0f,
                                             juce::Colours::transparentBlack, 160.0f, 300.0f, true));
    g.fillRect (0, 0, getWidth(), 300);

    // Titulo con brillo
    const auto titleFont = juce::Font (juce::FontOptions (36.0f, juce::Font::bold));
    g.setFont (titleFont);
    g.setColour (cyan.withAlpha (0.25f));
    g.drawText ("VOCAL EPIC PRO", 28, 12, 440, 52, juce::Justification::centredLeft);
    g.setColour (juce::Colours::white);
    g.drawText ("VOCAL EPIC PRO", 30, 14, 440, 50, juce::Justification::centredLeft);

    g.setFont (11.5f);
    g.setColour (juce::Colour (0xff7f93b3));
    g.drawText (juce::String::fromUTF8 ("SUITE DE PRODUCCIÓN VOCAL   -   REGGAETÓN  /  TRAP  /  AFROBEATS  /  DEEP HOUSE"),
                32, 62, 620, 18, juce::Justification::centredLeft);

    g.setColour (cyan.withAlpha (0.35f));
    g.fillRect (30, 92, getWidth() - 60, 1);

    // Etiqueta de presets
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xff7f93b3));
    g.drawText ("PRESETS  (VOZ PRINCIPAL / SEGUNDAS VOCES / COROS)", getWidth() - 330, 8, 300, 18, juce::Justification::centredLeft);

    // Titulos de seccion
    for (int r = 0; r < 3; ++r)
    {
        g.setColour (cyan);
        g.fillRect (kStartX, rowY[r] + 4, 3, 14);
        g.setFont (juce::Font (juce::FontOptions (12.5f, juce::Font::bold)));
        g.setColour (juce::Colour (0xffb7c6e0));
        g.drawText (juce::String::fromUTF8 (rowNames[r]), kStartX + 10, rowY[r], 300, 22, juce::Justification::centredLeft);
        g.setColour (juce::Colour (0xff1c2538));
        g.fillRect (kStartX + 170, rowY[r] + 11, 520 - 170, 1);
    }

    // Medidores
    auto drawBar = [&] (juce::Rectangle<float> r, float level)
    {
        g.setColour (juce::Colour (0xff0b0f18));
        g.fillRoundedRectangle (r, 5.0f);
        auto fill = r.withTrimmedTop (r.getHeight() * (1.0f - level));
        g.setGradientFill (juce::ColourGradient (EpicLookAndFeel::magenta(), r.getX(), r.getY(),
                                                 cyan, r.getX(), r.getBottom(), false));
        g.fillRoundedRectangle (fill, 5.0f);
        g.setColour (juce::Colour (0xff2a3550));
        g.drawRoundedRectangle (r, 5.0f, 1.0f);
    };

    drawBar ({ 735.0f, 120.0f, 24.0f, 430.0f }, inLevel);
    drawBar ({ 771.0f, 120.0f, 24.0f, 430.0f }, outLevel);

    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xff7f93b3));
    g.drawText ("ENT", 731, 556, 32, 16, juce::Justification::centred);
    g.drawText ("SAL", 767, 556, 32, 16, juce::Justification::centred);

    // Derechos de autor
    g.setColour (cyan.withAlpha (0.25f));
    g.fillRect (30, 606, getWidth() - 60, 1);
    g.setFont (11.0f);
    g.setColour (juce::Colour (0xff7f93b3));
    g.drawText (juce::String::fromUTF8 ("Vocal Epic Pro  v2.0   |   Autor: Louis Butrón   |   © 2026 Louis Butrón. Todos los derechos reservados."),
                30, 610, getWidth() - 60, 18, juce::Justification::centredLeft);
}

void VozProEditor::resized()
{
    presetBox.setBounds (getWidth() - 330, 28, 300, 30);

    int index = 0;
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < rowCounts[r]; ++c)
        {
            auto& k = knobs[(size_t) index++];
            const int x = kStartX + c * kCellW;
            const int y = rowY[r] + 26;
            k.label.setBounds  (x, y, kCellW, 18);
            k.slider.setBounds (x, y + 18, kCellW, kCellH - 18);
        }
    }
}
