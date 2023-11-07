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

        normalizeAudioVector(audioVector);
        
        std::cout << "Done storing audio file in vector!" << std::endl;
        
        if (mReaderSource.get() == nullptr){
            std::cout << "no readerSource" << std::endl;
        }
    } else {
        std::cout << "no reader" << std::endl;
    }
}

void FileHandler::exportFiles(const std::vector<float>& origAudioData,
                            const std::vector<float>& resampledCycles,
                            const std::vector<float>& polarCycles,
                            const std::array<std::vector<float>, N_WT_BANDS>& resynthesizedCycles,
                            const juce::String exportFolderName)
{
    // Get the user's desktop directory
    juce::File desktop = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);

    juce::Time currentTime = juce::Time::getCurrentTime();
    juce::String timeString = currentTime.toString(true, true, true, true); // date, time, include seconds, use 24-hour clock
    timeString = timeString.replaceCharacters(": ", "--");

    // BLResampler folder on desktop
    juce::File folderBLResampler = desktop.getChildFile("BLResamplerExports/");
    if (!folderBLResampler.exists())
        folderBLResampler.createDirectory();

    juce::File folderOfThisExport = folderBLResampler.getChildFile(exportFolderName + "--" + timeString);
    folderOfThisExport.createDirectory();

    saveMonoAudioFile(origAudioData, folderOfThisExport, "original");
    saveMonoAudioFile(resampledCycles, folderOfThisExport, "resampled");
    saveMonoAudioFile(polarCycles, folderOfThisExport, "polar");
    saveMultiChannelAudioFile(resynthesizedCycles, folderOfThisExport, "resynthesized");
}

void FileHandler::saveMonoAudioFile(const std::vector<float> &audioVector,
                                                 const juce::File& folderOfThisExport,
                                                 const juce::String fileName){
    // Create the complete filename
    juce::File audioFile = folderOfThisExport.getChildFile(fileName + ".wav");

    // Create a FileOutputStream for the audioFile
    std::unique_ptr<juce::FileOutputStream> fileStream(audioFile.createOutputStream());

    if (fileStream){
        // Create a WavAudioFormat object
        juce::WavAudioFormat wavFormat;

        // Create an AudioFormatWriter
        std::unique_ptr<juce::AudioFormatWriter> audioWriter;
        audioWriter.reset(wavFormat.createWriterFor(fileStream.get(), 44100, 1, 16, {}, 0));

        if (audioWriter)
        {
            fileStream.release(); // The writer will delete the stream for us
            
            // Put the vector data into an AudioBuffer
            juce::AudioBuffer<float> buffer(1, static_cast<int>(audioVector.size()));
            buffer.copyFrom(0, 0, audioVector.data(), static_cast<int>(audioVector.size()));

            // Write buffer to audio file
            audioWriter->writeFromAudioSampleBuffer(buffer, 0, static_cast<int>(audioVector.size()));
        }
    } else {
        std::cerr << "Could not save to file" << std::endl;
    }
}

void FileHandler::saveMultiChannelAudioFile(const std::array<std::vector<float>, N_WT_BANDS>& arrayOfVectors,
                                            const juce::File& folderOfThisExport,
                                            const juce::String fileName){
    // all channels will have the same length, so we take numSamples from channel zero
    int numSamples = static_cast<int>(arrayOfVectors[0].size());
    
    // Create the complete filename
    juce::File audioFile = folderOfThisExport.getChildFile(fileName + ".wav");

    // Create a FileOutputStream for the audioFile
    std::unique_ptr<juce::FileOutputStream> fileStream(audioFile.createOutputStream());

    if (fileStream){
        // Create a WavAudioFormat object
        juce::WavAudioFormat wavFormat;

        // Create an AudioFormatWriter
        std::unique_ptr<juce::AudioFormatWriter> audioWriter;
        audioWriter.reset(wavFormat.createWriterFor(fileStream.get(), 44100, N_WT_BANDS, 16, {}, 0));

        if (audioWriter)
        {
            fileStream.release(); // The writer will delete the stream for us
            
            // this 10-channel buffer will store all data from the arrayOfVectors
            juce::AudioBuffer<float> buffer(N_WT_BANDS, numSamples);

            for(int bandIndex = 0; bandIndex < N_WT_BANDS; bandIndex++){
                buffer.copyFrom(bandIndex, 0, arrayOfVectors[bandIndex].data(), numSamples);
            }
            // Write buffer to audio file
            audioWriter->writeFromAudioSampleBuffer(buffer, 0, numSamples);
        }
    } else {
        std::cerr << "Could not save to file" << std::endl;
    }
}

void FileHandler::normalizeAudioVector(std::vector<float>& audioVector){
    float* data = audioVector.data();
    float maxVal = 0.0f;
    
    for(int i = 0; i < audioVector.size(); ++i){
        if(fabs(data[i] > maxVal))
            maxVal = fabs(data[i]);
    }

    float scaler = 1.0f / maxVal;
    scaler *= 0.95; //  leave a bit of headroom
    
    for(int i = 0; i < audioVector.size(); ++i){
        data[i] = data[i] * scaler;
    }
}

void FileHandler::saveVectorAsTextFileOnDesktop(std::vector<float>& vector, juce::String fileName){
    juce::File desktop = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
    juce::File textFile = desktop.getChildFile(fileName + ".txt");
    juce::FileOutputStream outputStream(textFile);

    if(!outputStream.openedOk()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    for(size_t i = 0; i < vector.size(); i++) {
        outputStream << static_cast<int>(i) << ", " << vector[i] << ";\n";
    }

    // Close the output stream
    outputStream.flush();
}

void FileHandler::storeDefaultCelloBinaryFileInOrigAudioData(std::vector<float>& audioVector){
    auto* data = BinaryData::Cello_C2_short_wav;
    int dataSize = BinaryData::Cello_C2_short_wavSize;
    
    // Create an InputStream to read from the binary data
    std::unique_ptr<juce::MemoryInputStream> inputStream(new juce::MemoryInputStream(data, dataSize, false));

    // Create a format manager and use it to create the reader
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats(); // Registers the basic formats, e.g., WAV, AIFF

    // Now you can use this inputStream to read the audio file as if it were a file on the disk
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(inputStream)));

    // Use the reader as needed, for example, to load the audio into an AudioBuffer
    if (reader != nullptr) {
        juce::AudioBuffer<float> buffer(reader->numChannels, static_cast<int>(reader->lengthInSamples));
        reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        
        int audioFileLengthInSamples = buffer.getNumSamples();
        
        audioVector.resize(audioFileLengthInSamples);
        const float* bufferData = buffer.getReadPointer(0);
        for(int i = 0; i < audioFileLengthInSamples; i++){
            audioVector[i] = bufferData[i];
        }
    }
}
