/*
  ==============================================================================

    ChorusEngine.cpp
    Created: 18 Aug 2020 1:50:56pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#include "ChorusEngine.h"

namespace dingus
{

//==============================================================================
template<typename SampleType>
ChorusEngine<SampleType>::ChorusEngine()
{
}

template<typename SampleType>
void ChorusEngine<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    // resize vectors for the number of channels
    jassert(spec.numChannels > 0);
    m_mixLevel.resize(spec.numChannels);
    m_boostFilters.resize(spec.numChannels);
    m_cutFilters.resize(spec.numChannels);

    m_sampleRate = static_cast<SampleType>(spec.sampleRate);

    // low and high pass filters
    auto& hiPass = processorChain.get<highPassIndex>();
    hiPass.state->type = FilterParams::Type::highPass;
    hiPass.state->setCutOffFrequency(spec.sampleRate, m_hiPassCutoff.getCurrentValue());

    auto& lowPass = processorChain.get<lowPassIndex>();
    lowPass.state->type = FilterParams::Type::lowPass;
    lowPass.state->setCutOffFrequency(spec.sampleRate, m_lowPassCutoff.getCurrentValue());

    // shelving filters for cut and boost, centered at 200Hz, Q = 1, with 0.3x boost/cut
    auto boostCoef = juce::dsp::IIR::Coefficients<SampleType>::makeLowShelf(spec.sampleRate, m_crossoverFreq, SampleType(1), SampleType(13e-1));
    auto cutCoef = juce::dsp::IIR::Coefficients<SampleType>::makeLowShelf(spec.sampleRate, m_crossoverFreq, SampleType(1), SampleType(7e-1));

    // initialize all filters
    for (auto& boostFilter : m_boostFilters)
    {
        boostFilter.prepare(spec);
        boostFilter.coefficients = boostCoef;
    }

    for (auto& cutFilter : m_cutFilters)
    {
        cutFilter.prepare(spec);
        cutFilter.coefficients = cutCoef;
    }

    tempBlock = juce::dsp::AudioBlock<SampleType>(heapBlock, spec.numChannels, spec.maximumBlockSize);
    processorChain.prepare(spec);

    // set ramped values
    for (auto& mixLevel : m_mixLevel)
        mixLevel.reset(spec.sampleRate, 0.2);

    m_lowPassCutoff.reset(spec.sampleRate, 0.5);
    m_hiPassCutoff.reset(spec.sampleRate, 0.5);
}

template<typename SampleType>
void ChorusEngine<SampleType>::reset()
{
    processorChain.reset();

    for (auto& boostFilter : m_boostFilters)
        boostFilter.reset();

    for (auto& cutFitler : m_cutFilters)
        cutFitler.reset();
}

//==============================================================================

template<typename SampleType>
void ChorusEngine<SampleType>::updateFilterCutoffs(int skip /*= 0*/)
{
    m_hiPassCutoff.skip(skip);
    m_lowPassCutoff.skip(skip);

    auto& hiPass = processorChain.get<highPassIndex>();
    hiPass.state->setCutOffFrequency(m_sampleRate, m_hiPassCutoff.getNextValue());

    auto& lowPass = processorChain.get<lowPassIndex>();
    lowPass.state->setCutOffFrequency(m_sampleRate, m_lowPassCutoff.getNextValue());
}

//==============================================================================

template<typename SampleType>
void ChorusEngine<SampleType>::setMix(SampleType mix)
{
    for (auto& mixLevel : m_mixLevel)
        mixLevel.setTargetValue(mix);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setHighPass(SampleType cutoff)
{
    m_hiPassCutoff.setTargetValue(cutoff);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setLowPass(SampleType cutoff)
{
    m_lowPassCutoff.setTargetValue(cutoff);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setFilterBypass(bool bypass)
{
    processorChain.setBypassed<highPassIndex>(bypass);
    processorChain.setBypassed<lowPassIndex>(bypass);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setRate(SampleType rate)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setRate(rate);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setDepth(SampleType depth)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setDepth(depth);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setDelayTime(SampleType delayTime)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setDelayTime(delayTime);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setDelayWidth(SampleType width)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setDelayWidth(width);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setVoiceSpread(SampleType spread)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setVoiceSpread(spread);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setMode(Mode mode)
{
    m_mode = mode;
}

template<typename SampleType>
void ChorusEngine<SampleType>::setNumVoice(size_t numVoices)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setActiveVoices(numVoices + 1);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setPhaseOffset(SampleType phaseOffset, size_t channel /*= 0*/)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setPhaseOffset(phaseOffset, channel);
}

template<typename SampleType>
void ChorusEngine<SampleType>::setLfoType(WaveType type)
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    chorusVoices.setLfoType(type);
}

template<typename SampleType>
int ChorusEngine<SampleType>::getLatency() const
{
    auto& chorusVoices = processorChain.get<voicesIndex>();
    return juce::roundToInt(chorusVoices.getDelayTime() * m_sampleRate);
}

//==============================================================================

} // dingus