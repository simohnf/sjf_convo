/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define TEXT_HEIGHT 20
#define SLIDER_SIZE 90
#define INDENT TEXT_HEIGHT*0.5
#define WIDTH SLIDER_SIZE*8 + INDENT*6
#define HEIGHT TEXT_HEIGHT*4 + INDENT*2 + SLIDER_SIZE*2
//==============================================================================
Sjf_convoAudioProcessorEditor::Sjf_convoAudioProcessorEditor (Sjf_convoAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState( vts )
{
    setLookAndFeel( &otherLookAndFeel );
    
    
    addAndMakeVisible( &loadImpulseButton );
    loadImpulseButton.setButtonText( "Load" );
    loadImpulseButton.onClick = [this]
    {
        audioProcessor.loadImpulse();
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    loadImpulseButton.setTooltip( "Use this to load a new .wav/.aiff file to use as an impulse response" );
    
    
    
    addAndMakeVisible( &reverseImpulseButton );
    reverseImpulseButton.setButtonText( "Reverse" );
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
    reverseImpulseButton.setTooltip( "This will reverse the impulse response and any start/end or envelope settings" );
    


    
    addAndMakeVisible( &preDelaySlider );
//    preDelaySlider.setRange( 0 , 100 );
    preDelaySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "preDelay", preDelaySlider )  );
    preDelaySlider.setSliderStyle( juce::Slider::Rotary );
    preDelaySlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    preDelaySlider.onValueChange = [this]
    {
        audioProcessor.setPreDelay( preDelaySlider.getValue() );
    };
    preDelaySlider.setTooltip( "This sets the delay applied to the input signal before it is passed through the convolution algorithm" );
    
    addAndMakeVisible( &stretchSlider );
//    stretchSlider.setRange( -2 , 2 );
    stretchSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "stretch", stretchSlider )  );
    stretchSlider.setSliderStyle( juce::Slider::Rotary );
    stretchSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    stretchSlider.onValueChange = [this]
    {
        audioProcessor.setStrecthFactor( stretchSlider.getValue() );
    };
    stretchSlider.setTooltip("This changes the length of the impulse response. Positive stretching results in an elongated impulse reponse and reduced high frequency content, negative stretching results in a shorter impulse repsonse and increased high frequency content" );
    
    
    addAndMakeVisible( &startAndEndSlider );
    startAndEndSlider.setRange( 0 , 1 );
