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
    reverseImpulseButton.setToggleState( audioProcessor.getReverseState(), juce::dontSendNotification );
    reverseImpulseButton.onClick = [this]
    {
        if (m_justRestoreGUIFlag){ DBG("DONT DO REVERSE STUFF");return; }
        DBG("DO REVERSE STUFF");
        audioProcessor.reverseImpulse( reverseImpulseButton.getToggleState() );
        waveformThumbnail.reverseEnvelope();
        auto max = startAndEndSlider.getMaxValue();
        auto min = startAndEndSlider.getMinValue();
        auto dif = max - min;
        min = 1.0f - max;
        max = min + dif;
        startAndEndSlider.setMinAndMaxValues( min, max );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    reverseImpulseButton.setTooltip( "This will reverse the impulse response and any start/end or envelope settings" );
    


    
    addAndMakeVisible( &preDelaySlider );
    preDelaySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "preDelay", preDelaySlider )  );
    preDelaySlider.setSliderStyle( juce::Slider::Rotary );
    preDelaySlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    preDelaySlider.setTooltip( "This sets the delay applied to the input signal before it is passed through the convolution algorithm" );
    
    addAndMakeVisible( &stretchSlider );
    stretchSlider.setRange( -2 , 2 );
    stretchSlider.setSliderStyle( juce::Slider::Rotary );
    stretchSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    stretchSlider.setValue( audioProcessor.getStretchFactor() );
    stretchSlider.onValueChange = [this]
    {
        audioProcessor.setStretchFactor( stretchSlider.getValue() );
    };
    stretchSlider.setTooltip("This changes the length of the impulse response. Positive stretching results in an elongated impulse reponse and reduced high frequency content, negative stretching results in a shorter impulse repsonse and increased high frequency content" );
    
    
    addAndMakeVisible( &startAndEndSlider ); 
    startAndEndSlider.setRange( 0 , 1 );
    startAndEndSlider.setSliderStyle( juce::Slider::TwoValueHorizontal );
    startAndEndSlider.setMinAndMaxValues( audioProcessor.getStartAndEnd()[ 0 ], audioProcessor.getStartAndEnd()[ 1 ] );
    startAndEndSlider.setTextBoxStyle( juce::Slider::NoTextBox, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    startAndEndSlider.onValueChange = [this]
    {
        if (m_justRestoreGUIFlag){ DBG("DONT SET START END");return; }
        DBG("Set START/END FROM SLIDER CHANGE" << startAndEndSlider.getMinValue() << " " << startAndEndSlider.getMaxValue());
        audioProcessor.setImpulseStartAndEnd( startAndEndSlider.getMinValue(), startAndEndSlider.getMaxValue() );
    };
    startAndEndSlider.setTooltip( "This allows you to manually trim the beginning or the end of the impulse response" );
    
    
    
    addAndMakeVisible( &lpfCutoffSlider );
    lpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "lpfCutoff", lpfCutoffSlider )  );
    lpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    lpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    lpfCutoffSlider.setTooltip( "This sets the low pass filter cutoff frequency" );
    
    addAndMakeVisible( &hpfCutoffSlider );
    hpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "hpfCutoff", hpfCutoffSlider )  );
    hpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    hpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    hpfCutoffSlider.setTooltip( "This sets the high pass filter cutoff frequency" );
    
    addAndMakeVisible( &filterOnOffButton );
    filterOnOffButton.setButtonText( "Filter" );
    filterOnOffButtonAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment ( valueTreeState, "filterOnOff", filterOnOffButton ));
    filterOnOffButton.setTooltip( "This turns on/off the two filters" );
    
    addAndMakeVisible( &waveformThumbnail );
    waveformThumbnail.shouldOutputOnMouseUp( true );
    waveformThumbnail.onMouseEvent = [this]
    {
        if (m_justRestoreGUIFlag){ return; }
        audioProcessor.setAmplitudeEnvelope( waveformThumbnail.getEnvelope() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    waveformThumbnail.setTooltip( "This displays the impulse response currently in use. \nIt also allows you to create an amplitude envelope which is applied to the impulse. To create new breakpoints in the envelope hold shift and click the, to delete a breakpoint hold alt and click the breakpoint." );
    
    
    addAndMakeVisible( &dryWetSlider );
    dryWetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "mix", dryWetSlider )  );
    dryWetSlider.setSliderStyle( juce::Slider::Rotary );
    dryWetSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, dryWetSlider.getWidth(), TEXT_HEIGHT );
    dryWetSlider.setTooltip( "This sets the percentage of the wet signal sent to the output" );
    

    addAndMakeVisible( &inputLevelSlider );
    inputLevelSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "inputLevel", inputLevelSlider )  );
    inputLevelSlider.setSliderStyle( juce::Slider::Rotary );
    inputLevelSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, dryWetSlider.getWidth(), TEXT_HEIGHT );
    inputLevelSlider.setTooltip( "This sets the level of the input signal before it gets passed through the convolution algorithm" );
    
    addAndMakeVisible( &tooltipsToggle );
    tooltipsToggle.setButtonText("HINTS");
    tooltipsToggle.onClick = [this]
    {
        if (tooltipsToggle.getToggleState())
        {
            tooltipLabel.setVisible( true );
            setSize (WIDTH, HEIGHT+tooltipLabel.getHeight());
        }
        else
        {
            tooltipLabel.setVisible( false );
            setSize (WIDTH, HEIGHT);
        }
    };
    tooltipsToggle.setTooltip( MAIN_TOOLTIP );
    
    addAndMakeVisible(tooltipLabel);
    tooltipLabel.setVisible( false );
    tooltipLabel.setColour( juce::Label::backgroundColourId, otherLookAndFeel.backGroundColour.withAlpha( 0.85f ) );
    tooltipLabel.setTooltip( MAIN_TOOLTIP );
    
    
