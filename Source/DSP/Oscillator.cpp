/*
  ==============================================================================

    Oscillator.cpp
    Created: 18 Aug 2020 10:52:00am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "Oscillator.h"

namespace dingus
{
//==============================================================================

template<typename SampleType>
Oscillator<SampleType>::Oscillator()
{
    initTables();
}

template<typename SampleType>
Oscillator<SampleType>::Oscillator(size_t tableSize) : m_tableSize(tableSize)
{
    initTables();
}

//==============================================================================

template<typename SampleType>
void Oscillator<SampleType>::updateDelta()
{
    m_tableDelta = (m_frequency + m_frequency) * m_sizeOverSR;
}

template<typename SampleType>
void Oscillator<SampleType>::initTables()
{
    size_t triIndex = static_cast<size_t>(WaveType::TRI);
    size_t sineIndex = static_cast<size_t>(WaveType::SINE);

    for (auto& table : m_lookupTables)
    {
        table.resize(m_tableSize + 1);
        std::fill(table.begin(), table.end(), SampleType(0));
    }

    // angle ranges from -pi to pi
    SampleType currentAngle = juce::MathConstants<SampleType>::pi * SampleType(-1);
    SampleType angleDelta = juce::MathConstants<SampleType>::twoPi / m_tableSize;

    for (size_t i = 0; i < m_tableSize; ++i)
    {
        m_lookupTables[triIndex][i] = (2 / juce::MathConstants<SampleType>::pi) * std::asin(std::sin(currentAngle));
        m_lookupTables[sineIndex][i] = std::sin(currentAngle);
        currentAngle += angleDelta;
    }

    m_lookupTables[triIndex][m_tableSize] = m_lookupTables[triIndex][0];
    m_lookupTables[sineIndex][m_tableSize] = m_lookupTables[sineIndex][0];
}

//==============================================================================

template<typename SampleType>
void Oscillator<SampleType>::setFrequency(SampleType frequency)
{
    m_frequency = frequency;
    updateDelta();
}

template<typename SampleType>
void Oscillator<SampleType>::setPhaseOffset(SampleType phaseOffset)
{
    m_phaseOffset.setTargetValue(phaseOffset);
}

template<typename SampleType>
void Oscillator<SampleType>::setType(WaveType type)
{
    m_type = type;
}

template<typename SampleType>
SampleType Oscillator<SampleType>::getFrequency()
{
    return m_frequency;
}

//==============================================================================

template<typename SampleType>
void Oscillator<SampleType>::reset()
{
    m_tablePos = 0;
}

template<typename SampleType>
void Oscillator<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    m_tablePos = 0;
    m_sampleRate = static_cast<SampleType>(spec.sampleRate);
    m_sizeOverSR = static_cast<SampleType>(m_tableSize / m_sampleRate);
    updateDelta();
    m_phaseOffset.reset(spec.sampleRate, 0.5);
}

//==============================================================================

template<typename SampleType>
SampleType Oscillator<SampleType>::processSample()
{
    // this offset position is calculated for each sample so that phase can be modulated
    // performance could be increased by making this static or by using a phase multiplier instead
    SampleType offsetPos = std::fmod(
        m_tablePos + m_phaseOffset.getNextValue() * static_cast<SampleType>(m_tableSize),
        static_cast<SampleType>(m_tableSize));

    size_t index0 = static_cast<size_t>(offsetPos);
    size_t index1 = index0 + 1;

    SampleType frac = offsetPos - static_cast<SampleType>(index0);

    size_t type = static_cast<size_t>(m_type);
    SampleType value0 = m_lookupTables[type][index0];
    SampleType value1 = m_lookupTables[type][index1];

    SampleType sample = value0 + frac * (value1 - value0);

    if ((m_tablePos += m_tableDelta) > m_tableSize)
        m_tablePos -= m_tableSize;

    return sample;
}

//==============================================================================

} // dingus