//    startAndEndSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "stretch", stretchSlider )  );
    startAndEndSlider.setSliderStyle( juce::Slider::TwoValueHorizontal );
    startAndEndSlider.setTextBoxStyle( juce::Slider::NoTextBox, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    startAndEndSlider.setMinValue( 0 );
    startAndEndSlider.setMaxValue( 1 );
    startAndEndSlider.onValueChange = [this]
    {
        DBG( startAndEndSlider.getMinValue() << " " << startAndEndSlider.getMaxValue() );
        audioProcessor.setImpulseStartAndEnd( startAndEndSlider.getMinValue(), startAndEndSlider.getMaxValue() );
//        audioProcessor.setStrecthFactor( stretchSlider.getValue() );
    };
    startAndEndSlider.setTooltip( "This allows you to manually trim the beginning or the end of the impulse response" );
    
    
    
    addAndMakeVisible( &lpfCutoffSlider );
//    lpfCutoffSlider.setRange( 20, 20000 );
//    lpfCutoffSlider.setValue( 20000 );
    lpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "lpfCutoff", lpfCutoffSlider )  );
    lpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    lpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    lpfCutoffSlider.onValueChange = [this]
    {
        audioProcessor.setLPFCutoff( lpfCutoffSlider.getValue() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    lpfCutoffSlider.setTooltip( "This sets the low pass filter cutoff frequency" );
    
    addAndMakeVisible( &hpfCutoffSlider );
//    hpfCutoffSlider.setRange( 20, 20000 );
//    hpfCutoffSlider.setValue( 20 );
    hpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "hpfCutoff", hpfCutoffSlider )  );
    hpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    hpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    hpfCutoffSlider.onValueChange = [this]
    {
        audioProcessor.setHPFCutoff( hpfCutoffSlider.getValue() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    hpfCutoffSlider.setTooltip( "This sets the high pass filter cutoff frequency" );
    
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
    filterPositionBox.setTooltip( "This allows you to set the position of the two filter. \nOff turns off both filters, IR filters the impulse response itself, Output places the filter after the convolution engine filtering the output of the algorithm " );
    
    addAndMakeVisible( &waveformThumbnail );
//    waveformThumbnail.setNormaliseFlag( true );
    waveformThumbnail.shouldOutputOnMouseUp( true );
    waveformThumbnail.onMouseEvent = [this]
    {
        DBG( "WAVEFORM MOUSE EVENT " );
        audioProcessor.setAmplitudeEnvelope( waveformThumbnail.getEnvelope() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    waveformThumbnail.setTooltip( "This displays the impulse response currently in use. \nIt also allows you to create an amplitude envelope which is applied to the impulse. To create new breakpoints in the envelope hold shift and click the, to delete a breakpoint hold alt and click the breakpoint." );
    
    
    addAndMakeVisible( &dryWetSlider );
//    dryWetSlider.setRange( 0, 100 );
    dryWetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "mix", dryWetSlider )  );
    dryWetSlider.setSliderStyle( juce::Slider::Rotary );
    dryWetSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, dryWetSlider.getWidth(), TEXT_HEIGHT );
    dryWetSlider.onValueChange = [this]
    {
        audioProcessor.setDryWet( dryWetSlider.getValue() );
    };
    dryWetSlider.setTooltip( "This sets the percentage of the wet signal sent to the output" );
    
    //     , , , inputLevelSliderAttachment;
    addAndMakeVisible( &inputLevelSlider );
//    inputLevelSlider.setRange( -100, 6 );
//    inputLevelSlider.setValue( 0 );
    inputLevelSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "inputLevel", inputLevelSlider )  );
    inputLevelSlider.setSliderStyle( juce::Slider::Rotary );
    inputLevelSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, dryWetSlider.getWidth(), TEXT_HEIGHT );
    inputLevelSlider.onValueChange = [this]
    {
        audioProcessor.setInputLevelDB( inputLevelSlider.getValue() );
    };
    inputLevelSlider.setTooltip( "This sets the level of the input signal before it gets passed through the convolution algorithm" );
    
    addAndMakeVisible( &tooltipsToggle );
    tooltipsToggle.setButtonText("HINTS");
    tooltipsToggle.onClick = [this]
    {
        if (tooltipsToggle.getToggleState())
        {
            //            tooltipWindow.getObject().setAlpha(1.0f);
            tooltipLabel.setVisible( true );
            setSize (WIDTH, HEIGHT+tooltipLabel.getHeight());
        }
        else
        {
            tooltipLabel.setVisible( false );
            setSize (WIDTH, HEIGHT);
            //            tooltipWindow.getObject().setAlpha(0.0f);
        }
    };
    tooltipsToggle.setTooltip( MAIN_TOOLTIP );
    
    addAndMakeVisible(tooltipLabel);
    tooltipLabel.setVisible( false );
    tooltipLabel.setColour( juce::Label::backgroundColourId, otherLookAndFeel.backGroundColour.withAlpha( 0.85f ) );
    tooltipLabel.setTooltip( MAIN_TOOLTIP );
    
    setSize (WIDTH, HEIGHT);
    startTimer( 200 );
    
}

Sjf_convoAudioProcessorEditor::~Sjf_convoAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
}

//==============================================================================
void Sjf_convoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
#ifdef JUCE_DEBUG
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
#else
    juce::Rectangle<int> r = { (int)( WIDTH ), (int)(HEIGHT + tooltipLabel.getHeight()) };
    sjf_makeBackground< 40 >( g, r );
