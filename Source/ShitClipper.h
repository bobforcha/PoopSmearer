/*
  ==============================================================================

    ShitClipper.h
    Created: 15 Feb 2022 11:06:33pm
    Author:  bob

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ShitClipper
{
public:
    // =============================================================================
    ShitClipper();
    ~ShitClipper();

    // Main methods to be called in plugin prepareToPlay() and processBlock()
    // methods.
    void prepare(juce::dsp::ProcessSpec spec);
    void process();

    void initProcessors();
    void update();

    // Parameter setup
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

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