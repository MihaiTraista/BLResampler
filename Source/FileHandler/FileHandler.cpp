/*
  ==============================================================================

    FileHandler.cpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#include <vector>

#include "FileHandler.hpp"

FileHandler::FileHandler(){
    mFormatManager.registerBasicFormats();
}

FileHandler::~FileHandler(){}

void FileHandler::saveResampledFileOnDisk(std::vector<float>& mResampledCycles){
    std::cout << "Saving File..." << std::endl;

    // Create an AudioBuffer to hold your data
    juce::AudioBuffer<float> buffer(1, static_cast<int>(mResampledCycles.size()));
    buffer.copyFrom(0, 0, mResampledCycles.data(), static_cast<int>(mResampledCycles.size()));

    // Define file path
    juce::File file(juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getFullPathName() + "/output.wav");

    // Get the WAV format
    std::unique_ptr<juce::AudioFormat> wavFormat = std::make_unique<juce::WavAudioFormat>();

    // Create an OutputStream to write the file
    std::unique_ptr<juce::FileOutputStream> outStream(file.createOutputStream());

    if (outStream)
    {
        // Create an AudioFormatWriter
        std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat->createWriterFor(outStream.get(), 44100, buffer.getNumChannels(), 16, {}, 0));

        if (writer)
        {
            outStream.release(); // The writer will now manage the output stream
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
            std::cout << "File Saved!" << std::endl;
        }
        else
        {
            std::cout << "Error: Couldn't create AudioFormatWriter." << std::endl;
        }
    }
    else
    {
        std::cout << "Error: Couldn't create FileOutputStream." << std::endl;
    }
}

void FileHandler::storeAudioFileInBuffer(juce::File& audioFile, juce::AudioBuffer<float>& mAudioFileBuffer){
    std::cout << "Storing audio file in buffer..." << std::endl;
    
    auto* reader = mFormatManager.createReaderFor(audioFile);

    if(reader){
        
        std::cout << "reader->sampleRate: " << reader->sampleRate << std::endl;
        
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        mReaderSource.reset(newSource.release());
        
        mAudioFileBuffer.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
        
        reader->read(&mAudioFileBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        std::cout << "Done storing audio file in buffer!" << std::endl;
        
        if (mReaderSource.get() == nullptr){
            std::cout << "no readerSource" << std::endl;
        }
    } else {
        std::cout << "no reader" << std::endl;
    }
}
