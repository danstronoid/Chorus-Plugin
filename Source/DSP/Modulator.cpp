/*
  ==============================================================================

    Modulator.cpp
    Created: 19 Aug 2020 10:20:05am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "Modulator.h"

namespace dingus
{
//==============================================================================
Modulator::Modulator()
{
}

void Modulator::prepare(const juce::dsp::ProcessSpec& spec)
{
    // initialize the mod lfo
    m_modLfo.prepare(spec);
    m_modLfoDepth.reset(spec.sampleRate, 0.5);
}

float Modulator::getNextValue()
{
    float currentValue = *m_targetValue;
    float lfoValue = m_modLfo.processSample();
    float maxDepth = m_maxValue * 0.5f;
    return juce::jlimit(m_minValue, m_maxValue, currentValue + lfoValue * m_modLfoDepth.getNextValue() * maxDepth);
}

void Modulator::reset()
{
    m_modLfo.reset();
}

//==============================================================================

void Modulator::setProcessCallback(std::function<void(const juce::String&, float)> callback)
{
    m_processCallback = callback;
}

void Modulator::setTargetParameter(std::atomic<float>* value, juce::NormalisableRange<float> range, const juce::String& id)
{
    m_targetID = id;
    m_targetValue = value;
    m_minValue = range.start;
    m_maxValue = range.end;
}

//==============================================================================

void Modulator::setLfoRate(float rate)
{
    m_modLfoRate = rate;
    m_modLfo.setFrequency(rate);
}

void Modulator::setLfoDepth(float depth)
{
    m_modLfoDepth.setTargetValue(depth);
}

void Modulator::setLfoType(WaveType type)
{
    m_modLfo.setType(type);
}

const juce::String& Modulator::getCurrentID() const
{
    return m_targetID;
}

//==============================================================================

} // dingus