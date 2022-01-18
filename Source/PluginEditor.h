/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
// Custom Rotary Slider
// struct RotarySliderWithLabel : juce::Slider
// {
//     RotarySliderWithLabel() : juce::Slider(
//         juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
//         juce::Slider::TextEntryBoxPosition::TextBoxBelow
//     )
//     {

//     }
// };

struct LookAndFeel : juce::LookAndFeel_V4
{
    virtual void drawRotarySlider(juce::Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPosProportional,
                                    float rotaryStartAngle,
                                    float rotaryEndAngle,
                                    juce::Slider&) override;
};

struct RotarySliderWithLabel : juce::Slider
{
    RotarySliderWithLabel(juce::RangedAudioParameter& rap) :
    juce::Slider(
        juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabel()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
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

    // Add sliders
    RotarySliderWithLabel driveSlider, toneSlider, levelSlider;

    // Add attachments
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment driveSliderAttachment, toneSliderAttachment, levelSliderAttachment;

    // get editor components
    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PoopSmearerAudioProcessorEditor)
};
