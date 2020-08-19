/*
  ==============================================================================

    TitleComponent.h
    Created: 19 Aug 2020 11:56:54am
    Author:  Daniel Schwartz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/*
    This title component holds all title text/graphics as well as master input/output controls.
*/
class TitleComponent : public juce::Component
{
public:
    TitleComponent(juce::AudioProcessorValueTreeState& vts) :
        parameters(vts)
    {
        using namespace juce;

        //Image
 /*       image = Drawable::createFromSVGFile(graphics.getChildFile("YourFileHere.svg"));
        image->replaceColour(Colours::black, Colours::darkgrey);
        addAndMakeVisible(*image);*/

        // Sliders

        // in slider
        inSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        inSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        inAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "18_input_gain", inSlider));
        addAndMakeVisible(&inSlider);

        // out slider
        outSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        outSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        outAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(parameters, "19_output_gain", outSlider));
        addAndMakeVisible(&outSlider);

        // Labels

        // in label
        inLabel.setText("Input", dontSendNotification);
        inLabel.setJustificationType(Justification::centred);
        inLabel.attachToComponent(&inSlider, false);
        addAndMakeVisible(&inLabel);

        // out label
        outLabel.setText("Output", dontSendNotification);
        outLabel.setJustificationType(Justification::centred);
        outLabel.attachToComponent(&outSlider, false);
        addAndMakeVisible(&outLabel);
    }

    ~TitleComponent() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int>area = getLocalBounds().reduced(padding);

        /*g.setColour(Colours::darkcyan);
        g.fillRoundedRectangle(area.toFloat(), 10);*/

        int textHeight = area.getHeight() / 4;

        juce::Font title;
        title.setSizeAndStyle(48, "Arial", 1, 0);
        title.setBold(true);

        juce::Font subtitle;
        subtitle.setSizeAndStyle(18, "Arial", 1, 0);
        subtitle.setBold(false);

        g.setColour(juce::Colours::ghostwhite);

        g.setFont(title);
        g.drawText("Chorus", area.removeFromTop(textHeight), juce::Justification::centredTop, false);

        g.setFont(subtitle);
        g.drawText("by Dingus Audio", area.removeFromTop(textHeight), juce::Justification::centredTop, false);


    }

    void resized() override
    {
        juce::Rectangle<int>area = getLocalBounds().reduced(padding * 2);

        int componentWidth = area.getWidth() / 4;
        int componentHeight = area.getHeight() / 2;

        // leave space for title
        area.removeFromTop(componentHeight);

        outSlider.setBounds(area.removeFromRight(componentWidth));
        inSlider.setBounds(area.removeFromRight(componentWidth));

        area = getLocalBounds().reduced(padding * 2);

        //image->setBoundsToFit(area.removeFromBottom(componentHeight).removeFromLeft(componentWidth * 2), Justification::bottomLeft, false);
    }

private:
    juce::AudioProcessorValueTreeState& parameters;

    const int padding{ 10 };

    //const juce::File graphics{ juce::File("C:/Users/Daniel Schwartz/Documents/JUCE/DualChorus/Source/GUI/Graphics") };
    //std::unique_ptr<juce::Drawable> image;

    juce::Slider inSlider;
    juce::Slider outSlider;

    juce::Label inLabel;
    juce::Label outLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleComponent)
};
