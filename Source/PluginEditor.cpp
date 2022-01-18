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

    g.setColour(Colours::silver);
    g.fillEllipse(bounds);

    g.setColour(Colours::black);
    g.drawEllipse(bounds, 4.f);

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
void RotarySliderWithLabel::paint(juce::Graphics &g)
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

juce::Rectangle<int> RotarySliderWithLabel::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    float scaleFactor = 0.8f;
    auto pedalBounds = juce::Rectangle<int>(bounds.getWidth() * scaleFactor,
                                            bounds.getHeight() * scaleFactor);
    pedalBounds.setCentre(bounds.getCentre());

    return pedalBounds;
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
    // auto center = bounds.getCentre();
    // auto width = bounds.getWidth();
    // auto height = bounds.getHeight();
    float scaleFactor = 0.8f;

    using namespace juce;
    auto pedalBounds = Rectangle<float>(bounds.getWidth() * scaleFactor,
                                        bounds.getHeight() * scaleFactor);
    pedalBounds.setCentre(bounds.getCentreX(), bounds.getCentreY());

    auto knobArea = pedalBounds.removeFromTop(pedalBounds.getHeight() * 0.33f);
    
    auto driveArea = knobArea.removeFromLeft(knobArea.getWidth() * 0.33f);
    auto driveKnobArea = driveArea.removeFromTop(driveArea.getHeight() * 0.5f);
    Rectangle<int> driveKnobBounds = Rectangle<int>(driveKnobArea.getWidth() * 0.75f,
                                                    driveKnobArea.getWidth() * 0.75f);
    driveKnobBounds.setCentre(driveKnobArea.getCentreX(), driveKnobArea.getCentreY());
    auto driveLabelArea = driveKnobArea.removeFromTop(driveSlider.getTextHeight() * 1.8f);

    auto levelArea = knobArea.removeFromRight(knobArea.getWidth() * 0.5f);
    auto levelKnobArea = levelArea.removeFromTop(levelArea.getHeight() * 0.5f);
    Rectangle<int> levelKnobBounds = Rectangle<int>(levelKnobArea.getWidth() * 0.75f,
                                                    levelKnobArea.getWidth() * 0.75f);
    levelKnobBounds.setCentre(levelKnobArea.getCentreX(), levelKnobArea.getCentreY());

    auto toneKnobArea = knobArea.removeFromBottom(knobArea.getHeight() * 0.5f);
    Rectangle<int> toneKnobBounds = Rectangle<int>(toneKnobArea.getWidth() * 0.75f,
                                                    toneKnobArea.getWidth() * 0.75f);
    toneKnobBounds.setCentre(toneKnobArea.getCentreX(), toneKnobArea.getCentreY());

    driveSlider.setBounds(driveKnobBounds);
    levelSlider.setBounds(levelKnobBounds);
    toneSlider.setBounds(toneKnobBounds);
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
