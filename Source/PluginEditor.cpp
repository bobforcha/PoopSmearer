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
    background = drawBackgroundImage();
    setImage(background);
}

juce::Image PedalBackground::drawBackgroundImage()
{
    using namespace juce;

    auto bgImg = Image(Image::PixelFormat::RGB, 300, 475, false);
    Graphics g(bgImg);
    g.fillAll(Colours::black);

    // draw main pedal body
    auto bounds = bgImg.getBounds();
    float scaleFactor = 0.8f;
    auto pedalArea = Rectangle<float>(bounds.getWidth() * scaleFactor,
                                    bounds.getHeight() * scaleFactor);
    pedalArea.setCentre(bounds.getCentreX(), bounds.getCentreY());
    g.setColour(Colours::green);
    g.fillRoundedRectangle(pedalArea, 15.f);

    // draw label and button area
    auto pedalBottomArea = pedalArea.removeFromBottom(pedalArea.getHeight() * 0.66f);
    auto buttonLabelArea = Rectangle<float>(pedalBottomArea.getWidth() * scaleFactor,
                                            pedalBottomArea.getHeight() * scaleFactor);
    buttonLabelArea.setCentre(pedalBottomArea.getCentreX(),
                                pedalBottomArea.getCentreY());
    
    g.setColour(Colours::black);
    g.fillRoundedRectangle(buttonLabelArea, 5.f);

    // draw label
    auto buttonLabelTop = buttonLabelArea.removeFromTop(buttonLabelArea.getHeight() * 0.5f);
    auto pedalLabelArea = Rectangle<float>(buttonLabelTop.getWidth() * 0.9f,
                                            buttonLabelTop.getHeight() * 0.9f);
    pedalLabelArea.setCentre(buttonLabelTop.getCentre());
    g.setColour(Colours::silver);
    g.fillRoundedRectangle(pedalLabelArea, 4.5f);

    g.setColour (Colours::blue);
    g.setFont (22.0f);
    g.drawFittedText ("Poop Smearer\nPS9", pedalLabelArea.toNearestInt(), Justification::centred, 1);

    // draw bottom company tag
    auto tagArea = pedalBottomArea.removeFromBottom(pedalBottomArea.getHeight() * 0.1f);
    g.setColour(Colours::black);
    g.setFont(16.0f);
    g.drawFittedText("Bob's Plugin Bargain Bin", tagArea.toNearestInt(), Justification::centred, 1);

    return bgImg;
}

// =============================================================================
void BypassButton::paintButton(juce::Graphics& g, 
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown)
{
    if(shouldDrawButtonAsDown)
    {
        getLookAndFeel().drawButtonBackground(g, *this, juce::Colours::grey, false, true);
    }  
    else {
        getLookAndFeel().drawButtonBackground(g, *this, juce::Colours::silver, false, false);
    }
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
    bypassButton(*audioProcessor.apvts.getParameter("Bypass")),
    driveSlider(*audioProcessor.apvts.getParameter("Drive")),
    toneSlider(*audioProcessor.apvts.getParameter("Tone")),
    levelSlider(*audioProcessor.apvts.getParameter("Level")),
    bypassButtonAttachment(audioProcessor.apvts, "Bypass", bypassButton),
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

    auto buttonLabelArea = Rectangle<float>(pedalBounds.getWidth() * scaleFactor,
                                            pedalBounds.getHeight() * scaleFactor);
    buttonLabelArea.setCentre(pedalBounds.getCentre());

    auto buttonLabelBottom = buttonLabelArea.removeFromBottom(buttonLabelArea.getHeight() * 0.5);

    auto buttonArea = Rectangle<float>(buttonLabelBottom.getWidth() * 0.9,
                                        buttonLabelBottom.getHeight() * 0.9);
    
    buttonArea.setCentre(buttonLabelBottom.getCentre());

    pedalBackground.setBounds(bounds);
    bypassButton.setBounds(buttonArea.toNearestInt());
    driveSlider.setBounds(driveArea.toNearestInt());
    levelSlider.setBounds(levelArea.toNearestInt());
    toneSlider.setBounds(knobArea.toNearestInt());
}

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
