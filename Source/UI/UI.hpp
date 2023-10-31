/*
  ==============================================================================

    UI.hpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class UI : public juce::Component
{
public:
    UI();
    ~UI() override;

    void paint(juce::Graphics&) override;
        
    void resized() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UI)
};
