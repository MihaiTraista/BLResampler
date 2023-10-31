/*
  ==============================================================================

    EventInterface.hpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"

class EventInterface
{
public:
    virtual void handleSliderValueChanged(juce::Slider* slider) = 0;
    virtual void handleButtonClicked(juce::Button* button) = 0;
    virtual void handleComboBoxChanged(juce::ComboBox* box) = 0;
    
private:
};
