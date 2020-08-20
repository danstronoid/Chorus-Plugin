/*
  ==============================================================================

    ChorusEngine.h
    Created: 18 Aug 2020 1:50:56pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "ChorusVoices.h"

namespace dingus
{

// modes that select which processing algorithm to use
enum class Mode
{
    STEREO,
    MONO,
    DIMENSION,
    VIBRATO
};

//==============================================================================
// this engine combines the chorus effect(s) with filters and other processing

template<typename SampleType>
class ChorusEngine
{
public:
    ChorusEngine();

    // prepares the chorus engine for playback
    void prepare(const juce::dsp::ProcessSpec& spec);

    // resets entire processorChain and filters
    void reset();

    // processes a block of samples using a juce ProcessContext 
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();
        auto numChannels = outputBlock.getNumChannels();

        auto chorusBlock = tempBlock.getSubBlock(0, numSamples);
        chorusBlock.fill(SampleType(0)).add(context.getInputBlock());

        size_t filterUpdateCounter = m_filterUpdateRate;

        // updates the filterCutoffs according to the updateRate
        for (size_t pos = 0; pos < numSamples;)
        {
            auto blockSize = juce::jmin((size_t)numSamples - pos, filterUpdateCounter);
            auto subBlock = chorusBlock.getSubBlock(pos, blockSize);
            juce::dsp::ProcessContextReplacing<SampleType> tempContext(subBlock);
            processorChain.process(tempContext);

            pos += blockSize;
            filterUpdateCounter -= blockSize;

            if (filterUpdateCounter == 0)
            {
                filterUpdateCounter = m_filterUpdateRate;
                updateFilterCutoffs(static_cast<int>(m_filterUpdateRate));
            }
        }

        // keep the mode from swtiching in the middle of the process block
        Mode currentMode = m_mode;

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* dryIn = inputBlock.getChannelPointer(channel);
            auto* processedInA = chorusBlock.getChannelPointer(channel);
            auto* processedInB = chorusBlock.getChannelPointer((channel + 1) % numChannels);
            auto* output = outputBlock.getChannelPointer(channel);

            auto& boost = m_boostFilters[channel];
            auto& cut = m_cutFilters[channel];

            switch (currentMode)
            {
            case Mode::STEREO:
                for (size_t i = 0; i < numSamples; ++i)
                {
                    //////////////////////// Stereo Chorus /////////////////////////
                    SampleType mixLevel = m_mixLevel[channel].getNextValue();
                    output[i] = dryIn[i] * (1 - mixLevel) + (processedInA[i] - processedInB[i]) * mixLevel;
                }
                break;
            case Mode::MONO:
            {
                SampleType gainAdjust = SampleType(1) / juce::MathConstants<SampleType>::sqrt2;
                for (size_t i = 0; i < numSamples; ++i)
                {
                    //////////////////////// Mono Chorus ///////////////////////////
                    SampleType mixLevel = m_mixLevel[channel].getNextValue();
                    output[i] = dryIn[i] * (1 - mixLevel) + (processedInA[i] + processedInB[i]) * mixLevel * gainAdjust;
                }
            }
            break;
            case Mode::DIMENSION:
                for (size_t i = 0; i < numSamples; ++i)
                {
                    //////////////////////// Dimension Chorus  /////////////////////
                    SampleType mixLevel = m_mixLevel[channel].getNextValue();
                    output[i] = (boost.processSample(dryIn[i])) * (1 - mixLevel) + (processedInA[i] - cut.processSample(processedInB[i])) * mixLevel;
                }
                break;
            case Mode::VIBRATO:
                for (size_t i = 0; i < numSamples; ++i)
                {
                    //////////////////////// Stereo Chorus /////////////////////////
                    SampleType mixLevel = m_mixLevel[channel].getNextValue();
                    output[i] = dryIn[i] * (1 - mixLevel) + (processedInA[i]) * mixLevel;
                }
            default:
                break;
            }
        }
    }

    //==============================================================================
    // private members
private:
    // audio block for processing the delays
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<SampleType> tempBlock;

    SampleType m_sampleRate{};

    // the mix level of wet/dry signal, 1 is 100% wet and 0 is 100% dry
    // need a smoothed value per channel so that getNextValue() returns the same value for each channel
    std::vector<juce::SmoothedValue<SampleType>> m_mixLevel;

    // this enum determines the algorithm used by the chorus engine
    Mode m_mode{ Mode::STEREO };

    enum {
        voicesIndex,
        highPassIndex,
        lowPassIndex
    };

    juce::dsp::ProcessorChain<
        ChorusVoices<SampleType>,
        juce::dsp::StateVariableTPTFilter<SampleType>,
        juce::dsp::StateVariableTPTFilter<SampleType>
    > processorChain;

    // high and low pass filters which can be used to filter the processed signal
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> m_hiPassCutoff{ SampleType(20) };
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> m_lowPassCutoff{ SampleType(20000) };
    const size_t m_filterUpdateRate{ 100 };

    // cut and boost filters for each channel
    // these cannot be part of the processor chain because they're being applied to specific things
    // boost(dry signal) + wet_channelA - cut(wet_channelB)
    std::vector<juce::dsp::IIR::Filter<SampleType>> m_boostFilters;
    std::vector<juce::dsp::IIR::Filter<SampleType>> m_cutFilters;
    SampleType m_crossoverFreq{ SampleType(200) };

    void updateFilterCutoffs(int skip = 0);

public:
    //==============================================================================
    // set functions

    // sets the wet/dry mix level
    void setMix(SampleType mix);

    // sets the high pass cutoff
    void setHighPass(SampleType cutoff);

    // sets the low pass cutoff
    void setLowPass(SampleType cutoff);

    // bypasses both high pass and low pass filters
    void setFilterBypass(bool bypass);

    // set the rate of the lfo in Hz
    void setRate(SampleType rate);

    // set the lfo depth using a value from 0-1
    void setDepth(SampleType depth);

    // sets the delay time
    void setDelayTime(SampleType delayTime);

    // scales the delay time of the right channel down towards 1ms
    // setDelayWidth() calls setDelayTime() which does the actual scaling
    void setDelayWidth(SampleType width);

    // set the amount of spread among the delay times for each voice
    void setVoiceSpread(SampleType spread);

    // sets the processing mode for the chorus engine
    void setMode(Mode mode);

    // bypasses inactive voices
    void setNumVoice(size_t numVoices);

    // offsets the phase of the left lfo
    void setPhaseOffset(SampleType phaseOffset, size_t channel = 0);

    // sets the wave type of the lfo oscillator
    void setLfoType(WaveType type);

    // returns the delay time in samples which determines the latency
    int getLatency() const;
};

//==============================================================================
} // dingus