/*
  ==============================================================================

    MainComponent.h
    Created: 19 Aug 2020 11:56:15am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
    This is the main component which holds the controls for the core parameters
    of the chorus.
*/
class MainComponent : public juce::Component
{
public:
    MainComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;
        // Sliders

        // rate slider
        rateSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        rateSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        rateAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "00_chorus_rate", rateSlider));
        addAndMakeVisible(&rateSlider);

        // depth slider
        depthSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        depthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        depthAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "01_chorus_depth", depthSlider));
        addAndMakeVisible(&depthSlider);

        // mix slider
        mixSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        mixSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        mixAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "02_chorus_mix", mixSlider));
        addAndMakeVisible(&mixSlider);

        // delay slider
        delaySlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        delayAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "03_chorus_delay", delaySlider));
        addAndMakeVisible(&delaySlider);

        // width slider
        widthSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        widthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        widthAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "04_chorus_width", widthSlider));
        addAndMakeVisible(&widthSlider);

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

        // mix label
        mixLabel.setText("Mix", dontSendNotification);
        mixLabel.setJustificationType(Justification::centred);
        mixLabel.attachToComponent(&mixSlider, false);
        addAndMakeVisible(&mixLabel);

        // delay label
        delayLabel.setText("Delay", dontSendNotification);
        delayLabel.setJustificationType(Justification::centred);
        delayLabel.attachToComponent(&delaySlider, false);
        addAndMakeVisible(&delayLabel);

        // width label
        widthLabel.setText("Width", dontSendNotification);
        widthLabel.setJustificationType(Justification::centred);
        widthLabel.attachToComponent(&widthSlider, false);
        addAndMakeVisible(&widthLabel);

    }

    ~MainComponent() override
    {
    }

    void paint(juce::Graphics& /*g*/) override
    {
        /*Rectangle<int>area = getLocalBounds().reduced(padding);

        g.setColour(Colours::darkcyan);
        g.fillRoundedRectangle(area.toFloat(), 10);*/
    }

    void resized() override
    {
        juce::Rectangle<int>area = getLocalBounds().reduced(padding * 2);

        //int labelArea = 20;
        area.removeFromTop(padding * 2);

        int componentWidth = area.getWidth() / 5;
        //int componentHeight = area.getHeight();

        rateSlider.setBounds(area.removeFromLeft(componentWidth));
        depthSlider.setBounds(area.removeFromLeft(componentWidth));
        mixSlider.setBounds(area.removeFromLeft(componentWidth));
        delaySlider.setBounds(area.removeFromLeft(componentWidth));
        widthSlider.setBounds(area.removeFromLeft(componentWidth));
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    juce::Slider rateSlider;
    juce::Slider depthSlider;
    juce::Slider mixSlider;
    juce::Slider delaySlider;
    juce::Slider widthSlider;

    juce::Label rateLabel;
    juce::Label depthLabel;
    juce::Label mixLabel;
    juce::Label delayLabel;
    juce::Label widthLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
