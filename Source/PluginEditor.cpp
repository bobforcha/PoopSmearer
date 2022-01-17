/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PoopSmearerAudioProcessorEditor::PoopSmearerAudioProcessorEditor (PoopSmearerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }
    
    setSize (300, 570);
}

PoopSmearerAudioProcessorEditor::~PoopSmearerAudioProcessorEditor()
{
}

//==============================================================================
void PoopSmearerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void PoopSmearerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto center = bounds.getCentre();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    using namespace juce;
    auto knobArea = bounds.removeFromTop(height * 0.33f);
    auto driveKnobArea = knobArea.removeFromLeft(width * 0.33f);
    auto levelKnobArea = knobArea.removeFromRight(width * 0.33f);

    driveSlider.setBounds(driveKnobArea);
    levelSlider.setBounds(levelKnobArea);
    toneSlider.setBounds(knobArea);
}

std::vector<juce::Component*> PoopSmearerAudioProcessorEditor::getComps()
{
    return
    {
        &driveSlider,
        &toneSlider,
        &levelSlider
    };
}
