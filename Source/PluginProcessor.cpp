/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_convoAudioProcessor::Sjf_convoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
, parameters(*this, nullptr, juce::Identifier("sjf_convo"), createParameterLayout() )
#endif
{


    m_convBuffer.setSize( 2, getBlockSize() );
    m_convo.trimImpulseEnd( true );
    




    
    wetMixParameter = parameters.getRawParameterValue("mix");
    inputLevelParameter = parameters.getRawParameterValue("inputLevel");
    filterPositionParamer = parameters.getRawParameterValue("filterPosition");
    LPFCutoffParameter = parameters.getRawParameterValue("lpfCutoff");;
    HPFCutoffParameter = parameters.getRawParameterValue("hpfCutoff");
    stretchParameter = parameters.getRawParameterValue("stretch");
    preDelayParameter = parameters.getRawParameterValue("preDelay");
    startParameter = parameters.getRawParameterValue("start");
    endParameterParameter = parameters.getRawParameterValue("end");
}

Sjf_convoAudioProcessor::~Sjf_convoAudioProcessor()
{
}

//==============================================================================
const juce::String Sjf_convoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Sjf_convoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_convoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_convoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Sjf_convoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Sjf_convoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Sjf_convoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Sjf_convoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Sjf_convoAudioProcessor::getProgramName (int index)
{
    return {};
}

void Sjf_convoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Sjf_convoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_convo.prepare( sampleRate, samplesPerBlock );
    m_convBuffer.setSize( 2, samplesPerBlock );
}

void Sjf_convoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Sjf_convoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Sjf_convoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto bufferSize = buffer.getNumSamples();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, bufferSize );

    m_convBuffer.makeCopyOf( buffer );
    
    
    auto inLevel = std::pow( 10, m_inputLevelDB/20 );
    m_convBuffer.applyGain( inLevel );
    m_convo.process( m_convBuffer );


    auto wet = std::sqrt( m_wet * 0.01f );
    auto dry = std::sqrt( 1 - (m_wet * 0.01f) );
    DBG( "DRY " << dry << " WET " << wet );
//    DBG( m_convBuffer.getNumChannels() << " " << m_convBuffer.getNumSamples()  << " " << buffer.getNumChannels() << " " << buffer.getNumSamples() );
    buffer.applyGain( dry );
    m_convBuffer.applyGain( wet );

    for ( int c = 0; c < totalNumOutputChannels; c++ )
    {
        buffer.addFrom( c, 0, m_convBuffer, c, 0, bufferSize );
    }
}

//==============================================================================
bool Sjf_convoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Sjf_convoAudioProcessor::createEditor()
{
    return new Sjf_convoAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void Sjf_convoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Sjf_convoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout Sjf_convoAudioProcessor::createParameterLayout()
{
    static constexpr int pIDVersionNumber = 1;
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    //    filterOrder
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "inputLevel", pIDVersionNumber }, "inputLevel", -80, 6, 0 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "mix", pIDVersionNumber }, "Mix", 0, 100, 100 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "filterPosition", pIDVersionNumber }, "FilterPosition", 1, 3, 1 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "lpfCutoff", pIDVersionNumber }, "LpfCutoff", 10, 20000, 20000 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "hpfCutoff", pIDVersionNumber }, "HpfCutoff", 10, 20000, 10 ) );
    
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "stretch", pIDVersionNumber }, "Stretch", -2, 2, 0 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "preDelay", pIDVersionNumber }, "Predelay", 0, 100, 0 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "start", pIDVersionNumber }, "Start", 0, 1, 0 ) );
    params.add( std::make_unique<juce::AudioParameterFloat>( juce::ParameterID{ "end", pIDVersionNumber }, "End", 0, 1, 1 ) );
    
    return params;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Sjf_convoAudioProcessor();
}

