/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../sjf_audio/sjf_widgets.h"
#include "../sjf_audio/sjf_LookAndFeel.h"
//==============================================================================
/**
*/
class Sjf_convoAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Sjf_convoAudioProcessorEditor (Sjf_convoAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~Sjf_convoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
private:
    
    void setNonAutomatableValues();
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Sjf_convoAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_lookAndFeel otherLookAndFeel;
    
    juce::TextButton loadImpulseButton;
    juce::ToggleButton filterOnOffButton, reverseImpulseButton, palindromeButton;
    juce::Slider preDelaySlider, stretchSlider, lpfCutoffSlider, hpfCutoffSlider, dryWetSlider, inputLevelSlider;
    sjf_twoValSlider startAndEndSlider;
    juce::ToggleButton tooltipsToggle;
    
    juce::Label tooltipLabel, fileNameLabel;
    juce::String MAIN_TOOLTIP = "sjf_convo: \nConvolution plugin... primarily designed as a convolution reverb algorithm, but should be capable of convolving input signal with audio file (although the larger the file the more intense the CPU usage..)\n";

    
    sjf_waveform waveformThumbnail;
    
    
    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > filterOnOffButtonAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > preDelaySliderAttachment, lpfCutoffSliderAttachment, hpfCutoffSliderAttachment, dryWetSliderAttachment, inputLevelSliderAttachment;
    
    bool m_justRestoreGUIFlag = true;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessorEditor)
};
