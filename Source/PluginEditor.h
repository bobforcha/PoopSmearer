/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
// Background pedal image
struct PedalBackground : juce::ImageComponent
{
    PedalBackground();

    ~PedalBackground()
    {}

    juce::Rectangle<int> getPedalArea();
    juce::Rectangle<int> getKnobArea();
    juce::Rectangle<int> getDriveKnobArea();
    juce::Rectangle<int> getToneKnobArea();
    juce::Rectangle<int> getLevelKnobArea();
    juce::Rectangle<int> getButtonArea();
    juce::Rectangle<int> getLEDArea(); 

private:
    juce::Image background;
};

// Bypass Button
struct BypassButton : juce::Button
{
    BypassButton(juce::RangedAudioParameter& rap) : juce::Button("Bypass"), param(&rap)
    {
        setLookAndFeel(&lnf);
        setAlwaysOnTop(true);
    }
    ~BypassButton()
    {
        setLookAndFeel(nullptr);
    }

    void paintButton(juce::Graphics& g, 
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
    juce::Rectangle<int> getButtonBounds() const;

private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
};

// Custom Rotary Slider
struct RotarySliderWithLabelBelow : juce::Slider
{
    RotarySliderWithLabelBelow(juce::RangedAudioParameter& rap) :
    juce::Slider(
        juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabelBelow()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    juce::Rectangle<int> getLabelBounds() const;
    int getTextHeight() const { return 16; }
    juce::String getDisplayString() const;

private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
};

struct RotarySliderWithLabelAbove : juce::Slider
{
    RotarySliderWithLabelAbove(juce::RangedAudioParameter& rap) :
    juce::Slider(
        juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabelAbove()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    juce::Rectangle<int> getLabelBounds() const;
    int getTextHeight() const { return 16; }
    juce::String getDisplayString() const;

private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
};
class PoopSmearerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PoopSmearerAudioProcessorEditor (PoopSmearerAudioProcessor&);
    ~PoopSmearerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PoopSmearerAudioProcessor& audioProcessor;

    // Background Image Component
    PedalBackground pedalBackground;

    // Add bypass switch
    BypassButton bypassButton;

    // Add sliders
    RotarySliderWithLabelBelow driveSlider, levelSlider;
    RotarySliderWithLabelAbove toneSlider;

    // Add attachments
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment driveSliderAttachment, toneSliderAttachment, levelSliderAttachment;
    APVTS::ButtonAttachment bypassButtonAttachment;

    // get editor components
    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PoopSmearerAudioProcessorEditor)
};
