/*
  ==============================================================================

    LookAndFeel.h
    Created: 24 Feb 2022 2:24:01pm
    Author:  bob

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
    virtual void drawRotarySlider(juce::Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPosProportional,
                                    float rotaryStartAngle,
                                    float rotaryEndAngle,
                                    juce::Slider&) override;
};