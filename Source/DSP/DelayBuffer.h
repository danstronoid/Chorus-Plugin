/*
  ==============================================================================

    DelayBuffer.h
    Created: 17 Aug 2020 4:59:51pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include <cmath>

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
    DelayBuffer();

    // increase the size of the buffer
    void resize(size_t size);

    // get the current buffer size
    size_t size();

    // clear the buffer and reset position
    void clear();

    // takes an integer delay time in samples and returns value
    SampleType get(size_t delayInSamples);

    // takes a fractional delay time (in samples) and returns value using linear interpolation
    SampleType getLinear(SampleType delayTime);

    // push a new value to the buffer
    void push(SampleType value);

private:
    size_t m_position{ 0 };
    std::vector<SampleType> m_data;
};
//==============================================================================

} // dingus