#endif
    
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText( "sjf_convo", 0, 0, getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    g.drawFittedText( "Input", inputLevelSlider.getX(), inputLevelSlider.getY() - TEXT_HEIGHT, inputLevelSlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    g.drawFittedText( "PreDelay", preDelaySlider.getX(), preDelaySlider.getY() - TEXT_HEIGHT, preDelaySlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    g.drawFittedText( "Stretch", stretchSlider.getX(), stretchSlider.getY() - TEXT_HEIGHT, stretchSlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    g.drawFittedText( "Impulse Response", waveformThumbnail.getX(), waveformThumbnail.getY(), waveformThumbnail.getWidth(), waveformThumbnail.getHeight(), juce::Justification::centred, 1 );
    g.drawFittedText( "Start and End Points", startAndEndSlider.getX(), startAndEndSlider.getY(), startAndEndSlider.getWidth(), startAndEndSlider.getHeight(), juce::Justification::centred, 1 );
    
    
    g.drawFittedText( "LPF", lpfCutoffSlider.getX(), lpfCutoffSlider.getY() - TEXT_HEIGHT, lpfCutoffSlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    g.drawFittedText( "HPF", hpfCutoffSlider.getX(), hpfCutoffSlider.getY() - TEXT_HEIGHT, hpfCutoffSlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
    
    g.drawFittedText( "Mix", dryWetSlider.getX(), dryWetSlider.getY() - TEXT_HEIGHT, dryWetSlider.getWidth(), TEXT_HEIGHT, juce::Justification::centred, 1 );
}

void Sjf_convoAudioProcessorEditor::resized()
{
    loadImpulseButton.setBounds( INDENT, TEXT_HEIGHT*2, SLIDER_SIZE, SLIDER_SIZE * 0.5 );
    reverseImpulseButton.setBounds( loadImpulseButton.getX(), loadImpulseButton.getBottom(), loadImpulseButton.getWidth(), loadImpulseButton.getHeight() );
//    trimImpulseButton.setBounds( reverseImpulseButton.getRight(), reverseImpulseButton.getY(), reverseImpulseButton.getWidth(), reverseImpulseButton.getHeight() );
    
    inputLevelSlider.setBounds( reverseImpulseButton.getX(), reverseImpulseButton.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    preDelaySlider.setBounds( loadImpulseButton.getRight() + INDENT, loadImpulseButton.getY(), SLIDER_SIZE, SLIDER_SIZE );
    stretchSlider.setBounds( preDelaySlider.getX(), preDelaySlider.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    waveformThumbnail.setBounds(preDelaySlider.getRight() + INDENT, TEXT_HEIGHT, SLIDER_SIZE*4, SLIDER_SIZE*2 + INDENT*3 + TEXT_HEIGHT );
    startAndEndSlider.setBounds( waveformThumbnail.getX(), waveformThumbnail.getBottom(), waveformThumbnail.getWidth(), TEXT_HEIGHT );

    
    lpfCutoffSlider.setBounds( waveformThumbnail.getRight() + INDENT, waveformThumbnail.getY() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    hpfCutoffSlider.setBounds( lpfCutoffSlider.getX(), lpfCutoffSlider.getBottom() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    filterPositionBox.setBounds( hpfCutoffSlider.getX(), hpfCutoffSlider.getBottom() + INDENT, SLIDER_SIZE, TEXT_HEIGHT );
    
    dryWetSlider.setBounds( lpfCutoffSlider.getRight() + INDENT, lpfCutoffSlider.getY() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    
    
    tooltipsToggle.setBounds( dryWetSlider.getX(), dryWetSlider.getBottom() + INDENT, dryWetSlider.getWidth(), TEXT_HEIGHT );
    
    tooltipLabel.setBounds( 0, HEIGHT, WIDTH, TEXT_HEIGHT*4);
}



void Sjf_convoAudioProcessorEditor::timerCallback()
{
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}
