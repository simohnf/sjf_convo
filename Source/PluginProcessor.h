/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../../sjf_audio/JuceFIR.h"
#include "../../sjf_audio/sjf_convo.h"
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
    void reverseImpulse(){ m_convo.reverseImpulse(); }
    void trimImpulseEnd(){ m_convo.trimImpulseEnd(); }
    
    void setPreDelay( float preDelayInMS ){ m_convo.setPreDelay( getSampleRate() * 0.001f * preDelayInMS ); }
    
    juce::AudioBuffer< float >& getIRBuffer(){ return m_convo.getIRBuffer(); }
    double getIRSampleRate() { return m_convo.getIRSampleRate(); }
private:

    sjf_convo< 2, 2048 > m_convo;
    
//    juce::dsp::Convolution m_conv;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_convoAudioProcessor)
};
