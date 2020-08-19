/*
  ==============================================================================

    Oscillator.h
    Created: 18 Aug 2020 9:29:04am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>

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
    Use a float or double audio sample type.  Table size should be relatively small 
    since this is a modulation osciallor, but can be set in the constructor. 
    The default table size is 128 samples.
*/
template<typename SampleType>
class Oscillator 
{
public:
    Oscillator();
    
    // table size can be specified optionally
    Oscillator(size_t tableSize);

    // resets the table position
    void reset();

    // set the osc frequency
    void setFrequency(SampleType frequency);

    // modifies the phase of the oscillator by offseting the table read position
    void setPhaseOffset(SampleType phaseOffset);

    // sets the wave type of the oscillator
    void setType(WaveType type);

    // returns the current frequency
    SampleType getFrequency();

    // prepares the oscillator for playback given a ProcessSpec
    void prepare(const juce::dsp::ProcessSpec& spec);

    // calucluate and return the next sample using linear interpolation
    SampleType processSample();

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

private:
    SampleType m_sampleRate;

    WaveType m_type{ WaveType::TRI };
    std::array<std::vector<SampleType>, static_cast<size_t>(WaveType::MAX)> m_lookupTables;

    size_t m_tableSize{ 128 };
    SampleType m_sizeOverSR{};
    SampleType m_tablePos{};
    SampleType m_tableDelta{};
    juce::SmoothedValue<SampleType> m_phaseOffset{};

    SampleType m_frequency{ SampleType(2) };

    // update the table delta
    void updateDelta();

    // initialize values for the wavetables 
    void initTables();
};
//==============================================================================

} // dingus