//    setNonAutomatableValues();
    
    setSize (WIDTH, HEIGHT);
    m_justRestoreGUIFlag = false;
    startTimer( 200 );
    
    DBG("INTERFACE CREATED");
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
    
    inputLevelSlider.setBounds( reverseImpulseButton.getX(), reverseImpulseButton.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    preDelaySlider.setBounds( loadImpulseButton.getRight() + INDENT, loadImpulseButton.getY(), SLIDER_SIZE, SLIDER_SIZE );
    stretchSlider.setBounds( preDelaySlider.getX(), preDelaySlider.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    waveformThumbnail.setBounds(preDelaySlider.getRight() + INDENT, TEXT_HEIGHT, SLIDER_SIZE*4, SLIDER_SIZE*2 + INDENT*3 + TEXT_HEIGHT );
    startAndEndSlider.setBounds( waveformThumbnail.getX(), waveformThumbnail.getBottom(), waveformThumbnail.getWidth(), TEXT_HEIGHT );

    
    lpfCutoffSlider.setBounds( waveformThumbnail.getRight() + INDENT, waveformThumbnail.getY() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    hpfCutoffSlider.setBounds( lpfCutoffSlider.getX(), lpfCutoffSlider.getBottom() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    filterOnOffButton.setBounds( hpfCutoffSlider.getX(), hpfCutoffSlider.getBottom() + INDENT, SLIDER_SIZE, TEXT_HEIGHT );
    
    dryWetSlider.setBounds( lpfCutoffSlider.getRight() + INDENT, lpfCutoffSlider.getY() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    
    
    tooltipsToggle.setBounds( dryWetSlider.getX(), dryWetSlider.getBottom() + INDENT, dryWetSlider.getWidth(), TEXT_HEIGHT );
    
    tooltipLabel.setBounds( 0, HEIGHT, WIDTH, TEXT_HEIGHT*4);
}



void Sjf_convoAudioProcessorEditor::timerCallback()
{
    if ( audioProcessor.stateReloaded() )
    {
        audioProcessor.setStateReloaded( false );
        setNonAutomatableValues();
    }
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}

void Sjf_convoAudioProcessorEditor::setNonAutomatableValues()
{
    m_justRestoreGUIFlag = true;
    reverseImpulseButton.setToggleState( audioProcessor.getReverseState(), juce::dontSendNotification );
    stretchSlider.setValue( audioProcessor.getStretchFactor() );
    
    auto startEnd = audioProcessor.getStartAndEnd();
    DBG( "INTERFACE START " <<  startEnd[ 0 ] << " END " << startEnd[ 1 ] );
    startAndEndSlider.setMinAndMaxValues( startEnd[ 0 ], startEnd[ 1 ] );
    m_justRestoreGUIFlag = false;

    
    //        waveformThumbnail
}
