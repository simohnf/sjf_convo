/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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

    juce::TextButton loadImpulseButton, reverseImpulseButton;
    juce::Slider preDelaySlider;
    
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;                  
    juce::AudioThumbnail thumbnail;
    
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
    }
    
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::white);
        g.fillRect (thumbnailBounds);
        
        g.setColour (juce::Colours::red);                               // [8]
        
        thumbnail.drawChannels (g,                                      // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessorEditor)
};
