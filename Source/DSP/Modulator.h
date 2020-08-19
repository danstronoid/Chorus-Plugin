/*
  ==============================================================================

    Modulator.h
    Created: 18 Aug 2020 3:58:15pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>
#include <atomic>
#include "Oscillator.h"

namespace dingus
{

//==============================================================================
/**
    This modulator class is used to modulate a ranged audio parameter with a
    lfo signal.  This is done by passing a processCallback function to the modulator
    class which then calls the processCallback for every sample in the process block.
    The processCallback function should be the parameterChanged function associated with 
    a parameter listener.  It is important that this function and a valid parameter is
    passed to the modulator before calling the process method. Then in the main processBlock, 
    modulator.process has to be called before any other processing to ensure the parameter 
    changes will be applied to that block.
*/
class Modulator
{
public:
    Modulator();

    // prepares the modulator for playback
    void prepare(const juce::dsp::ProcessSpec& spec);

    // calculates the next value using by applying the lfo signal to the target value
    // the value is hard limited by the range of the provided parameter
    float getNextValue();

    // the process callback must be set before calling process
    // this calls the processCallback for each sample in the provided context
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        jassert(m_targetValue && m_processCallback);

        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();

        for (size_t i = 0; i < numSamples; ++i)
        {
            m_processCallback(m_targetID, getNextValue());
        }
    }

    // resets the lfo position
    void reset();

    //==============================================================================

    // this must be set before process is called
    // sets the function that will be called in the process block
    void setProcessCallback(std::function<void(const juce::String&, float)> callback);

    // this must be set before process is called, otherwise this could produce undefined behaviour 
    // this takes the target parameter's raw pointer value, range, and id
    void setTargetParameter(std::atomic<float>* value, juce::NormalisableRange<float> range, const juce::String& id);

    //==============================================================================

    // sets the rate of the lfo
    void setLfoRate(float rate);

    // sets the depth of the lfo
    void setLfoDepth(float depth);

    // sets the lfo wave type
    void setLfoType(WaveType type);

    // returns the ID of the current target parameter
    const juce::String& getCurrentID() const;

    //==============================================================================

private:
    // lfo for modulation
    Oscillator<float> m_modLfo;
    float m_modLfoRate{ 2.0f };
    juce::SmoothedValue<float> m_modLfoDepth{ 0.0f };

    juce::String m_targetID{ "" };
    std::atomic<float>* m_targetValue{ nullptr };
    float m_minValue{};
    float m_maxValue{};

    std::function<void(const juce::String&, float)> m_processCallback{};
};

//==============================================================================

} // dingus