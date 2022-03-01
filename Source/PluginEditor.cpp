/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// =============================================================================
PedalBackground::PedalBackground()
{
    // background = drawBackgroundImage();
    background = juce::ImageCache::getFromMemory(BinaryData::PoopSmearerPedal_png,
                                                    BinaryData::PoopSmearerPedal_pngSize);
    setImage(background);
    setAlwaysOnTop(false);
}

juce::Rectangle<int> PedalBackground::getPedalArea()
{
    auto bounds = getLocalBounds();
    float scaleFactor = 0.8f;
    auto pedalBounds = juce::Rectangle<int>(bounds.getWidth() * scaleFactor,
                                            bounds.getHeight() * scaleFactor);
    pedalBounds.setCentre(bounds.getCentre());

    return pedalBounds;
}

juce::Rectangle<int> PedalBackground::getKnobArea()
{
    return getPedalArea().removeFromTop(getHeight() * 0.33f);
}

juce::Rectangle<int> PedalBackground::getDriveKnobArea()
{
    auto knobArea = getKnobArea();
    return knobArea.removeFromLeft(knobArea.getWidth() * 0.33f);
}

juce::Rectangle<int> PedalBackground::getLevelKnobArea()
{
    auto knobArea = getKnobArea();
    return knobArea.removeFromRight(knobArea.getWidth() * 0.33f);
}

juce::Rectangle<int> PedalBackground::getToneKnobArea()
{
    auto knobArea = getKnobArea();
    auto driveArea = knobArea.removeFromLeft(knobArea.getWidth() * 0.33f);
    auto levelArea = knobArea.removeFromRight(knobArea.getWidth() * 0.5f);
    return knobArea;
}

juce::Rectangle<int> PedalBackground::getButtonArea()
{
    auto pedalArea = getPedalArea();
    auto pedalBottom = pedalArea.removeFromBottom(pedalArea.getHeight() * 0.66f);
    auto buttonLabelArea = juce::Rectangle<int>(pedalBottom.getWidth() * 0.8f,
                                                pedalBottom.getHeight() * 0.8f);
    buttonLabelArea.setCentre(pedalBottom.getCentre());
    auto buttonLabelBottom = buttonLabelArea.removeFromBottom(buttonLabelArea.getHeight() * 0.5f);
    auto buttonArea = juce::Rectangle<int>(buttonLabelBottom.getWidth() * 0.9f,
                                            buttonLabelBottom.getHeight() * 0.85f);
    buttonArea.setCentre(buttonLabelBottom.getCentre());
    buttonArea.setBottom(buttonLabelBottom.getBottom() - (buttonLabelBottom.getHeight() * 0.05f));

    return buttonArea;
}

// =============================================================================
void BypassButton::paintButton(juce::Graphics& g, 
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown)
{
    juce::Colour bgColor;

    if(shouldDrawButtonAsDown)
    {
        bgColor = juce::Colours::grey;
    }
    else
    {
        bgColor = juce::Colour(204, 204, 204);
    }

    getLookAndFeel().drawButtonBackground(g,
                                            *this,
                                            bgColor,
                                            shouldDrawButtonAsHighlighted,
                                            shouldDrawButtonAsDown);
}

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
    bypassButton(),
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

    bypassButton.onClick = [this] { toggleBypass(); };

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
}

void PoopSmearerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    using namespace juce;
    auto bounds = getLocalBounds();

    pedalBackground.setBounds(bounds);

    auto knobArea = pedalBackground.getKnobArea();  
    auto driveArea = pedalBackground.getDriveKnobArea();
    auto levelArea = pedalBackground.getLevelKnobArea();
    auto toneArea = pedalBackground.getToneKnobArea();
    auto buttonArea = pedalBackground.getButtonArea();
    
    bypassButton.setBounds(buttonArea);
    driveSlider.setBounds(driveArea);
    levelSlider.setBounds(levelArea);
    toneSlider.setBounds(toneArea);
}

// =============================================================================
std::vector<juce::Component*> PoopSmearerAudioProcessorEditor::getComps()
{
    return
    {
        &pedalBackground,
        &bypassButton,
        &driveSlider,
        &toneSlider,
        &levelSlider
    };
}

// =============================================================================
void PoopSmearerAudioProcessorEditor::toggleBypass()
{
    auto bypassParam = audioProcessor.apvts.getParameter("Bypass");
    bool bypass = false;

    if(bypassParam->getValue() >= 0.5f)
    {
        bypass = true;
    }

    bypass = !bypass;

    bypassParam->setValueNotifyingHost(bypass);
}