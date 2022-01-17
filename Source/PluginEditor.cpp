/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPosProportional,
                                    float rotaryStartAngle,
                                    float rotaryEndAngle,
                                    juce::Slider&)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    g.setColour(Colours::black);
    g.fillEllipse(bounds);

    g.setColour(Colours::silver);
    g.drawEllipse(bounds, 2.f);

    auto center = bounds.getCentre();

    Path p;

    Rectangle<float> r;

    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);
    r.setTop(bounds.getY());
    r.setBottom(center.getY());

    p.addRectangle(r);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.fillPath(p);
}

// =============================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
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
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
  return getLocalBounds();
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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Poop Smearer PS9", getLocalBounds(), juce::Justification::centred, 1);
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
