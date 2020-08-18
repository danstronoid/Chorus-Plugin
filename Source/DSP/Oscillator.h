/*
  ==============================================================================

    Oscillator.h
    Created: 18 Aug 2020 9:29:04am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace dingus
{

//==============================================================================
// this enum represents the types available wave tables
enum class WaveType 
{
    TRI,
    SINE,
    MAX
};

//==============================================================================
/**
    This is a simple wavetable oscillator to be used in DSP as a modulation source.
    Use a float or double audio sample type.  Tablesize should be relatively small 
    since this is a modulation osciallor, but can be increased using the provided 
    template parameter.
*/
template<typename SampleType, size_t tableSize = 128>
class Oscillator {
public:
    Oscillator()
    {
        initTables();
    }

    // prepares the oscillator for playback given a ProcessSpec
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        m_tablePos = 0;
        m_sampleRate = static_cast<SampleType>(spec.sampleRate);
        m_sizeOverSR = static_cast<SampleType>(m_tableSize / m_sampleRate);
        updateDelta();
        m_phaseOffset.reset(spec.sampleRate, 0.5);
    }

    // resets the table position
    void reset()
    {
        m_tablePos = 0;
    }

    // calucluate and return the next sample using linear interpolation
    SampleType processSample() noexcept
    {
        // this offset position is calculated for each sample so that phase can be modulated
        // performance could be increased by making this static or by using a phase multiplier instead
        SampleType offsetPos = fmod(
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

    // process the osciallator using a block.  Only processes one channel.
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();

        auto* output = outputBlock.getChannelPointer(0);

        for (size_t i = 0; i < numSamples; ++i)
        {
            output[i] = processSample();
        }
    }

    // set the osc frequency
    void setFrequency(SampleType frequency)
    {
        m_frequency = frequency;
        updateDelta();
    }

    // modifies the phase of the oscillator by offseting the table read position
    void setPhaseOffset(SampleType phaseOffset)
    {
        m_phaseOffset.setTargetValue(phaseOffset);
    }

    // sets the wave type of the oscillator
    void setType(WaveType type)
    {
        m_type = type;
    }

    // returns the current frequency
    SampleType getFrequency()
    {
        return m_frequency;
    }

private:
    SampleType m_sampleRate;

    WaveType m_type{ WaveType::TRI };
    std::array<std::vector<SampleType>, static_cast<size_t>(WaveType::MAX)> m_lookupTables;

    size_t m_tableSize{ tableSize };
    SampleType m_sizeOverSR{};
    SampleType m_tablePos{};
    SampleType m_tableDelta{};
    SmoothedValue<SampleType> m_phaseOffset{};

    SampleType m_frequency{ SampleType(2) };

    // update the table delta
    void updateDelta()
    {
        m_tableDelta = (m_frequency + m_frequency) * m_sizeOverSR;
    }

    // initialize values for the wavetables 
    void initTables()
    {
        size_t triIndex = static_cast<size_t>(WaveType::TRI);
        size_t sineIndex = static_cast<size_t>(WaveType::SINE);

        for (auto& table : m_lookupTables)
        {
            table.resize(m_tableSize + 1);
            std::fill(table.begin(), table.end(), SampleType(0));
        }

        // angle ranges from -pi to pi
        SampleType currentAngle = MathConstants<SampleType>::pi * SampleType(-1);
        SampleType angleDelta = MathConstants<SampleType>::twoPi / m_tableSize;

        for (size_t i = 0; i < m_tableSize; ++i)
        {
            m_lookupTables[triIndex][i] = (2 / MathConstants<SampleType>::pi) * std::asin(std::sin(currentAngle));
            m_lookupTables[sineIndex][i] = std::sin(currentAngle);
            currentAngle += angleDelta;
        }

        m_lookupTables[triIndex][m_tableSize] = m_lookupTables[triIndex][0];
        m_lookupTables[sineIndex][m_tableSize] = m_lookupTables[sineIndex][0];
    }
};
//==============================================================================

} // dingus