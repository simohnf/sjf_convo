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
    };
    
    
    
//    addAndMakeVisible( &panicButton );
//    panicButton.setButtonText( "PANIC" );
//    panicButton.onClick = [this]
//    {
//        audioProcessor.PANIC();
//    };
    
    addAndMakeVisible( &reverseImpulseButton );
    reverseImpulseButton.setButtonText( "Reverse IR" );
    reverseImpulseButton.onClick = [this]
    {
        audioProcessor.reverseImpulse( reverseImpulseButton.getToggleState() );
        waveformThumbnail.reverseEnvelope();
        auto max = startAndEndSlider.getMaxValue();
        auto min = startAndEndSlider.getMinValue();
        auto dif = max - min;
        min = 1.0f - max;
        max = min + dif;
        if ( max >= startAndEndSlider.getMaxValue() )
        {
            startAndEndSlider.setMaxValue( max );
            startAndEndSlider.setMinValue( min );
        }
        else
        {
            startAndEndSlider.setMinValue( min );
            startAndEndSlider.setMaxValue( max );
        }
        
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    
    
    
    addAndMakeVisible( &trimImpulseButton );
    trimImpulseButton.setButtonText( "Trim IR" );
    trimImpulseButton.onClick = [this]
    {
        audioProcessor.trimImpulseEnd( trimImpulseButton.getToggleState() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    
    
    addAndMakeVisible( &preDelaySlider );
    preDelaySlider.setRange( 0 , 1000 );
    preDelaySlider.setSliderStyle( juce::Slider::Rotary );
    preDelaySlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    preDelaySlider.onValueChange = [this]
    {
        audioProcessor.setPreDelay( preDelaySlider.getValue() );
    };
    
    
    addAndMakeVisible( &stretchSlider );
    stretchSlider.setRange( -2 , 2 );
    stretchSlider.setSliderStyle( juce::Slider::Rotary );
    stretchSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    stretchSlider.onValueChange = [this]
    {
        audioProcessor.setStrecthFactor( stretchSlider.getValue() );
    };

    
    
    addAndMakeVisible( &startAndEndSlider );
    startAndEndSlider.setRange( 0 , 1 );
    startAndEndSlider.setSliderStyle( juce::Slider::TwoValueHorizontal );
    startAndEndSlider.setTextBoxStyle( juce::Slider::NoTextBox, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    startAndEndSlider.setMinValue( 0 );
    startAndEndSlider.setMaxValue( 1 );
    startAndEndSlider.onValueChange = [this]
    {
//        DBG( startAndEndSlider.getMinValue() << " " << startAndEndSlider.getMaxValue() );
        audioProcessor.setImpulseStartAndEnd( startAndEndSlider.getMinValue(), startAndEndSlider.getMaxValue() );
//        audioProcessor.setStrecthFactor( stretchSlider.getValue() );
    };
    
    addAndMakeVisible( &lpfCutoffSlider );
    lpfCutoffSlider.setRange( 20, 20000 );
    lpfCutoffSlider.setValue( 20000 );
    lpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    lpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    lpfCutoffSlider.onValueChange = [this]
    {
        audioProcessor.setLPFCutoff( lpfCutoffSlider.getValue() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    
    addAndMakeVisible( &hpfCutoffSlider );
    hpfCutoffSlider.setRange( 20, 20000 );
    hpfCutoffSlider.setValue( 20 );
    hpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    hpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    hpfCutoffSlider.onValueChange = [this]
    {
        audioProcessor.setHPFCutoff( hpfCutoffSlider.getValue() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    
    addAndMakeVisible( &filterPositionBox );
    filterPositionBox.addItem( "Off", 1 );
    filterPositionBox.addItem( "IR", 2 );
    filterPositionBox.addItem( "Output", 3 );
    filterPositionBox.setSelectedId( 1 );
    filterPositionBox.onChange = [this]
    {
        audioProcessor.setFilterPosition( filterPositionBox.getSelectedId() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    
    addAndMakeVisible( &waveformThumbnail );
    waveformThumbnail.setNormaliseFlag( true );
    waveformThumbnail.shouldOutputOnMouseUp( true );
    waveformThumbnail.onMouseEvent = [this]
    {
        DBG( "WAVEFORM MOUSE EVENT " );
        audioProcessor.setAmplitudeEnvelope( waveformThumbnail.getEnvelope() );
    };
    
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
    
    preDelaySlider.setBounds( loadImpulseButton.getX(), loadImpulseButton.getBottom(), 100, 100 );
    stretchSlider.setBounds( preDelaySlider.getRight(), preDelaySlider.getY(), 100, 100 );
    
    lpfCutoffSlider.setBounds( stretchSlider.getRight(), stretchSlider.getY(), 100, 100 );
    hpfCutoffSlider.setBounds( lpfCutoffSlider.getRight(), lpfCutoffSlider.getY(), 100, 100 );
    filterPositionBox.setBounds( hpfCutoffSlider.getRight(), hpfCutoffSlider.getY(), 50, TEXT_HEIGHT );
//    panicButton.setBounds( filterPositionBox.getX(), filterPositionBox.getBottom(), 50, TEXT_HEIGHT );
    
    waveformThumbnail.setBounds(loadImpulseButton.getX(), preDelaySlider.getBottom(), getWidth(), SLIDER_SIZE*2 );
    startAndEndSlider.setBounds( waveformThumbnail.getX(), waveformThumbnail.getBottom(), waveformThumbnail.getWidth(), TEXT_HEIGHT );
}


//void Sjf_convoAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source)
//{
//    if (source == &thumbnail)       repaint();
//}
