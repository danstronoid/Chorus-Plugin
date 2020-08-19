/*
  ==============================================================================

    ModComponent.h
    Created: 19 Aug 2020 12:24:56pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
*/
class ModComponent : public juce::Component
{
public:
    ModComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;
        // target box
        targetBox.addItem("Rate", 1);
        targetBox.addItem("Depth", 2);
        targetBox.addItem("Mix", 3);
        targetBox.addItem("Delay", 4);
        targetBox.addItem("Width", 5);
        targetBox.setJustificationType(Justification::centred);
        targetAttach.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(parameters, "14_mod_target", targetBox));
        addAndMakeVisible(&targetBox);

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
        sineAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(parameters, "15_mod_type", sineButton));
        addAndMakeVisible(&sineButton);

        // Sliders

        // rate slider
        rateSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        rateSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        rateAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "16_mod_rate", rateSlider));
        addAndMakeVisible(&rateSlider);

        // depth slider
        depthSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        depthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        depthAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "17_mod_depth", depthSlider));
        addAndMakeVisible(&depthSlider);

        // Labels

        // rate label
        rateLabel.setText("Rate", dontSendNotification);
        rateLabel.setJustificationType(Justification::centred);
        rateLabel.attachToComponent(&rateSlider, false);
        addAndMakeVisible(&rateLabel);

        // depth label
        depthLabel.setText("Depth", dontSendNotification);
        depthLabel.setJustificationType(Justification::centred);
        depthLabel.attachToComponent(&depthSlider, false);
        addAndMakeVisible(&depthLabel);

        // target label
        targetLabel.setText("Target", dontSendNotification);
        targetLabel.setJustificationType(Justification::centred);
        targetLabel.attachToComponent(&targetBox, false);
        addAndMakeVisible(&targetLabel);
    }

    ~ModComponent() override
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

        rateSlider.setBounds(area.removeFromLeft(componentWidth));
        depthSlider.setBounds(area.removeFromLeft(componentWidth));

        juce::Rectangle<int> boxArea = area.removeFromLeft(componentWidth);

        targetBox.setSize(componentWidth, componentHeight / 4);
        triButton.setSize(componentWidth / 2, componentHeight / 4);
        sineButton.setSize(componentWidth / 2, componentHeight / 4);

        targetBox.setBoundsToFit(boxArea.removeFromTop(componentHeight / 2), juce::Justification::centred, true);
        triButton.setBoundsToFit(boxArea.removeFromLeft(componentWidth / 2), juce::Justification::centred, true);
        sineButton.setBoundsToFit(boxArea.removeFromLeft(componentWidth / 2), juce::Justification::centred, true);
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    juce::ComboBox targetBox;

    juce::TextButton triButton{ "Tri" };
    juce::TextButton sineButton{ "Sine" };

    juce::Slider rateSlider;
    juce::Slider depthSlider;

    juce::Label targetLabel;
    juce::Label rateLabel;
    juce::Label depthLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> targetAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sineAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModComponent)
};