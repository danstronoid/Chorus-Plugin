/*
  ==============================================================================

    ChorusVoices.h
    Created: 18 Aug 2020 1:49:35pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include "ModDelay.h"

namespace dingus
{

/**
    This class is used to manage all of the voices used by the chorus engine.
    Each voice is a modulated delay line (mono) or a pair of modulated delay lines (stereo).
*/
template<typename SampleType>
class ChorusVoices
{
public:
    ChorusVoices();

    // takes an option to specify the number of voices, default is 4
    // each voice can be stereo, so 4 voices (stereo) = 8 delay lines
    ChorusVoices(size_t maxVoices);

    // prepares each voice for playback
    void prepare(const juce::dsp::ProcessSpec& spec);

    // processes a block of samples using a juce ProcessContext
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        jassert(m_activeVoices > 0 && m_activeVoices <= m_maxVoices);

        size_t currentVoices = m_activeVoices;
        SampleType gainAdjust = SampleType(1) / std::sqrt(static_cast<SampleType>(currentVoices));

        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();
        auto numChannels = outputBlock.getNumChannels();

        for (size_t voice = 0; voice < m_maxVoices; ++voice)
        {
            for (size_t channel = 0; channel < numChannels; ++channel)
            {
                auto* input = inputBlock.getChannelPointer(channel);
                auto* output = outputBlock.getChannelPointer(channel);

                for (size_t i = 0; i < numSamples; ++i)
                {
                    // still need to process buffers for inactive voices
                    SampleType outputSample = m_voices[voice].processSample(input[i], channel);

                    if (voice < currentVoices)
                        output[i] += outputSample * gainAdjust;
                }
            }
        }
    }

    // resets all voices
    void reset();

    //==============================================================================
    // set functions

    // sets the delay time and updates delay times for all voices using updateDelayTime()
    void setDelayTime(SampleType delayTime);

    // scales the delay time of the right channel down towards 1ms
    // setDelayWidth() calls setDelayTime() which does the actual scaling
    void setDelayWidth(SampleType width);

    // spreads the voice delay times
    // calls setDelayTime() to update the spread
    void setVoiceSpread(SampleType spread);

    // set the rate of the lfo in Hz
    void setRate(SampleType rate);

    // set the lfo depth using a value from 0-1
    void setDepth(SampleType depth);

    // sets the number of voices to output
    void setActiveVoices(size_t numVoices);

    // offsets the phase of the left lfo
    void setPhaseOffset(SampleType phaseOffset, size_t channel = 0);

    // sets the wave type of the lfo oscillator
    void setLfoType(WaveType type);

    // returns the max number of voices available
    size_t getMaxVoices() const;

    // returns the active number of voices
    size_t getActiveVoices() const;

    // returns the current delay time
    SampleType getDelayTime() const;

private:
    // the actual voices are held in a vector of modulated delay lines
    std::vector<ModDelay<SampleType>> m_voices;
    size_t m_maxVoices{ 4 };
    size_t m_activeVoices{ 1 };

    // keep track of the minimum delay time
    // delay width scales the minimum time of the right channel
    // spread determines the spread of the voices in time
    SampleType m_delayTime{ SampleType(5e-3) };
    SampleType m_delayWidth{ SampleType(0) };
    SampleType m_spread{ 1 };

    // delayTime, delayWidth, and spread all need to update setDelayTime() for each voice
    void updateDelayTime();
};

//==============================================================================
} // dingus