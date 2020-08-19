/*
  ==============================================================================

    ChorusVoices.cpp
    Created: 18 Aug 2020 1:49:35pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "ChorusVoices.h"

namespace dingus
{

//==============================================================================
template<typename SampleType>
ChorusVoices<SampleType>::ChorusVoices()
{
    m_voices.resize(m_maxVoices);
}

template<typename SampleType>
ChorusVoices<SampleType>::ChorusVoices(size_t maxVoices) : m_maxVoices(maxVoices)
{
    ChorusVoices();
}

//==============================================================================

template<typename SampleType>
void ChorusVoices<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    for (auto& voice : m_voices)
        voice.prepare(spec);
}

template<typename SampleType>
void ChorusVoices<SampleType>::reset()
{
    for (auto& voice : m_voices)
        voice.reset();
}

//==============================================================================
// set functions

template<typename SampleType>
void ChorusVoices<SampleType>::setDelayTime(SampleType delayTime)
{
    m_delayTime = delayTime;
    updateDelayTime();
}

template<typename SampleType>
void ChorusVoices<SampleType>::setDelayWidth(SampleType width)
{
    m_delayWidth = width;
    updateDelayTime();
}

template<typename SampleType>
void ChorusVoices<SampleType>::setVoiceSpread(SampleType spread)
{
    m_spread = spread;
    updateDelayTime();
}

template<typename SampleType>
void ChorusVoices<SampleType>::setRate(SampleType rate)
{
    for (auto& voice : m_voices)
        voice.setRate(rate);
}

template<typename SampleType>
void ChorusVoices<SampleType>::setDepth(SampleType depth)
{
    for (auto& voice : m_voices)
        voice.setDepth(depth);
}

template<typename SampleType>
void ChorusVoices<SampleType>::setActiveVoices(size_t numVoices)
{
    m_activeVoices = numVoices;
}

template<typename SampleType>
void ChorusVoices<SampleType>::setPhaseOffset(SampleType phaseOffset, size_t channel/* = 0*/)
{
    for (auto& voice : m_voices)
        voice.setPhaseOffset(phaseOffset, channel);
}

template<typename SampleType>
void ChorusVoices<SampleType>::setLfoType(WaveType type)
{
    for (auto& voice : m_voices)
        voice.setLfoType(type);
}

template<typename SampleType>
size_t ChorusVoices<SampleType>::getMaxVoices() const
{
    return m_maxVoices;
}

template<typename SampleType>
size_t ChorusVoices<SampleType>::getActiveVoices() const
{
    return m_activeVoices;
}

template<typename SampleType>
SampleType ChorusVoices<SampleType>::getDelayTime() const
{
    return m_delayTime;
}

//==============================================================================

template<typename SampleType>
void ChorusVoices<SampleType>::updateDelayTime()
{
    SampleType voiceCount = 0;

    for (auto& voice : m_voices)
    {
        // spreads the additional voices between the delay time and a minimum time of 5ms
        // m_delaytime could change while this is updating 
        // need to calculate the entire offset delay time in one line 
        // this prevents using two different values of m_delayTime which could result in a delayTime < 0;
        SampleType voiceOffset = m_delayTime - m_spread * (m_delayTime - SampleType(5e-3)) 
            * (voiceCount / static_cast<SampleType>(m_maxVoices));

        // scales the width down towards 1ms
        SampleType widthOffset = m_delayWidth * (voiceOffset - SampleType(1e-3));

        size_t numChannels = voice.getNumChannels();

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            if (channel % 2 == 1)
                voice.setDelayTime(voiceOffset - widthOffset, channel);
            else
                voice.setDelayTime(voiceOffset, channel);
        }

        ++voiceCount;
    }
}

//==============================================================================

template class ChorusVoices<float>;
template class ChorusVoices<double>;

} // dingus