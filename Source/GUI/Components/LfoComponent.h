/*
  ==============================================================================

    LfoComponent.h
    Created: 19 Aug 2020 12:25:14pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
*/
class LfoComponent : public juce::Component
{
public:
    LfoComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;
        // Buttons

        // tri button
        triButton.setRadioGroupId(1);
        triButton.setClickingTogglesState(true);
        triButton.setConnectedEdges(2);
        triButton.setToggleState(true, NotificationType::dontSendNotification);
        addAndMakeVisible(&triButton);

        // sine button, this is connected to the lfo type parameter
        sineButton.setRadioGroupId(1);
        sineButton.setClickingTogglesState(true);
        sineButton.setConnectedEdges(1);
        sineAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(parameters, "08_lfo_type", sineButton));
        addAndMakeVisible(&sineButton);

        // Sliders

        // phase L slider
        phaseLSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        phaseLSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        phaseLAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "09_lfo_phaseL", phaseLSlider));
        addAndMakeVisible(&phaseLSlider);

        // phase R slider
        phaseRSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        phaseRSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        phaseRAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "10_lfo_phaseR", phaseRSlider));
        addAndMakeVisible(&phaseRSlider);

        // Labels

        // phase L label
        phaseLLabel.setText("Phase L", dontSendNotification);
        phaseLLabel.setJustificationType(Justification::centred);
        phaseLLabel.attachToComponent(&phaseLSlider, false);
        addAndMakeVisible(&phaseLLabel);

        // phase R label
        phaseRLabel.setText("Phase R", dontSendNotification);
        phaseRLabel.setJustificationType(Justification::centred);
        phaseRLabel.attachToComponent(&phaseRSlider, false);
        addAndMakeVisible(&phaseRLabel);


    }

    ~LfoComponent() override
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
        int componentHeight = area.getHeight();

        phaseLSlider.setBounds(area.removeFromLeft(componentWidth));
        phaseRSlider.setBounds(area.removeFromLeft(componentWidth));

        triButton.setSize(componentWidth / 2, componentHeight / 4);
        sineButton.setSize(componentWidth / 2, componentHeight / 4);

        triButton.setBoundsToFit(area.removeFromLeft(componentWidth / 2), juce::Justification::centred, true);
        sineButton.setBoundsToFit(area.removeFromLeft(componentWidth / 2), juce::Justification::centred, true);
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    juce::TextButton triButton{ "Tri" };
    juce::TextButton sineButton{ "Sine" };

    juce::Slider phaseLSlider;
    juce::Slider phaseRSlider;

    juce::Label phaseLLabel;
    juce::Label phaseRLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sineAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseLAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseRAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoComponent)
};