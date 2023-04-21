/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../../sjf_audio/JuceFIR.h"
#include "../../sjf_audio/sjf_convo.h"
#include "../../sjf_audio/sjf_audioUtilities.h"
//==============================================================================
/**
*/
class Sjf_convoAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Sjf_convoAudioProcessor();
    ~Sjf_convoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadImpulse(){ m_convo.loadImpulse(); }
    void PANIC(){ m_convo.PANIC(); }
    void reverseImpulse( bool shouldReverseImpulse ){ m_convo.reverseImpulse( shouldReverseImpulse ); }
    void trimImpulseEnd( bool shouldTrimImpulse ){ m_convo.trimImpulseEnd( shouldTrimImpulse ); }
    void setImpulseStartAndEnd( float start0to1, float end0to1 ){ m_convo.setImpulseStartAndEnd( start0to1, end0to1 ); }
    void setAmplitudeEnvelope( std::vector< std::array< float, 2 > > env )
    {
        DBG("SHOULD SET ENVELOPE");
        m_convo.setAmplitudeEnvelope( env );
    }
    void setStrecthFactor( float stretchFactor )
    {
        stretchFactor = std::pow( 2.0f, stretchFactor );
        m_convo.setStrecthFactor( stretchFactor );
    }
    
    void setFilterPosition( int filterPosition ){ m_convo.setFilterPosition( filterPosition ); }
    void setLPFCutoff( float f )
    {
        auto cutoffCoefficient = calculateLPFCoefficient< float >( f, getSampleRate() );
        m_convo.setLPFCutoff( cutoffCoefficient );
    }
    void setHPFCutoff( float f )
    {
        auto cutoffCoefficient = calculateLPFCoefficient< float >( f, getSampleRate() );
        m_convo.setHPFCutoff( cutoffCoefficient );
    }
    
    void setPreDelay( float preDelayInMS ){ m_convo.setPreDelay( getSampleRate() * 0.001f * preDelayInMS ); }
    
    void setDryWet( float wetPercentage ){ m_wet = wetPercentage; }
    void setInputLevelDB( float inputLevelDB ) { m_inputLevelDB = inputLevelDB; }
    
    
    juce::AudioBuffer< float >& getIRBuffer(){ return m_convo.getIRBuffer(); }
    double getIRSampleRate() { return m_convo.getIRSampleRate(); }
private:

    sjf_convo< 2, 2048 > m_convo; 
    juce::AudioBuffer< float > m_convBuffer;
    float m_wet = 0, m_inputLevelDB = 0;
    
//    juce::dsp::Convolution m_conv;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessor)
};
