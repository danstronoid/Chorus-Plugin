/*
  ==============================================================================

    DelayBuffer.h
    Created: 17 Aug 2020 4:59:51pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


namespace dingus 
{

//==============================================================================
/**  
    Circular buffer for a delay line, can be used for integer or fractional delays.
    Use a float or double audio sample type. 
 */
template <typename SampleType>
class DelayBuffer
{
public:
    DelayBuffer()
    {
        clear();
    }

    // increase the size of the buffer
    void resize(size_t size)
    {
        m_data.resize(size);
        m_position = 0;
    }

    // get the current buffer size
    size_t size()
    {
        return m_data.size();
    }

    // clear the buffer and reset position
    void clear()
    {
        std::fill(m_data.begin(), m_data.end(), T(0));
        m_position = 0;
    }

    // takes an integer delay time in samples and returns value
    SampleType get(size_t delayInSamples)
    {
        jassert(delayInSamples >= 0 && delayInSamples < size());

        return m_data[(m_position + delayInSamples + 1) % size()];
    }

    // takes a fractional delay time (in samples) and returns value using linear interpolation
    SampleType getLinear(SampleType delayTime)
    {
        SampleType position = static_cast<SampleType>(std::fmod((m_position + delayTime + T(1)), size()));

        size_t index0 = static_cast<size_t>(position);
        size_t index1 = (index0 + 1) % size();

        SampleType frac = position - static_cast<T>(index0);

        SampleType value0 = m_data[index0];
        SampleType value1 = m_data[index1];

        SampleType output = value0 + frac * (value1 - value0);

        return output;
    }

    // push a new value to the buffer
    void push(SampleType value)
    {
        m_data[m_position] = value;

        if (m_position == 0)
            m_position = size() - 1;
        else
            --m_position;
    }

private:
    size_t m_position{ 0 };
    std::vector<SampleType> m_data;
};
//==============================================================================

} // dingus

