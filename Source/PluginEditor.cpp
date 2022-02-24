/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// =============================================================================
void RotarySliderWithLabelBelow::paint(juce::Graphics &g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                        sliderBounds.getX(),
                                        sliderBounds.getY(),
                                        sliderBounds.getWidth(),
                                        sliderBounds.getHeight(),
                                        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                        startAng,
                                        endAng,
                                        *this);

    auto labelBounds = getLabelBounds();
    g.drawFittedText(this->param->getParameterID().toUpperCase(), labelBounds, Justification::centred, 1);
}

juce::Rectangle<int> RotarySliderWithLabelBelow::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto boundsTop = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    float scaleFactor = 0.8f;
    float diameter = std::min<float>(boundsTop.getWidth() * scaleFactor, boundsTop.getHeight() * scaleFactor);
    auto sliderBounds = juce::Rectangle<int>(diameter,
                                                diameter);
    sliderBounds.setCentre(boundsTop.getCentre());

    return sliderBounds;
}

juce::Rectangle<int> RotarySliderWithLabelBelow::getLabelBounds() const
{
    auto bounds = getLocalBounds();
    auto sliderBounds = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto labelBounds = bounds.removeFromTop(getTextHeight() * 1.8f);
    return labelBounds;
}

// =============================================================================
void RotarySliderWithLabelAbove::paint(juce::Graphics &g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                        sliderBounds.getX(),
                                        sliderBounds.getY(),
                                        sliderBounds.getWidth(),
                                        sliderBounds.getHeight(),
                                        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                        startAng,
                                        endAng,
                                        *this);

    auto labelBounds = getLabelBounds();
    g.drawFittedText(this->param->getParameterID().toUpperCase(), labelBounds, Justification::centred, 1);
}

juce::Rectangle<int> RotarySliderWithLabelAbove::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto boundsBottom = bounds.removeFromBottom(bounds.getHeight() * 0.5f);
    float scaleFactor = 0.8f;
    float diameter = std::min<float>(boundsBottom.getWidth() * scaleFactor, boundsBottom.getHeight() * scaleFactor);
    auto sliderBounds = juce::Rectangle<int>(diameter,
                                                diameter);
    sliderBounds.setCentre(boundsBottom.getCentre());

    return sliderBounds;
}

juce::Rectangle<int> RotarySliderWithLabelAbove::getLabelBounds() const
{
    auto bounds = getLocalBounds();
    auto sliderBounds = bounds.removeFromBottom(bounds.getHeight() * 0.5f);
    auto labelBounds = bounds.removeFromBottom(getTextHeight() * 1.8f);
    return labelBounds;
}

//==============================================================================
PoopSmearerAudioProcessorEditor::PoopSmearerAudioProcessorEditor (PoopSmearerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    driveSlider(*audioProcessor.apvts.getParameter("Drive")),
    toneSlider(*audioProcessor.apvts.getParameter("Tone")),
    levelSlider(*audioProcessor.apvts.getParameter("Level")),
    driveSliderAttachment(audioProcessor.apvts, "Drive", driveSlider),
    toneSliderAttachment(audioProcessor.apvts, "Tone", toneSlider),
    levelSliderAttachment(audioProcessor.apvts, "Level", levelSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }
    
    setSize (300, 475);
}

PoopSmearerAudioProcessorEditor::~PoopSmearerAudioProcessorEditor()
{
}

//==============================================================================
void PoopSmearerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    using namespace juce;
    g.fillAll (Colours::black);

    auto bounds = getLocalBounds();
    float scaleFactor = 0.8f;
    auto pedalArea = Rectangle<float>(bounds.getWidth() * scaleFactor,
                                        bounds.getHeight() * scaleFactor);
    pedalArea.setCentre(bounds.getCentreX(), bounds.getCentreY());
    g.setColour(Colours::green);
    g.fillRoundedRectangle(pedalArea, 10.f);

    auto pedalBottomArea = pedalArea.removeFromBottom(pedalArea.getHeight() * 0.66f);
    auto buttonLabelArea = Rectangle<float>(pedalBottomArea.getWidth() * scaleFactor,
                                            pedalBottomArea.getHeight() * scaleFactor);
    buttonLabelArea.setCentre(pedalBottomArea.getCentreX(),
                                pedalBottomArea.getCentreY());
    
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(buttonLabelArea, 5.f);

    auto buttonLabelTop = buttonLabelArea.removeFromTop(buttonLabelArea.getHeight() * 0.5f);
    auto pedalLabelArea = Rectangle<float>(buttonLabelTop.getWidth() * 0.9f,
                                            buttonLabelTop.getHeight() * 0.9f);
    pedalLabelArea.setCentre(buttonLabelTop.getCentre());
    g.setColour(Colours::silver);
    g.fillRoundedRectangle(pedalLabelArea, 4.5f);

    g.setColour (juce::Colours::blue);
    g.setFont (22.0f);
    g.drawFittedText ("Poop Smearer\nPS9", pedalLabelArea.toNearestInt(), juce::Justification::centred, 1);

    auto buttonArea = Rectangle<float>(buttonLabelArea.getWidth() * 0.9f,
                                        buttonLabelArea.getHeight() * 0.9f);
    buttonArea.setCentre(buttonLabelArea.getCentre());
    g.setColour(Colours::silver);
    g.fillRoundedRectangle(buttonArea, 4.5f);

    auto tagArea = pedalBottomArea.removeFromBottom(pedalBottomArea.getHeight() * 0.1f);
    g.setColour(Colours::black);
    g.setFont(16.0f);
    g.drawFittedText("Bob's Plugin Bargain Bin", tagArea.toNearestInt(), Justification::centred, 1);
}

void PoopSmearerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();

    float scaleFactor = 0.8f;

    using namespace juce;
    auto pedalBounds = Rectangle<float>(bounds.getWidth() * scaleFactor,
                                        bounds.getHeight() * scaleFactor);
    pedalBounds.setCentre(bounds.getCentreX(), bounds.getCentreY());

    auto knobArea = pedalBounds.removeFromTop(pedalBounds.getHeight() * 0.33f);
    
    auto driveArea = knobArea.removeFromLeft(knobArea.getWidth() * 0.33f);

    auto levelArea = knobArea.removeFromRight(knobArea.getWidth() * 0.5f);

    driveSlider.setBounds(driveArea.toNearestInt());
    levelSlider.setBounds(levelArea.toNearestInt());
    toneSlider.setBounds(knobArea.toNearestInt());
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
