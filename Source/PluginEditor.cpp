/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define TEXT_HEIGHT 20
//==============================================================================
Sjf_convoAudioProcessorEditor::Sjf_convoAudioProcessorEditor (Sjf_convoAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), thumbnailCache (5), thumbnail (512, formatManager, thumbnailCache)
{
    
    addAndMakeVisible( &loadImpulseButton );
    loadImpulseButton.setButtonText( "Load IR" );
    loadImpulseButton.onClick = [this]
    {
        audioProcessor.loadImpulse();
    };
    
    
    
    addAndMakeVisible( &reverseImpulseButton );
    reverseImpulseButton.setButtonText( "Reverse IR" );
    reverseImpulseButton.onClick = [this]
    {
        audioProcessor.reverseImpulse();
        auto IR = audioProcessor.getIRBuffer();
        auto SR = audioProcessor.getIRSampleRate();
        auto nSamps = IR.getNumSamples();
        thumbnail.reset( IR.getNumChannels(), SR, nSamps );
        thumbnail.addBlock( nSamps, IR, 0, nSamps );
        repaint();
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
    
    setSize (400, 300);
}

Sjf_convoAudioProcessorEditor::~Sjf_convoAudioProcessorEditor()
{
}

//==============================================================================
void Sjf_convoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText( "sjf_convo", 0, 0, getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    juce::Rectangle<int> thumbnailBounds ( 100, 0, getWidth() - 100, getHeight() - 120);
    
    
    auto IR = audioProcessor.getIRBuffer();
    auto SR = audioProcessor.getIRSampleRate();
    auto nSamps = IR.getNumSamples();
    auto nChans = IR.getNumChannels();
    if ( nChans > 0 )
    {
        auto stride = nSamps / thumbnailBounds.getWidth();
    auto chanHeight = thumbnailBounds.getHeight() / nChans;
    auto hChanHeight = chanHeight * 0.5;
    for ( int c = 0; c < nChans; c++ )
    {
        auto centre = c * chanHeight + thumbnailBounds.getY() + hChanHeight;
        for ( int s = 0; s < thumbnailBounds.getWidth(); s++ )
        {
            auto lineSize = IR.getSample( c, s*stride ) * hChanHeight;
            if ( lineSize >= 0 )
            {
                g.drawVerticalLine( thumbnailBounds.getX() + s, centre+lineSize, centre );
            }
            else
            {
                g.drawVerticalLine( thumbnailBounds.getX() + s, centre, centre + abs(lineSize) );
            }
            
        }
    }
        
    }
//    if (thumbnail.getNumChannels() == 0)
//        paintIfNoFileLoaded (g, thumbnailBounds);
//    else
//        paintIfFileLoaded (g, thumbnailBounds);
}

void Sjf_convoAudioProcessorEditor::resized()
{
    loadImpulseButton.setBounds( 0, 0, 50, TEXT_HEIGHT );
    reverseImpulseButton.setBounds( loadImpulseButton.getX(), loadImpulseButton.getBottom(), loadImpulseButton.getWidth(), loadImpulseButton.getHeight() );
    preDelaySlider.setBounds( reverseImpulseButton.getX(), reverseImpulseButton.getBottom(), 100, 100 );
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


//void Sjf_convoAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source)
//{
//    if (source == &thumbnail)       repaint();
//}
