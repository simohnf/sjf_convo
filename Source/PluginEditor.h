/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../sjf_audio/sjf_widgets.h"
#include "../../sjf_audio/sjf_LookAndFeel.h"
//==============================================================================
/**
*/
class Sjf_convoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Sjf_convoAudioProcessorEditor (Sjf_convoAudioProcessor&);
    ~Sjf_convoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Sjf_convoAudioProcessor& audioProcessor;

    sjf_lookAndFeel otherLookAndFeel;
    
    juce::TextButton loadImpulseButton /*, panicButton*/;
    juce::ToggleButton reverseImpulseButton, trimImpulseButton;
    juce::ComboBox filterPositionBox;
    juce::Slider preDelaySlider, stretchSlider, startAndEndSlider, lpfCutoffSlider, hpfCutoffSlider;
    
//    juce::AudioFormatManager formatManager;
//    juce::AudioThumbnailCache thumbnailCache;
//    juce::AudioThumbnail thumbnail;
    

    
    sjf_waveform waveformThumbnail;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessorEditor)
};
