#include "DragDropArea.hpp"

DragDropArea::DragDropArea(juce::String displayString,
                           std::vector<float>& aV,
                           juce::String& fN,
                           std::function<void(bool isResampled)> newFileWasDroppedReference) :
    notifyParentThatNewFileWasDropped(newFileWasDroppedReference),
    mDisplayString(displayString),
    mAudioVector(aV),
    mFileName(fN)
{}

DragDropArea::~DragDropArea(){}

void DragDropArea::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGBA(255, 255, 255, 20));

    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 160));
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 1);

    g.setFont(11.0f);
    g.drawText("Drag & Drop", 0, 3, getWidth(), 16, juce::Justification::centredTop);

    g.setFont(16.0f);
    g.setColour(juce::Colour::fromRGBA(240, 240, 100, 200));
    
    g.drawText(mDisplayString, 0, 21, getWidth(), 16, juce::Justification::centredTop);
    
    if (mIsMouseHovering){
        g.setColour(juce::Colour::fromRGBA(255, 255, 255, 150));
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 10);
    }
}

void DragDropArea::resized(){}

void DragDropArea::filesDropped (const juce::StringArray& files, int x, int y)
{
    mIsMouseHovering = false;

    juce::File audioFile(files[0]);

    mFileName = audioFile.getFileName();
    
    pFileHandler->readAudioFileAndCopyToVector(audioFile, mAudioVector);
    
    notifyParentThatNewFileWasDropped(mDisplayString == juce::String("Resampled"));

    std::cout << "Files Read" << std::endl;
}
