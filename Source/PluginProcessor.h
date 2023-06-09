/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../../sjf_audio/JuceFIR.h"
#include "../sjf_audio/sjf_convo.h"
#include "../sjf_audio/sjf_audioUtilities.h"
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
    bool getReverseState() { return m_convo.getReverseState(); }
    void palindromeImpulse( bool shouldMakePalindromeOfImpulse ){ m_convo.palindromeImpulse( shouldMakePalindromeOfImpulse ); }
    bool getPalindromeState(){ return m_convo.getPalindromeState(); }
    void trimImpulseEnd( bool shouldTrimImpulse ){ m_convo.trimImpulseEnd( shouldTrimImpulse ); }
    
    void setImpulseStartAndEnd( float start0to1, float end0to1 ){ m_convo.setImpulseStartAndEnd( start0to1, end0to1 ); }
    std::array< float, 2 > getStartAndEnd(){ return m_convo.getImpulseStartAndEnd(); }
    
    void setAmplitudeEnvelope( std::vector< std::array< float, 2 > > env ) { m_convo.setAmplitudeEnvelope( env ); }
    std::vector< std::array< float, 2 > > getAmplitudeEnvelope(){ return m_convo.getAmplitudeEnvelope(); }
    
    void setStretchFactor( float stretchFactor ) { m_convo.setStretchFactor( std::pow( 2.0f, stretchFactor ) ); }
    float getStretchFactor(){ return std::log2( m_convo.getStretchFactor() ); } 
    
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
    
    void setNonAutomatableParameterValues();
    
    bool stateReloaded(){ return m_stateReloadedFlag; }
    void setStateReloaded( bool setToFalseIfTheStateWasReloaded ){ m_stateReloadedFlag = setToFalseIfTheStateWasReloaded; }
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    
    juce::String getFilePath(){ return m_convo.getFilePath(); }
    juce::String getFileName(){ return m_convo.getFileName(); }
private:

    juce::AudioProcessorValueTreeState parameters;
    
    sjf_convo< 2, 2048 > m_convo; 
    juce::AudioBuffer< float > m_convBuffer;
    float m_wet = 0, m_inputLevelDB = 0;
    
    
    
    std::atomic<float>* wetMixParameter = nullptr;
    std::atomic<float>* inputLevelParameter = nullptr;
    std::atomic<float>* filterOnOffParameter = nullptr;
    std::atomic<float>* LPFCutoffParameter = nullptr;
    std::atomic<float>* HPFCutoffParameter = nullptr;
    std::atomic<float>* preDelayParameter = nullptr;
    

    juce::Value nEnvPointsParameter, stretchParameter, startParameter, endParameter, reverseParameter, palindromeParameter, filePathParameter;
    juce::Value envelopeParameterString;
    
    bool m_stateReloadedFlag = false;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessor)
};
