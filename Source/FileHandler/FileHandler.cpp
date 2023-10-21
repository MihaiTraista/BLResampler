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

void FileHandler::readAudioFileAndCopyToVector(juce::File& audioFile, std::vector<float>& audioVector){
    std::cout << "Storing audio file in buffer..." << std::endl;
    
    auto* reader = mFormatManager.createReaderFor(audioFile);

    if(reader){
        
        std::cout << "reader->sampleRate: " << reader->sampleRate << std::endl;
        
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        mReaderSource.reset(newSource.release());
        
        audioVector.resize(static_cast<int>(reader->lengthInSamples));
        
        // Create a temporary AudioBuffer that points to the vector's data
        juce::AudioBuffer<float> tempAudioBuffer(1, static_cast<int>(reader->lengthInSamples));

        reader->read(&tempAudioBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        
        // Copy the data from the temporary AudioBuffer to the vector
        auto* channelData = tempAudioBuffer.getReadPointer(0);
        std::copy(channelData, channelData + audioVector.size(), audioVector.begin());
        
        std::cout << "Done storing audio file in vector!" << std::endl;
        
        if (mReaderSource.get() == nullptr){
            std::cout << "no readerSource" << std::endl;
        }
    } else {
        std::cout << "no reader" << std::endl;
    }
}

void FileHandler::saveVectorAsAudioFileToDesktop(const std::vector<float>& audioData, const juce::String fileName)
{
    // Get the user's desktop directory
    juce::File desktop = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);

    juce::Time currentTime = juce::Time::getCurrentTime();
    juce::String timeString = currentTime.toString(true, true, true, true); // date, time, include seconds, use 24-hour clock
    timeString = timeString.replaceCharacters(": ", "--");

    // Create the complete filename
    juce::File audioFile = desktop.getChildFile(fileName + "--" + timeString + ".wav");

    // Create a FileOutputStream for the audioFile
    std::unique_ptr<juce::FileOutputStream> fileStream(audioFile.createOutputStream());

    if (fileStream)
    {
        // Create a WavAudioFormat object
        juce::WavAudioFormat wavFormat;

        // Create an AudioFormatWriter
        std::unique_ptr<juce::AudioFormatWriter> audioWriter;
        audioWriter.reset(wavFormat.createWriterFor(fileStream.get(), 44100, 1, 16, {}, 0));

        if (audioWriter)
        {
            fileStream.release(); // The writer will delete the stream for us
            
            // Put the vector data into an AudioBuffer
            juce::AudioBuffer<float> buffer(1, audioData.size());
            buffer.copyFrom(0, 0, audioData.data(), audioData.size());

            // Write buffer to audio file
            audioWriter->writeFromAudioSampleBuffer(buffer, 0, audioData.size());
        }
    }
}
