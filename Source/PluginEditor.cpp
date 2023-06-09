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
    DBG("REVERSE 1 " << ( audioProcessor.getReverseState() ? "ON" : "OFF" ) );
    reverseImpulseButton.setToggleState( audioProcessor.getReverseState(), juce::dontSendNotification );
    reverseImpulseButton.onClick = [this]
    {
//        if (m_justRestoreGUIFlag){ return; } // ?????
        audioProcessor.reverseImpulse( reverseImpulseButton.getToggleState() );
        waveformThumbnail.reverseEnvelope();
        auto minMax = startAndEndSlider.getMinAndMaxValues();
        auto min = minMax[ 0 ];
        auto max = minMax[ 1 ];
        auto dif = max - min;
        min = 1.0f - max;
        max = min + dif;
        startAndEndSlider.setMinAndMaxValues( min, max );
        startAndEndSlider.onMouseEvent();
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    reverseImpulseButton.setTooltip( "This will reverse the impulse response and any start/end or envelope settings" );
    


    addAndMakeVisible( &palindromeButton );
    palindromeButton.setButtonText( "Palindrome" );
    DBG("PALINDROME 1 " << ( audioProcessor.getPalindromeState() ? "ON" : "OFF" ) );
    palindromeButton.setToggleState( audioProcessor.getPalindromeState(), juce::dontSendNotification );
    palindromeButton.onClick = [this]
    {
//        if (m_justRestoreGUIFlag){ return; } // ?????
        audioProcessor.palindromeImpulse( palindromeButton.getToggleState() );
    };
    palindromeButton.setTooltip( "This will copy a reverse version of the trimmed section of the impulse response to the end of the impulse response for use in the convolution");
    
    addAndMakeVisible( &preDelaySlider );
    preDelaySliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "preDelay", preDelaySlider )  );
    preDelaySlider.setSliderStyle( juce::Slider::Rotary );
    preDelaySlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    preDelaySlider.setTextValueSuffix("ms");
    preDelaySlider.setTooltip( "This sets the delay applied to the input signal before it is passed through the convolution algorithm" );
    
    addAndMakeVisible( &stretchSlider );
    stretchSlider.setRange( -2 , 2 );
    stretchSlider.setNumDecimalPlacesToDisplay( 3 );
    stretchSlider.setSliderStyle( juce::Slider::Rotary );
    stretchSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    stretchSlider.setValue( audioProcessor.getStretchFactor() );
    stretchSlider.onValueChange = [this]
    {
        audioProcessor.setStretchFactor( stretchSlider.getValue() );
    };
    stretchSlider.setTooltip("This changes the length of the impulse response. \nPositive stretching results in an elongated impulse reponse and reduced high frequency content, negative stretching results in a shorter impulse repsonse and increased high frequency content" );
    
    
    addAndMakeVisible( &startAndEndSlider ); 
    startAndEndSlider.setRange( 0 , 1 );
    startAndEndSlider.setHorizontal( true );
    startAndEndSlider.setMinAndMaxValues( audioProcessor.getStartAndEnd()[ 0 ], audioProcessor.getStartAndEnd()[ 1 ] );
    startAndEndSlider.onMouseEvent = [this]
    {
//        if (m_justRestoreGUIFlag){ return; } /// ?????
        auto minMax = startAndEndSlider.getMinAndMaxValues();
        audioProcessor.setImpulseStartAndEnd( minMax[ 0 ], minMax[ 1 ] );
    };
    startAndEndSlider.setTooltip( "This allows you to manually trim the beginning or the end of the impulse response" );
    
    
    
    addAndMakeVisible( &lpfCutoffSlider );
    lpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "lpfCutoff", lpfCutoffSlider )  );
    lpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    lpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    lpfCutoffSlider.setTextValueSuffix("Hz");
    lpfCutoffSlider.setTooltip( "This sets the low pass filter cutoff frequency" );
    
    addAndMakeVisible( &hpfCutoffSlider );
    hpfCutoffSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "hpfCutoff", hpfCutoffSlider )  );
    hpfCutoffSlider.setSliderStyle( juce::Slider::Rotary );
    hpfCutoffSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, preDelaySlider.getWidth(), TEXT_HEIGHT );
    hpfCutoffSlider.setTextValueSuffix("Hz");
    hpfCutoffSlider.setTooltip( "This sets the high pass filter cutoff frequency" );
    
    addAndMakeVisible( &filterOnOffButton );
    filterOnOffButton.setButtonText( "Filter" );
    filterOnOffButtonAttachment.reset( new juce::AudioProcessorValueTreeState::ButtonAttachment ( valueTreeState, "filterOnOff", filterOnOffButton ));
    filterOnOffButton.setTooltip( "This turns on/off the two filters" );
    
    addAndMakeVisible( &waveformThumbnail );
    waveformThumbnail.shouldOutputOnMouseUp( true );
    waveformThumbnail.onMouseEvent = [this]
    {
//        if (m_justRestoreGUIFlag){ return; }
        audioProcessor.setAmplitudeEnvelope( waveformThumbnail.getEnvelope() );
        waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    };
    waveformThumbnail.drawWaveform( audioProcessor.getIRBuffer() );
    auto env = audioProcessor.getAmplitudeEnvelope();
    waveformThumbnail.setEnvelope( env );
    waveformThumbnail.setTooltip( "This displays the impulse response currently in use. \nIt also allows you to create an amplitude envelope which is applied to the impulse. \nTo create new breakpoints in the envelope hold shift and click, to delete a breakpoint hold alt and click the breakpoint." );
    
    addAndMakeVisible( &fileNameLabel );
    fileNameLabel.setColour( juce::Label::backgroundColourId, juce::Colours::white.withAlpha(0.0f) );
    fileNameLabel.setColour( juce::Label::textColourId, juce::Colours::white.withAlpha(0.3f) );
    fileNameLabel.setJustificationType( juce::Justification::centred );
    fileNameLabel.setTooltip( "This displays the impulse response currently in use. \nIt also allows you to create an amplitude envelope which is applied to the impulse. \nTo create new breakpoints in the envelope hold shift and click the, to delete a breakpoint hold alt and click the breakpoint." );
    fileNameLabel.setInterceptsMouseClicks(false, false);
    
    addAndMakeVisible( &dryWetSlider );
    dryWetSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "mix", dryWetSlider )  );
    dryWetSlider.setSliderStyle( juce::Slider::Rotary );
    dryWetSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, dryWetSlider.getWidth(), TEXT_HEIGHT );
    dryWetSlider.setTextValueSuffix("%");
    dryWetSlider.setTooltip( "This sets the percentage of the wet signal sent to the output" );
    

    addAndMakeVisible( &inputLevelSlider );
    inputLevelSliderAttachment.reset( new juce::AudioProcessorValueTreeState::SliderAttachment ( valueTreeState, "inputLevel", inputLevelSlider )  );
    inputLevelSlider.setSliderStyle( juce::Slider::Rotary );
    inputLevelSlider.setTextBoxStyle( juce::Slider::TextBoxBelow, false, inputLevelSlider.getWidth(), TEXT_HEIGHT );
    inputLevelSlider.setTextValueSuffix("dB");
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
//    m_justRestoreGUIFlag = false;
    startTimer( 300 );
    
