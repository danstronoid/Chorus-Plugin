/*
  ==============================================================================

    FilterComponent.h
    Created: 19 Aug 2020 12:25:50pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
*/
class FilterComponent : public juce::Component
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;
        // Buttons

        // bypass
        bypassButton.setButtonText("bypass");
        bypassAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(parameters, "13_filter_bypass", bypassButton));
        addAndMakeVisible(&bypassButton);

        // Sliders

        // low pass slider
        loSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        loSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        loAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "12_filter_lopass", loSlider));
        addAndMakeVisible(&loSlider);

        // high pass slider
        hiSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        hiSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        hiAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "11_filter_hipass", hiSlider));
        addAndMakeVisible(&hiSlider);

        // Labels

        // low pass label
        loLabel.setText("Low Pass", dontSendNotification);
        loLabel.setJustificationType(Justification::centred);
        loLabel.attachToComponent(&loSlider, false);
        addAndMakeVisible(&loLabel);

        // high pass label
        hiLabel.setText("High Pass", dontSendNotification);
        hiLabel.setJustificationType(Justification::centred);
        hiLabel.attachToComponent(&hiSlider, false);
        addAndMakeVisible(&hiLabel);
    }

    ~FilterComponent() override
    {
    }

    void paint(juce::Graphics& /*g*/) override
    {

    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(padding);

        int labelArea = padding * 2;
        area.removeFromTop(labelArea);

        int componentWidth = area.getWidth() / 3;
        //int componentHeight = area.getHeight();

        hiSlider.setBounds(area.removeFromLeft(componentWidth));
        loSlider.setBounds(area.removeFromLeft(componentWidth));
        bypassButton.setBounds(area.removeFromLeft(componentWidth));
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    juce::ToggleButton bypassButton;

    juce::Slider loSlider;
    juce::Slider hiSlider;

    juce::Label loLabel;
    juce::Label hiLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hiAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComponent)
};