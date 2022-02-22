/*
  ==============================================================================

    ShitClipper.h
    Created: 15 Feb 2022 11:06:33pm
    Author:  bob

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float drive { 0 }, tone { 0 }, level { 0 };
};
class ShitClipper
{
public:
    // =============================================================================
    ShitClipper();
    ~ShitClipper();

    // Main methods to be called in plugin prepareToPlay() and processBlock()
    // methods.
    void prepare(juce::dsp::ProcessSpec spec,
                    const double sampleRate,
                    juce::AudioProcessorValueTreeState& apvts);
    void process(juce::AudioBuffer<float>& buffer,
                    const double sampleRate,
                    juce::AudioProcessorValueTreeState& apvts);

    // Inittialize main processor chains.
    void initWetChain(const ChainSettings& chainSettings, const double sampleRate);
    void initClipChain(const float drive, const double sampleRate);
    void initToneVolChain(const float tone, const float level, const double sampleRate);
    
    // Update main processor chains.
    void updateWetChain(const ChainSettings& chainSettings, const double sampleRate);
    void updateClipChain(const float drive, const double sampleRate);
    void updateToneVolChain(const float tone, const float level, const double sampleRate);

    // Clip chain methods.
    void setPreGain(const float drive);
    void setPostGain();
    void setWaveShaperFunction();
    void setClipperHpfFreq(const double sampleRate);
    void setClipperLpfFreq(const float drive, const double sampleRate);

    // Tone - Volume chain methods.
    void setMainLpfFreq(const double sampleRate);
    void setToneHpfFreq(const float tone, const double sampleRate);
    void setToneLpfFreq(const float tone, const double sampleRate);
    void setLevelGain(const float level);

    // Parameter setup to be used when creating APVTS in plugin.
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

    // Get settings from APVTS
    ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

    // aliases
    using DryWet = juce::dsp::DryWetMixer<float>;
    using Gain = juce::dsp::Gain<float>;
    using Filter = juce::dsp::IIR::Filter<float>;
    using WaveShaper = juce::dsp::WaveShaper<float>;

    using ClipChain = juce::dsp::ProcessorChain<Gain, WaveShaper, Gain, Filter, Filter>;
    using ToneVolChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Gain>;
    using WetChain = juce::dsp::ProcessorChain<ClipChain, ToneVolChain>;

private:
    //==============================================================================
    // signal splitter
    DryWet dryWet;

    // Main processor chain
    WetChain wetChain;

    // chain position enums - define processor chain order
    enum ClipChainPositions
    {
        preGain,
        clipper,
        postGain,
        Hpf,
        Lpf
    };

    enum ToneVolChainPositions
    {
        mainLpf,
        toneLpf,
        toneHpf,
        level
    };

    enum WetChainPositions
    {
        ClipChainIndex,
        ToneVolChainIndex
    };

    // Processor sub-chains
    ClipChain& clipChain = wetChain.get<WetChainPositions::ClipChainIndex>();
    ToneVolChain& toneVolChain = wetChain.get<WetChainPositions::ToneVolChainIndex>();
};