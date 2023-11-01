#pragma once

#include <JuceHeader.h>
#include <functional>
#include <vector>
#include <memory>

class DragDropArea :    public juce::Component,
                        public juce::FileDragAndDropTarget
{
public:
    DragDropArea(juce::String displayString,
                 std::function<void(juce::File, bool)> newFileWasDroppedReference);
    
    ~DragDropArea() override;

    void paint(juce::Graphics&) override;

    void resized() override;
    
    inline bool isInterestedInFileDrag (const juce::StringArray& files) override { return true; };
    inline void fileDragEnter (const juce::StringArray& files, int x, int y) override {
        mIsMouseHovering = true;
        repaint();
    };
    inline void fileDragExit (const juce::StringArray& files) override {
        mIsMouseHovering = false;
        repaint();
    };
    inline void fileDragMove (const juce::StringArray& files, int x, int y) override {};
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    std::function<void(juce::File, bool)> notifyParentThatNewFileWasDropped;
    
    juce::String mDisplayString;

//    juce::String mFileName = "";
    
    bool mIsMouseHovering = false;

//    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragDropArea)
};
