/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/ChorusEngine.h"
#include "DSP/Modulator.h"

//==============================================================================
/**
*/
class ChoruspluginAudioProcessor  : public juce::AudioProcessor,
                                    public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ChoruspluginAudioProcessor();
    ~ChoruspluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // returns if the processor supports double precision
    bool supportsDoublePrecisionProcessing() const override;

    // override for float precision
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // override for double precision
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

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

    // callback for when a parameter is changed, inherited from vts listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

    const juce::StringArray parameterIDs
    {
        "00_chorus_rate",
        "01_chorus_depth",
        "02_chorus_mix",
        "03_chorus_delay",
        "04_chorus_width",
        "05_chorus_mode",
        "06_chorus_voices",
        "07_chorus_spread",
        "08_lfo_type",
        "09_lfo_phaseL",
        "10_lfo_phaseR",
        "11_filter_hipass",
        "12_filter_lopass",
        "13_filter_bypass",
        "14_mod_target",
        "15_mod_type",
        "16_mod_rate",
        "17_mod_depth",
        "18_input_gain",
        "19_output_gain"
    };

    const juce::StringArray modTargets
    {
        "00_chorus_rate",
        "01_chorus_depth",
        "02_chorus_mix",
        "03_chorus_delay",
        "04_chorus_width"
    };

    dingus::Modulator modulator;

    enum
    {
        inputGainIndex,
        chorusIndex,
        outputGainIndex
    };

    // template declaration of processor chain
    template <typename SampleType>
    using ProcessorChain = juce::dsp::ProcessorChain<
        juce::dsp::Gain<SampleType>,
        dingus::ChorusEngine<SampleType>,
        juce::dsp::Gain<SampleType> >;

    // private process function to call in overloaded processBlocks for float & double
    template <typename SampleType>
    void process(juce::AudioBuffer<SampleType>& buffer, ProcessorChain<SampleType>& chain);

    // private update function which can be called on either version of the chain in parameterChanged()
    template <typename SampleType>
    void updateParameters(const juce::String& parameterID, SampleType newValue, ProcessorChain<SampleType>& chain);

    ProcessorChain<float> floatChain;
    ProcessorChain<double> doubleChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoruspluginAudioProcessor)
};
