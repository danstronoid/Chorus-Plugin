/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChoruspluginAudioProcessorEditor::ChoruspluginAudioProcessorEditor (ChoruspluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), parameters(vts),
    mainComponent(vts), titleComponent(vts), tabComponent(vts)
{
    setSize (windowWidth, windowHeight);

    addAndMakeVisible(&mainComponent);
    addAndMakeVisible(&titleComponent);
    addAndMakeVisible(&tabComponent);
}

ChoruspluginAudioProcessorEditor::~ChoruspluginAudioProcessorEditor()
{
}

//==============================================================================
void ChoruspluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void ChoruspluginAudioProcessorEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds().reduced(windowPadding);

    int componentHeight = area.getHeight() / 2;
    int componentWidth = area.getWidth() / 2;

    juce::Rectangle<int> top = area.removeFromTop(componentHeight);
    titleComponent.setBounds(top.removeFromLeft(componentWidth));
    tabComponent.setBounds(top.removeFromLeft(componentWidth));

    mainComponent.setBounds(area.removeFromTop(componentHeight));
}
