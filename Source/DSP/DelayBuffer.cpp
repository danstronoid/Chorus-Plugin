/*
  ==============================================================================

    DelayBuffer.cpp
    Created: 18 Aug 2020 10:52:16am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "DelayBuffer.h"

namespace dingus
{

//==============================================================================
template <typename SampleType>
DelayBuffer<SampleType>::DelayBuffer()
{
    clear();
}

template <typename SampleType>
void DelayBuffer<SampleType>::resize(size_t size)
{
    m_data.resize(size);
    m_position = 0;
}

template <typename SampleType>
size_t DelayBuffer<SampleType>::size()
{
    return m_data.size();
}

template <typename SampleType>
void DelayBuffer<SampleType>::clear()
{
    std::fill(m_data.begin(), m_data.end(), SampleType(0));
    m_position = 0;
}

//==============================================================================

template <typename SampleType>
SampleType DelayBuffer<SampleType>::get(size_t delayInSamples)
{
    jassert(delayInSamples >= 0 && delayInSamples < size());

    return m_data[(m_position + delayInSamples + 1) % size()];
}

template <typename SampleType>
SampleType DelayBuffer<SampleType>::getLinear(SampleType delayTime)
{
    SampleType position = static_cast<SampleType>(std::fmod((m_position + delayTime + SampleType(1)), size()));

    size_t index0 = static_cast<size_t>(position);
    size_t index1 = (index0 + 1) % size();

    SampleType frac = position - static_cast<SampleType>(index0);

    SampleType value0 = m_data[index0];
    SampleType value1 = m_data[index1];

    SampleType output = value0 + frac * (value1 - value0);

    return output;
}

template <typename SampleType>
void DelayBuffer<SampleType>::push(SampleType value)
{
    m_data[m_position] = value;

    if (m_position == 0)
        m_position = size() - 1;
    else
        --m_position;
}

//==============================================================================

template class DelayBuffer<float>;
template class DelayBuffer<double>;

} // dingus