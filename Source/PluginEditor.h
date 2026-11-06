#pragma once

#include "PluginProcessor.h"

// Estilo visual: perillas con brillo neon sobre fondo oscuro
class EpicLookAndFeel : public juce::LookAndFeel_V4
{
public:
    static juce::Colour cyan()    { return juce::Colour (0xff00e5ff); }
    static juce::Colour magenta() { return juce::Colour (0xffff2d6f); }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (6.0f);
        const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        const auto centre = bounds.getCentre();
        const float angle = startAngle + pos * (endAngle - startAngle);

        // Pista de fondo
        juce::Path track;
        track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, startAngle, endAngle, true);
        g.setColour (juce::Colour (0xff1b2232));
        g.strokePath (track, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Arco de valor con resplandor
        juce::Path value;
        value.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, startAngle, angle, true);
        g.setColour (cyan().withAlpha (0.10f));
        g.strokePath (value, juce::PathStrokeType (16.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (cyan().withAlpha (0.25f));
        g.strokePath (value, juce::PathStrokeType (9.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (cyan());
        g.strokePath (value, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Cuerpo de la perilla
        const float inner = radius * 0.68f;
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3d465c), centre.x, centre.y - inner,
                                                 juce::Colour (0xff10141d), centre.x, centre.y + inner, false));
        g.fillEllipse (centre.x - inner, centre.y - inner, inner * 2.0f, inner * 2.0f);
        g.setColour (juce::Colour (0xff566079));
        g.drawEllipse (centre.x - inner, centre.y - inner, inner * 2.0f, inner * 2.0f, 1.2f);

        // Indicador
        juce::Path pointer;
        pointer.addRoundedRectangle (-1.8f, -inner + 4.0f, 3.6f, inner * 0.42f, 1.8f);
        g.setColour (juce::Colours::white);
        g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    }
};

class VozProEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit VozProEditor (VozProProcessor&);
    ~VozProEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    VozProProcessor& proc;
    EpicLookAndFeel lnf;   // debe declararse antes que los controles

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    std::array<Knob, kNumParams> knobs;
    juce::ComboBox presetBox;

    float inLevel = 0.0f, outLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VozProEditor)
};
