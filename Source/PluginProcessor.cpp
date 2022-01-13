/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PoopSmearerAudioProcessor::PoopSmearerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

PoopSmearerAudioProcessor::~PoopSmearerAudioProcessor()
{
}

//==============================================================================
const juce::String PoopSmearerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PoopSmearerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PoopSmearerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PoopSmearerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PoopSmearerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PoopSmearerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PoopSmearerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PoopSmearerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PoopSmearerAudioProcessor::getProgramName (int index)
{
    return {};
}

void PoopSmearerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PoopSmearerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    dryWet.prepare(spec);
    preGain.prepare(spec);
    clipper.prepare(spec);
    postGain.prepare(spec);
    clipHpf.prepare(spec);
    clipLpf.prepare(spec);
    mainLpf.prepare(spec);
    toneLpf.prepare(spec);
    level.prepare(spec);

    //  Get settings
    auto chainSettings = getChainSettings(apvts);

    // Set wet mix proportion
    dryWet.setWetMixProportion(0.5f);

    // set clipper pre-gain with Drive param
    auto preGainVal = juce::jmap<float>(chainSettings.drive, 0.f, 10.f, 21.f, 41.f);
    preGain.setGainDecibels(preGainVal);

    clipper.functionToUse = [] (float x)
    {
        return std::tanh(x);
    };

    // set clipper post-gain to fixed -20 dB
    postGain.setGainDecibels(-20.f);

    // initialize clipper HPF at 720 Hz
    auto clipHpfCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        720.f,
        sampleRate,
        1
    );

    clipHpf.coefficients = *clipHpfCoefficients[0];

    // initialize the clipper LPF using Drive param
    auto clipLpfFreq = juce::jmap<float>(10.f - chainSettings.drive, 0.0, 10.f, 5600.f, 20000.f);
    auto clipLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        clipLpfFreq,
        sampleRate,
        1
    );

    clipLpf.coefficients = *clipLpfCoefficients[0];

    // initialize the main LPF
    auto mainLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        723.4f,
        sampleRate,
        1
    );

    mainLpf.coefficients = *mainLpfCoefficients[0];

    // initialize the tone LPF with Tone param
    auto toneLpfFreq = juce::jmap<float>(chainSettings.tone, 0.f, 10.f, 723.4f, 3200.f);
    auto toneLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        toneLpfFreq,
        sampleRate,
        1
    );

    // initialize level gain with Level param
    auto levelGainDb = juce::jmap<float>(chainSettings.level, 0.f, 10.f, -20.f, 20.f);
    level.setGainDecibels(levelGainDb);
}

void PoopSmearerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PoopSmearerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PoopSmearerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer (channel);

    //     // ..do something to the data...
    // }

    // Get settings
    auto chainSettings = getChainSettings(apvts);

    // Cook variables

    // set clipper pre-gain with Drive param
    // printf("drive from settings: %f\n", chainSettings.drive);
    auto preGainVal = juce::jmap<float>(chainSettings.drive, 0.f, 10.f, 21.f, 41.f);
    // printf("preGainVal: %f\n", preGainVal);
    preGain.setGainDecibels(preGainVal);

    // set clipper LPF frequency using Drive param
    auto clipLpfFreq = juce::jmap<float>(10.f - chainSettings.drive, 0.f, 10.f, 5600.f, 20000.f);
    // printf("cutoff freq: %f\n", clipLpfFreq);
    auto clipLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        clipLpfFreq,
        getSampleRate(),
        1
    );

    clipLpf.coefficients = *clipLpfCoefficients[0];

    // set tone LPF with Tone param
    auto toneLpfFreq = juce::jmap<float>(chainSettings.tone, 0.f, 10.f, 723.4f, 3200.f);
    auto toneLpfCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        toneLpfFreq,
        getSampleRate(),
        1
    );

    // set level gain with Level param
    auto levelGainDb = juce::jmap<float>(chainSettings.level, 0.f, 10.f, -20.f, 20.f);
    level.setGainDecibels(levelGainDb);

    // Get block to process
    juce::dsp::AudioBlock<float> block(buffer);

    auto monoBlock = block.getSingleChannelBlock(0);
    auto dryBlock = monoBlock; // create dry copy of block

    dryWet.pushDrySamples(dryBlock);

    // Create processing context for wet block
    juce::dsp::ProcessContextReplacing<float> monoContext(monoBlock);

    // Process wet block and get output
    preGain.process(monoContext);
    clipper.process(monoContext);
    postGain.process(monoContext);
    clipHpf.process(monoContext);
    clipLpf.process(monoContext);
    mainLpf.process(monoContext);
    toneLpf.process(monoContext);
    level.process(monoContext);

    auto wetBlock = monoContext.getOutputBlock();

    // Mix dry and wet blocks
    dryWet.mixWetSamples(wetBlock);
}

//==============================================================================
bool PoopSmearerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PoopSmearerAudioProcessor::createEditor()
{
    // return new PoopSmearerAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PoopSmearerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PoopSmearerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.drive = apvts.getRawParameterValue("Drive")->load();
    settings.tone = apvts.getRawParameterValue("Tone")->load();
    settings.level = apvts.getRawParameterValue("Level")->load();

    return settings; 
}

juce::AudioProcessorValueTreeState::ParameterLayout
    PoopSmearerAudioProcessor::createParameterLayout()
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
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PoopSmearerAudioProcessor();
}
