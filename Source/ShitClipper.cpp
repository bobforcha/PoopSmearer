/*
  ==============================================================================

    ShitClipper.cpp
    Created: 15 Feb 2022 11:06:33pm
    Author:  bob

  ==============================================================================
*/

#include "ShitClipper.h"

ShitClipper::ShitClipper()
{}

ShitClipper::~ShitClipper()
{}

// =============================================================================
void ShitClipper::prepare(juce::dsp::ProcessSpec spec,
                            const double sampleRate,
                            juce::AudioProcessorValueTreeState& apvts)
{
    dryWet.prepare(spec);
    wetChain.prepare(spec);

    //  Get settings
    auto chainSettings = getChainSettings(apvts);

    // Initialize the wet processor chain
    initWetChain(chainSettings, sampleRate);
}

// =============================================================================
void ShitClipper::process(juce::AudioBuffer<float>& buffer,
                            const double sampleRate,
                            juce::AudioProcessorValueTreeState& apvts)
{
    // Get settings
    auto chainSettings = getChainSettings(apvts);

    // Cook variables
    updateWetChain(chainSettings, sampleRate);

    // Get block to process
    juce::dsp::AudioBlock<float> block(buffer);

    auto wetBlock = block.getSingleChannelBlock(0);
    auto dryBlock = wetBlock; // create dry copy of block

    dryWet.pushDrySamples(dryBlock);

    // Create processing context for wet block
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);

    // Process wet block and get output
    wetChain.process(wetContext);
    auto processedWetBlock = wetContext.getOutputBlock();

    // Mix dry and wet blocks
    dryWet.mixWetSamples(processedWetBlock);
}

// =============================================================================
// Initialize main processor chains.
void ShitClipper::initWetChain(const ChainSettings& chainSettings, const double sampleRate)
{
    // Set wet mix proportion
    dryWet.setWetMixProportion(0.5f);

    initClipChain(chainSettings.drive, sampleRate);
    initToneVolChain(chainSettings.tone, chainSettings.level, sampleRate);
}

void ShitClipper::initClipChain(const float drive, const double sampleRate)
{
    setPreGain(drive);
    setWaveShaperFunction();
    setPostGain();
    setClipperLpfFreq(drive, sampleRate);
    setClipperHpfFreq(sampleRate);
}

void ShitClipper::initToneVolChain(const float tone, const float level, const double sampleRate)
{
    setMainLpfFreq(sampleRate);
    setToneHpfFreq(tone, sampleRate);
    setToneLpfFreq(tone, sampleRate);
    setLevelGain(level);
}

// =============================================================================
// Update main processor chains.
void ShitClipper::updateWetChain(const ChainSettings& chainSettings, const double sampleRate)
{
    updateClipChain(chainSettings.drive, sampleRate);
    updateToneVolChain(chainSettings.tone, chainSettings.level, sampleRate);
}

void ShitClipper::updateClipChain(const float drive, const double sampleRate)
{
    setPreGain(drive);
    setClipperLpfFreq(drive, sampleRate);
}

void ShitClipper::updateToneVolChain(const float tone, const float level, const double sampleRate)
{
    setToneHpfFreq(tone, sampleRate);
    setToneLpfFreq(tone, sampleRate);
    setLevelGain(level);
}

// =============================================================================
// Clip chain methods.
void ShitClipper::setPreGain(const float drive)
{
    auto preGainVal = juce::jmap<float>(drive, 0.f, 10.f, 21.f, 41.f);
    clipChain.get<ClipChainPositions::preGain>().setGainDecibels(preGainVal);
}

void ShitClipper::setPostGain()
{
    // set clipper post-gain to fixed -18 dB
    clipChain.get<ClipChainPositions::postGain>().setGainDecibels(-18.f);
}

void ShitClipper::setWaveShaperFunction()
{
    // set clipper waveshaping function
    clipChain.get<ClipChainPositions::clipper>().functionToUse = [] (float x)
    {
        return std::tanh(x);
    };
}

void ShitClipper::setClipperHpfFreq(double sampleRate)
{
    // set clipper HPF at fixed 720 Hz
    auto clipHpfCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        720.f,
        sampleRate,
        1
    );

    *clipChain.get<ClipChainPositions::Hpf>().coefficients = *clipHpfCoefficients[0];
}

void ShitClipper::setClipperLpfFreq(const float drive, const double sampleRate)
{
    // set clipper LPF using Drive param
    auto clipLpfFreq = juce::jmap<float>(10.f - drive, 0.0, 10.f, 5600.f, 20000.f);
    auto clipLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        clipLpfFreq,
        sampleRate,
        1
    );

    *clipChain.get<ClipChainPositions::Lpf>().coefficients = *clipLpfCoefficients[0];
}

// =============================================================================
// Tone - Volume chain methods.
void ShitClipper::setMainLpfFreq(const double sampleRate)
{
    // set the main LPF
    auto mainLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        723.4f,
        sampleRate,
        1
    );

    *toneVolChain.get<ToneVolChainPositions::mainLpf>().coefficients = *mainLpfCoefficients[0];
}

void ShitClipper::setToneHpfFreq(const float tone, const double sampleRate)
{
    // sett the tone HPF with Tone param
    auto toneHpfFreq = juce::jmap<float>(tone, 0.f, 10.f, 20.f, 2066.f);
    auto toneHpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        toneHpfFreq,
        sampleRate,
        1
    );

    *toneVolChain.get<ToneVolChainPositions::toneHpf>().coefficients = *toneHpfCoefficients[0];
}

void ShitClipper::setToneLpfFreq(const float tone, const double sampleRate)
{
    // set the tone LPF with Tone param
    auto toneLpfFreq = juce::jmap<float>(tone, 0.f, 10.f, 723.4f, 3200.f);
    auto toneLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        toneLpfFreq,
        sampleRate,
        1
    );

    *toneVolChain.get<ToneVolChainPositions::toneLpf>().coefficients = *toneLpfCoefficients[0];
}

void ShitClipper::setLevelGain(const float level)
{
    // initialize level gain with Level param
    auto levelGainDb = juce::jmap<float>(level, 0.f, 10.f, -20.f, 20.f);
    toneVolChain.get<ToneVolChainPositions::level>().setGainDecibels(levelGainDb);
}

// =============================================================================
// Create plugin params
static juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Drive",
        "Drive",
        juce::NormalisableRange<float>(
            0.f, 10.f, 0.1f, 1.f),
        5.f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Tone",
        "Tone",
        juce::NormalisableRange<float>(
            0.f, 10.f, 0.1f, 1.f),
        5.f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Level",
        "Level",
        juce::NormalisableRange<float>(
            0.f, 10.f, 0.1f, 1.f),
        5.f
    ));

    return layout;
}

// =============================================================================
// Gett settings frrom plugin.
ChainSettings ShitClipper::getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.drive = apvts.getRawParameterValue("Drive")->load();
    settings.tone = apvts.getRawParameterValue("Tone")->load();
    settings.level = apvts.getRawParameterValue("Level")->load();

    return settings;
}