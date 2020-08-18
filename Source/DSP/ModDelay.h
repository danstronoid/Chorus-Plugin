/*
  ==============================================================================

    ModDelay.h
    Created: 18 Aug 2020 10:54:04am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"
#include "DelayBuffer.h"

namespace dingus
{

//==============================================================================
/**
    This class is used for a single modulating delay line.
    This was designed with a chorus effect in mind. 
    Use a float or double audio sample type.  
*/
template <typename SampleType>
class ModDelay
{
public:
    ModDelay()
    {
    }

    // prepares the delay for playback given a ProcessSpec
    void prepare(const juce::dsp::ProcessSpec& spec);

    // processes a single sample
    SampleType processSample(SampleType input, size_t channel);

    // processes a block of samples using a juce ProcessContext
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();
        auto numChannels = outputBlock.getNumChannels();

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* input = inputBlock.getChannelPointer(channel);
            auto* output = outputBlock.getChannelPointer(channel);
            auto& delayBuffer = m_delayBuffers[channel];
            auto& lfo = m_lfos[channel];

            for (size_t i = 0; i < numSamples; ++i)
            {
                SampleType inputSample = input[i];
                SampleType outputSample = processSample(inputSample, channel);
                output[i] = context.isBypassed ? inputSample : outputSample;
            }
        }
    }

    // clears the delay buffers and resets the osc position
    void reset();

    //==============================================================================
    // Set methods

    void setWetLevel(SampleType wetLevel);

    void setMaxDelayTime(SampleType maxDelay);

    // the target delay is the center point 
    // around which the delay is modulated
    void setDelayTime(SampleType delayTime, size_t channel = 0, bool force = false);

    // offsets the phase of the lfo for a given channel
    void setPhaseOffset(SampleType phaseOffset, size_t channel = 0);

    // sets the wave type of the lfo oscillator, sets lfo2 to the same type
    void setLfoType(WaveType type);

    // the rate of modulation
    void setRate(SampleType rate);

    // value from 0-1 multiplied by the maxDepth to produce the mod depth in sec
    void setDepth(SampleType depth);

    // sets the maximum depth in sec by which the delay time is modulated
    void setMaxDepth(SampleType maxDepth);

    // returns the targetDelay * sampleRate which will determine the plug in latency
    // useful for mod effects that require delay compensation
    int getLatency();

    //==============================================================================

private:
    SampleType m_sampleRate{};
    SampleType m_wetLevel{ SampleType(1) };

    // delay buffers for each channel
    std::vector<DelayBuffer<SampleType>> m_delayBuffers;
    std::vector<juce::SmoothedValue<SampleType>> m_delayTimes;
    SampleType m_maxDelayTime{ SampleType(1) };

    // lfos
    std::vector<Oscillator<SampleType>> m_lfos;
    SampleType m_lfoRate{ SampleType(2) };
    // need a smoothed value per channel so that getNextValue() returns the same value for each channel
    std::vector<juce::SmoothedValue<SampleType>> m_lfoDepth;
    // max depth is the maximum delay value to modulate
    SampleType m_maxDepth{ SampleType(1e-3) };

    void updateDelayBufferSize();

};
//==============================================================================

} // dingus