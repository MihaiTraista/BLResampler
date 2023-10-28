#pragma once

#include <JuceHeader.h>
#include <functional>
#include <vector>
#include <memory>

#include "../FileHandler/FileHandler.hpp"

class DragDropArea :    public juce::Component,
                        public juce::FileDragAndDropTarget
{
public:
    DragDropArea(juce::String displayString,
                 std::vector<float>& aV,
                 juce::String& fN,
                 std::function<void()> newFileWasDroppedReference);
    
    ~DragDropArea() override;

    void paint(juce::Graphics&) override;
    
    void resized() override;
    
    inline bool isInterestedInFileDrag (const juce::StringArray& files) override { return true; };
    inline void fileDragEnter (const juce::StringArray& files, int x, int y) override {};
    inline void fileDragExit (const juce::StringArray& files) override {};
    inline void fileDragMove (const juce::StringArray& files, int x, int y) override {};
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    std::function<void()> notifyParentThatNewFileWasDropped;
    
    juce::String mDisplayString;
    std::vector<float>& mAudioVector;
    juce::String& mFileName;

    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragDropArea)
};
