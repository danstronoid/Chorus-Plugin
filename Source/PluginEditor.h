/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Components/MainComponent.h"
#include "GUI/Components/TabComponent.h"
#include "GUI/Components/TitleComponent.h"

//==============================================================================
/**
*/
class ChoruspluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChoruspluginAudioProcessorEditor (ChoruspluginAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~ChoruspluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ChoruspluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;

    const int windowPadding{ 20 };
    const int windowWidth{ 800 + 2 * windowPadding };
    const int windowHeight{ 400 + 2 * windowPadding };

    MainComponent mainComponent;
    TitleComponent titleComponent;
    TabComponent tabComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoruspluginAudioProcessorEditor)
};
