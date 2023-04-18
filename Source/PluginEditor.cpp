/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define TEXT_HEIGHT 20
#define SLIDER_SIZE 100
//==============================================================================
Sjf_convoAudioProcessorEditor::Sjf_convoAudioProcessorEditor (Sjf_convoAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel( &otherLookAndFeel );
    
    
    addAndMakeVisible( &loadImpulseButton );
    loadImpulseButton.setButtonText( "Load IR" );
    loadImpulseButton.onClick = [this]
    {
        audioProcessor.loadImpulse();
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
//        repaint();
    };
    
    
    
    addAndMakeVisible( &reverseImpulseButton );
    reverseImpulseButton.setButtonText( "Reverse IR" );
    reverseImpulseButton.onClick = [this]
    {
        audioProcessor.reverseImpulse();
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
//        repaint();
    };
    
    
    
    addAndMakeVisible( &trimImpulseButton );
    trimImpulseButton.setButtonText( "Trim IR" );
    trimImpulseButton.onClick = [this]
    {
        audioProcessor.trimImpulseEnd();
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
//        repaint();
    };
    
    
    addAndMakeVisible( &preDelaySlider );
    preDelaySlider.setRange( 0 , 1000 );
    preDelaySlider.setSliderStyle( juce::Slider::Rotary );
    preDelaySlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    preDelaySlider.onValueChange = [this]
    {
        audioProcessor.setPreDelay( preDelaySlider.getValue() );
    };
    
//    thumbnail.addChangeListener (this);
    
    addAndMakeVisible( &waveformThumbnail );
    waveformThumbnail.setNormaliseFlag( true );
    
    
    setSize (500, 400);
}

Sjf_convoAudioProcessorEditor::~Sjf_convoAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
}

//==============================================================================
void Sjf_convoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText( "sjf_convo", 0, 0, getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    
    

}

void Sjf_convoAudioProcessorEditor::resized()
{
    loadImpulseButton.setBounds( 0, TEXT_HEIGHT, 50, TEXT_HEIGHT );
    reverseImpulseButton.setBounds( loadImpulseButton.getRight(), loadImpulseButton.getY(), loadImpulseButton.getWidth(), loadImpulseButton.getHeight() );
    trimImpulseButton.setBounds( reverseImpulseButton.getRight(), reverseImpulseButton.getY(), reverseImpulseButton.getWidth(), reverseImpulseButton.getHeight() );
    preDelaySlider.setBounds( trimImpulseButton.getX(), trimImpulseButton.getBottom(), 100, 100 );
    
    waveformThumbnail.setBounds(loadImpulseButton.getX(), preDelaySlider.getBottom(), getWidth(), SLIDER_SIZE*2 );
}


//void Sjf_convoAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source)
//{
//    if (source == &thumbnail)       repaint();
//}
