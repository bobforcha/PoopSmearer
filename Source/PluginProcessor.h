/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
struct ChainSettings
{
    float drive { 0 }, tone { 0 }, level { 0 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

class PoopSmearerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PoopSmearerAudioProcessor();
    ~PoopSmearerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

    juce::AudioProcessorValueTreeState apvts {
        *this,
        nullptr,
        "Parameters",
        createParameterLayout()
    };

private:
    //==============================================================================
    // aliases
    using DryWet = juce::dsp::DryWetMixer<float>;
    using Gain = juce::dsp::Gain<float>;
    using Filter = juce::dsp::IIR::Filter<float>;
    using WaveShaper = juce::dsp::WaveShaper<float>;

    using ClipChain = juce::dsp::ProcessorChain<Gain, WaveShaper, Gain, Filter, Filter>;
    using ToneVolChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Gain>;
    using WetChain = juce::dsp::ProcessorChain<ClipChain, ToneVolChain>;
    
    // signal splitter
    DryWet dryWet;
    
    // clipper components
    // juce::dsp::Gain<float> preGain;
    // juce::dsp::WaveShaper<float> clipper;
    // juce::dsp::Gain<float> postGain;
    // juce::dsp::IIR::Filter<float> clipHpf;
    // juce::dsp::IIR::Filter<float> clipLpf;

    // main LPF
    // Filter mainLpf;

    // tone filters
    // Filter toneLpf;
    // Filter toneHpf;

    // level gain
    // Gain level;

    WetChain wetChain;

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
        clipChain,
        toneVolChain
    };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PoopSmearerAudioProcessor)
};