//    DBG("INTERFACE CREATED");
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
    
    
    inputLevelSlider.setBounds( INDENT, TEXT_HEIGHT*2, SLIDER_SIZE, SLIDER_SIZE );
    preDelaySlider.setBounds( inputLevelSlider.getX(), inputLevelSlider.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    loadImpulseButton.setBounds( inputLevelSlider.getRight() + INDENT, inputLevelSlider.getY(), SLIDER_SIZE, SLIDER_SIZE/3 );
    reverseImpulseButton.setBounds( loadImpulseButton.getX(), loadImpulseButton.getBottom(), loadImpulseButton.getWidth(), loadImpulseButton.getHeight() );
    palindromeButton.setBounds( reverseImpulseButton.getX(), reverseImpulseButton.getBottom(), reverseImpulseButton.getWidth(), reverseImpulseButton.getHeight() );

    stretchSlider.setBounds( palindromeButton.getX(), palindromeButton.getBottom() + TEXT_HEIGHT + INDENT, SLIDER_SIZE, SLIDER_SIZE );
    
    
    waveformThumbnail.setBounds(loadImpulseButton.getRight() + INDENT, TEXT_HEIGHT, SLIDER_SIZE*4, SLIDER_SIZE*2 + INDENT*3 + TEXT_HEIGHT );
    fileNameLabel.setBounds(waveformThumbnail.getX(), waveformThumbnail.getBottom()-TEXT_HEIGHT, waveformThumbnail.getWidth(), TEXT_HEIGHT );
    startAndEndSlider.setBounds( waveformThumbnail.getX(), waveformThumbnail.getBottom(), waveformThumbnail.getWidth(), TEXT_HEIGHT );

    
    lpfCutoffSlider.setBounds( waveformThumbnail.getRight() + INDENT, waveformThumbnail.getY() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    hpfCutoffSlider.setBounds( lpfCutoffSlider.getX(), lpfCutoffSlider.getBottom() + TEXT_HEIGHT, SLIDER_SIZE, SLIDER_SIZE );
    filterOnOffButton.setBounds( hpfCutoffSlider.getX(), hpfCutoffSlider.getBottom() + INDENT, SLIDER_SIZE, TEXT_HEIGHT );
    
    dryWetSlider.setBounds( lpfCutoffSlider.getRight() + INDENT, lpfCutoffSlider.getY() + TEXT_HEIGHT*2, SLIDER_SIZE, SLIDER_SIZE );
    
    
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
    fileNameLabel.setText( audioProcessor.getFileName(), juce::dontSendNotification );
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}

void Sjf_convoAudioProcessorEditor::setNonAutomatableValues()
{
//    m_justRestoreGUIFlag = true;
    DBG("REVERSE " << ( audioProcessor.getReverseState() ? "ON" : "OFF" ) );
    reverseImpulseButton.setToggleState( audioProcessor.getReverseState(), juce::dontSendNotification );
    DBG("PALINDROME " << ( audioProcessor.getPalindromeState() ? "ON" : "OFF" ) );
    palindromeButton.setToggleState( audioProcessor.getPalindromeState(), juce::dontSendNotification );
    stretchSlider.setValue( audioProcessor.getStretchFactor() );
    
    auto startEnd = audioProcessor.getStartAndEnd();
    startAndEndSlider.setMinAndMaxValues( startEnd[ 0 ], startEnd[ 1 ] );
    auto env = audioProcessor.getAmplitudeEnvelope();
    waveformThumbnail.setEnvelope( env );
//    m_justRestoreGUIFlag = false;

    
    //        waveformThumbnail
}
