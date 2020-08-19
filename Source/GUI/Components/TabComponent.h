/*
  ==============================================================================

    TabComponent.h
    Created: 19 Aug 2020 11:56:36am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>
#include "VoiceComponent.h"
#include "LfoComponent.h"
#include "FilterComponent.h"
#include "ModComponent.h"

//==============================================================================
/*
    This tabbed component has tabs for various components that are categorized by 
    functionality.
*/
class TabComponent : public juce::Component
{
public:
    TabComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts), tabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop),
        voiceComponent(vts), lfoComponent(vts), filterComponent(vts), modComponent(vts)
    {
        tabbedComponent.setOutline(0);
        tabbedComponent.setIndent(0);
        tabbedComponent.addTab("Engine", juce::Colours::transparentBlack, &voiceComponent, true, 0);
        tabbedComponent.addTab("LFO", juce::Colours::transparentBlack, &lfoComponent, true, 1);
        tabbedComponent.addTab("Filter", juce::Colours::transparentBlack, &filterComponent, true, 2);
        tabbedComponent.addTab("Mod", juce::Colours::transparentBlack, &modComponent, true, 3);
        addAndMakeVisible(&tabbedComponent);
    }

    ~TabComponent() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int>area = getLocalBounds().reduced(padding);

        g.setColour(getLookAndFeel().findColour(juce::Toolbar::ColourIds::backgroundColourId));
        g.fillRoundedRectangle(area.toFloat(), 10);
    }

    void resized() override
    {
        juce::Rectangle<int>area = getLocalBounds().reduced(padding * 2);

        tabbedComponent.setBounds(area);
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding = 10;

    juce::TabbedComponent tabbedComponent;

    VoiceComponent voiceComponent;
    LfoComponent lfoComponent;
    FilterComponent filterComponent;
    ModComponent modComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabComponent)
};