/*
  ==============================================================================

    ModDelay.cpp
    Created: 18 Aug 2020 10:54:04am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "ModDelay.h"

namespace dingus
{

//==============================================================================

template <typename SampleType>
ModDelay<SampleType>::ModDelay()
{
}

template <typename SampleType>
void ModDelay<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);

    // need to resize to number of channels
    m_delayBuffers.resize(spec.numChannels);
    m_delayTimes.resize(spec.numChannels);
    m_lfos.resize(spec.numChannels);
    m_lfoDepth.resize(spec.numChannels);

    m_sampleRate = static_cast<SampleType>(spec.sampleRate);
    updateDelayBufferSize();

    // call prepare on all lfos
    for (auto& lfo : m_lfos)
    {
        lfo.prepare(spec);
    }

    // set ramped values
    for (auto& lfoDepth : m_lfoDepth)
        lfoDepth.reset(spec.sampleRate, 0.2);

    for (auto& delayTime : m_delayTimes)
        delayTime.reset(spec.sampleRate, 0.5);
}

template <typename SampleType>
SampleType ModDelay<SampleType>::processSample(SampleType input, size_t channel)
{
    // calculates lfo multiplied by depth for the delay offset in secs
    // transforms lfo value from range(-1, 1) to range(0, 1)
    SampleType lfoValue = (m_lfos[channel].processSample() + SampleType(2)) * SampleType(5e-1) * m_lfoDepth[channel].getNextValue();

    SampleType delayTime = (m_delayTimes[channel].getNextValue() + lfoValue) * m_sampleRate;
    SampleType delayedSample = m_delayBuffers[channel].getLinear(delayTime);

    m_delayBuffers[channel].push(input);

    SampleType outputSample = input * (1 - m_wetLevel) + delayedSample * m_wetLevel;
    return outputSample;
}

template <typename SampleType>
void ModDelay<SampleType>::reset()
{
    for (auto& buffer : m_delayBuffers)
        buffer.clear();

    for (auto& lfo : m_lfos)
        lfo.reset();
}

//==============================================================================

template <typename SampleType>
void ModDelay<SampleType>::setWetLevel(SampleType wetLevel)
{
    m_wetLevel = wetLevel;
}

template <typename SampleType>
void ModDelay<SampleType>::setMaxDelayTime(SampleType maxDelay)
{
    // this is more flexible then it needs to be
    // make sure the max delay time is a reasonable number
    // this will impact the buffer size allocated
    jassert(maxDelay >= SampleType(0) && maxDelay < SampleType(10))
        m_maxDelayTime = maxDelay;

    updateDelayBufferSize();
}

template <typename SampleType>
void ModDelay<SampleType>::setDelayTime(SampleType delayTime, size_t channel = 0, bool force = false)
{
    jassert(delayTime > SampleType(0) && delayTime < (m_maxDelayTime - m_maxDepth));

    if (force)
        m_delayTimes[channel].setCurrentAndTargetValue(delayTime);
    else
        m_delayTimes[channel].setTargetValue(delayTime);
}

template <typename SampleType>
void ModDelay<SampleType>::setPhaseOffset(SampleType phaseOffset, size_t channel = 0)
{
    m_lfos[channel].setPhaseOffset(phaseOffset);
}

template <typename SampleType>
void ModDelay<SampleType>::setLfoType(WaveType type)
{
    for (auto& lfo : m_lfos)
        lfo.setType(type);
}

template <typename SampleType>
void ModDelay<SampleType>::setRate(SampleType rate)
{
    jassert(rate >= SampleType(0));
    m_lfoRate = rate;
    for (auto& lfo : m_lfos)
        lfo.setFrequency(m_lfoRate);
}

template <typename SampleType>
void ModDelay<SampleType>::setDepth(SampleType depth)
{
    for (auto& lfoDepth : m_lfoDepth)
        lfoDepth.setTargetValue(depth * m_maxDepth);
}

template <typename SampleType>
void ModDelay<SampleType>::setMaxDepth(SampleType maxDepth)
{
    m_maxDepth = maxDepth;
}

template <typename SampleType>
int ModDelay<SampleType>::getLatency()
{
    return juce::roundToInt(m_delayTimes[0].getCurrentValue() * m_sampleRate);
}

//==============================================================================

template <typename SampleType>
void ModDelay<SampleType>::updateDelayBufferSize()
{
    size_t bufferSize = static_cast<size_t>(std::ceil(m_maxDelayTime * m_sampleRate));

    for (auto& buffer : m_delayBuffers)
        buffer.resize(bufferSize);
}

//==============================================================================
} // dingus