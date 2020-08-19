/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// this is a helper function to create a range with a skew for frequency
juce::NormalisableRange<float> frequencyRange(float min, float max, float interval)
{
    return { min, max, interval, 1.0f / std::log2(1.0f + std::sqrt(max / min)) };
}

//==============================================================================
ChoruspluginAudioProcessor::ChoruspluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this, nullptr, "parameters", createParameterLayout())
{
    // adds a listener for each ID
    for (auto id : parameterIDs)
        parameters.addParameterListener(id, this);

    // set the processCallback for the modulator to change a parameter
    modulator.setProcessCallback([this](const juce::String& id, float value) { 
        parameterChanged(id, value); 
        });
}

ChoruspluginAudioProcessor::~ChoruspluginAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ChoruspluginAudioProcessor::createParameterLayout()
{
    using namespace juce;

    std::vector< std::unique_ptr<RangedAudioParameter> > params;

    // chorus
    params.push_back(std::make_unique<AudioParameterFloat>("00_chorus_rate", "Rate", frequencyRange(0.01f, 20.0f, 0.01f), 2.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("01_chorus_depth", "Depth", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>("02_chorus_mix", "Mix", 0.00f, 1.00f, 1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("03_chorus_delay", "Delay Time", NormalisableRange<float>(0.005f, 0.075f, 0.001f), 0.005f));
    params.push_back(std::make_unique<AudioParameterFloat>("04_chorus_width", "Width", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterChoice>("05_chorus_mode", "Mode", StringArray("Stereo", "Mono", "Dim", "Vib"), 0));
    params.push_back(std::make_unique<AudioParameterChoice>("06_chorus_voices", "Voices", StringArray("2", "4", "6", "8"), 0));
    params.push_back(std::make_unique<AudioParameterFloat>("07_chorus_spread", "Spread", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // lfo
    params.push_back(std::make_unique<AudioParameterBool>("08_lfo_type", "Lfo Type", false));
    params.push_back(std::make_unique<AudioParameterFloat>("09_lfo_phaseL", "Phase Left", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>("10_lfo_phaseR", "Phase Right", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // filters
    params.push_back(std::make_unique<AudioParameterFloat>("11_filter_hipass", "High Pass", frequencyRange(20.0f, 20000.0f, 1.0f), 20.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("12_filter_lopass", "Low Pass", frequencyRange(20.0f, 20000.0f, 1.0f), 20000.0f));
    params.push_back(std::make_unique<AudioParameterBool>("13_filter_bypass", "Filter Bypass", false));

    // modulator
    params.push_back(std::make_unique<AudioParameterChoice>("14_mod_target", "Target", StringArray(
        "00_chorus_rate",
        "01_chorus_depth",
        "02_chorus_mix",
        "03_chorus_delay",
        "04_chorus_width"), 0));
    params.push_back(std::make_unique<AudioParameterBool>("15_mod_type", "Mod Type", false));
    params.push_back(std::make_unique<AudioParameterFloat>("16_mod_rate", "Mod Rate", frequencyRange(0.01f, 10.0f, 0.01f), 2.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("17_mod_depth", "Mod Depth", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // gain
    params.push_back(std::make_unique<AudioParameterFloat>("18_input_gain", "Input", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("19_output_gain", "Output", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    return { params.begin(), params.end() };
}

// callback for when a parameter is changed
void ChoruspluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    using namespace juce;

    auto& chorus = processorChain.get<chorusIndex>();

    switch (parameterIDs.indexOf(parameterID))
    {
        // chorus
    case (0): // rate
        chorus.setRate(newValue);
        break;
    case (1): // depth
        chorus.setDepth(newValue);
        break;
    case (2): // mix
        chorus.setMix(newValue);
        break;
    case (3): // delay
        chorus.setDelayTime(newValue);
        break;
    case (4): // width
        chorus.setDelayWidth(newValue);
        break;
    case (5): // mode
        chorus.setMode(static_cast<dingus::Mode>(newValue));
        break;
    case (6): // voices
        chorus.setNumVoice(static_cast<size_t>(newValue));
        break;
    case (7): // spread
        chorus.setVoiceSpread(newValue);
        break;

        // lfo
    case (8): // type
        chorus.setLfoType(static_cast<dingus::WaveType>(newValue));
        break;
    case (9): // phase left
        chorus.setPhaseOffset(newValue, 0);
        break;
    case (10): // phase right
        chorus.setPhaseOffset(newValue, 1);
        break;

        // filter
    case (11): // high pass
        chorus.setHighPass(newValue);
        break;
    case (12): // low pass
        chorus.setLowPass(newValue);
        break;
    case (13): // bypass
        chorus.setFilterBypass(newValue);
        break;

        // modulator
    case (14): // target
    {
        // keep track of the last value
        String lastID = modulator.getCurrentID();
        std::atomic<float>* lastValue = parameters.getRawParameterValue(lastID);

        // set the new target
        int targetIndex = static_cast<int>(newValue);
        std::atomic<float>* value = parameters.getRawParameterValue(modTargets[targetIndex]);
        NormalisableRange<float> range = parameters.getParameterRange(modTargets[targetIndex]);
        modulator.setTargetParameter(value, range, modTargets[targetIndex]);

        // recursive call to reset the last parameter
        // make sure the last value wasn't nullptr
        if (lastValue)
            parameterChanged(lastID, *parameters.getRawParameterValue(lastID));
    }
    break;
    case (15): // type
        modulator.setLfoType(static_cast<dingus::WaveType>(newValue));
        break;
    case (16): // rate
        modulator.setLfoRate(newValue);
        break;
    case (17): // depth
        modulator.setLfoDepth(newValue);
        break;

        // gain
    case (18): // input
    {
        auto& inputGain = processorChain.get<inputGainIndex>();
        inputGain.setGainLinear(newValue);
    }
    break;
    case (19): // output
    {
        auto& outputGain = processorChain.get<outputGainIndex>();
        outputGain.setGainLinear(newValue);
    }
    break;

    default:
        break;
    }
}

//==============================================================================
const juce::String ChoruspluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChoruspluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChoruspluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChoruspluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChoruspluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChoruspluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChoruspluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChoruspluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChoruspluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChoruspluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChoruspluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    modulator.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    processorChain.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });

    auto& inputGain = processorChain.get<inputGainIndex>();
    auto& outputGain = processorChain.get<outputGainIndex>();
    inputGain.setRampDurationSeconds(0.1);
    outputGain.setRampDurationSeconds(0.1);

    // auto& chorus = processorChain.get<chorusIndex>();
    // setLatencySamples(chorus.getLatency());

    // set initial values for each parameter
    for (auto id : parameterIDs)
    {
        parameterChanged(id, *parameters.getRawParameterValue(id));
    }
}

void ChoruspluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChoruspluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void ChoruspluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    modulator.process(context);
    processorChain.process(context);
}

//==============================================================================
bool ChoruspluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChoruspluginAudioProcessor::createEditor()
{
    //return new ChoruspluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ChoruspluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChoruspluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChoruspluginAudioProcessor();
}
