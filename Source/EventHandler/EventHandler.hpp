/*
  ==============================================================================

    EventHandler.hpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"
#include "../EventInterface/EventInterface.hpp"

class EventHandler : public EventInterface
{
public:
    EventHandler();
    ~EventHandler();
    
    void handleSliderValueChanged(juce::Slider* slider) override;
    void handleButtonClicked(juce::Button* button) override;
    void handleComboBoxChanged(juce::ComboBox* box) override;
        
private:
};
