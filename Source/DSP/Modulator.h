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

class Modulator
{
public:
    Modulator()
    {
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        // initialize the mod lfo
        m_modLfo.prepare(spec);
        m_modLfoDepth.reset(spec.sampleRate, 0.5);
    }

    float getNextValue() noexcept
    {
        float currentValue = *m_targetValue;
        float lfoValue = m_modLfo.processSample();
        float maxDepth = m_maxValue * 0.5f;
        return juce::jlimit(m_minValue, m_maxValue, currentValue + lfoValue * m_modLfoDepth.getNextValue() * maxDepth);
    }

    // the process callback must be set before calling process
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

    void reset() noexcept
    {
        m_modLfo.reset();
    }

    // this must be set before process is called
    void setProcessCallback(std::function<void(const juce::String&, float)> callback)
    {
        m_processCallback = callback;
    }

    // this must be set before process is called, otherwise this could produce undefined behaviour 
    void setTargetParameter(std::atomic<float>* value, juce::NormalisableRange<float> range, const juce::String& id)
    {
        m_targetID = id;
        m_targetValue = value;
        m_minValue = range.start;
        m_maxValue = range.end;
    }

    void setLfoRate(float rate)
    {
        m_modLfoRate = rate;
        m_modLfo.setFrequency(rate);
    }

    void setLfoDepth(float depth)
    {
        m_modLfoDepth.setTargetValue(depth);
    }

    void setLfoType(WaveType type)
    {
        m_modLfo.setType(type);
    }

    const juce::String& getCurrentID()
    {
        return m_targetID;
    }

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

} // dingus