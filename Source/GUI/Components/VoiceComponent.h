/*
  ==============================================================================

    VoiceComponent.h
    Created: 19 Aug 2020 12:24:38pm
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
*/
class VoiceComponent : public juce::Component
{
public:
    VoiceComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;

        // mode combobox
        modeBox.addItem("Stereo", 1);
        modeBox.addItem("Mono", 2);
        modeBox.addItem("Dimension", 3);
        modeBox.addItem("Vibrato", 4);
        modeBox.setJustificationType(Justification::centred);
        modeAttach.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(parameters, "05_chorus_mode", modeBox));
        addAndMakeVisible(&modeBox);

        // Sliders

        // voice slider
        voiceSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        voiceSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        voiceAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "06_chorus_voices", voiceSlider));
        addAndMakeVisible(&voiceSlider);

        // spread slider
        spreadSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        spreadSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        spreadAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "07_chorus_spread", spreadSlider));
        addAndMakeVisible(&spreadSlider);

        // Labels

        // mode label
        modeLabel.setText("Mode", dontSendNotification);
        modeLabel.setJustificationType(Justification::centred);
        modeLabel.attachToComponent(&modeBox, false);
        addAndMakeVisible(&modeLabel);

        // voice label
        voiceLabel.setText("Voices", dontSendNotification);
        voiceLabel.setJustificationType(Justification::centred);
        voiceLabel.attachToComponent(&voiceSlider, false);
        addAndMakeVisible(&voiceLabel);

        // spread label
        spreadLabel.setText("Spread", dontSendNotification);
        spreadLabel.setJustificationType(Justification::centred);
        spreadLabel.attachToComponent(&spreadSlider, false);
        addAndMakeVisible(&spreadSlider);
    }

    ~VoiceComponent() override
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

        voiceSlider.setBounds(area.removeFromLeft(componentWidth));
        spreadSlider.setBounds(area.removeFromLeft(componentWidth));

        modeBox.setSize(componentWidth, componentHeight / 4);
        modeBox.setBoundsToFit(area.removeFromLeft(componentWidth), juce::Justification::centred, true);
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    juce::ComboBox modeBox;
    juce::Slider voiceSlider;
    juce::Slider spreadSlider;

    juce::Label modeLabel;
    juce::Label voiceLabel;
    juce::Label spreadLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voiceAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceComponent)
};