/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#include "WaveformDisplay.hpp"

WaveformDisplay::WaveformDisplay(const std::vector<float>& audioVector,
                                 const std::vector<bool>& zeroCrossings,
                                 const std::vector<bool>& vectorThatShowsWhichSamplesAreCommitted,
                                 const int& closestZeroCrossingStartPointer,
                                 const int& closestZeroCrossingEndPointer,
                                 int displayStartSample,
                                 int displayLengthInSamples):
    pAudioVector(&audioVector),
    pZeroCrossings(&zeroCrossings),
    pVectorThatShowsWhichSamplesAreCommitted(&vectorThatShowsWhichSamplesAreCommitted),
    pClosestZeroCrossingStart(&closestZeroCrossingStartPointer),
    pClosestZeroCrossingEnd(&closestZeroCrossingEndPointer),
    mDisplayStartSample(displayStartSample),
    mDisplayLengthInSamples(displayLengthInSamples),
    mShowZeroCrossings(true)
{}

WaveformDisplay::WaveformDisplay(const std::vector<float>& audioVector,
                                 int displayStartSample,
                                 int displayLengthInSamples):
    pAudioVector(&audioVector),
    mDisplayStartSample(displayStartSample),
    mDisplayLengthInSamples(displayLengthInSamples),
    mShowZeroCrossings(false)
{}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    if (pAudioVector->size() < 2 || mDisplayLengthInSamples < 2)
        return;
    
    g.fillAll(juce::Colour::fromRGB(10, 10, 20));

    g.setColour(juce::Colour::fromRGB(180, 180, 150));

    float width = getWidth();
    float height = getHeight();

    // if the zoom factor is larger than 10 times the width, then we draw an envelope follower
    // by averaging the absolute sample values at that location and drawing a line that is symmetric to the center
    // if the zoom factor is less than 10 times the width,
    // we draw the actual sample values with lines that are above or below the center
    bool zoomOutThresholdExceeded = mDisplayLengthInSamples > width * 10;

    const float* bufferToDraw = pAudioVector->data();

    if (zoomOutThresholdExceeded){
        drawWaveformAsEnvelopeFollower(g, width, height, mDisplayLengthInSamples, bufferToDraw);
    } else{
        drawWaveformSampleBySample(g, width, height, mDisplayLengthInSamples, bufferToDraw);
    }
    
    if(mShowZeroCrossings)
        drawZeroCrossingCirclesAndHintLines(g, width, height, mDisplayLengthInSamples, bufferToDraw);
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::drawWaveformAsEnvelopeFollower(juce::Graphics& g,
                                                     int width,
                                                     int height,
                                                     int numSamplesToDisplay,
                                                     const float* bufferToDraw){
    int startSampleIndex, endSampleIndex;
    
    for (int x = 0; x < width; ++x){
        startSampleIndex = juce::jmap<float>(x, 0, width, mDisplayStartSample, mDisplayStartSample + numSamplesToDisplay);
        endSampleIndex = juce::jmap<float>(x + 1, 0, width, mDisplayStartSample, mDisplayStartSample + numSamplesToDisplay);

        float sum = 0.0f;
        for (int i = startSampleIndex; i < endSampleIndex; ++i){
            sum += std::abs(bufferToDraw[i]);
        }
        float averageSampleValue = sum / static_cast<float>(endSampleIndex - startSampleIndex);
        float lineLength = averageSampleValue * (height / 2.0f);
        g.setColour(juce::Colour::fromRGB(220, 220, 200));
        g.drawVerticalLine(x, height / 2.0f - lineLength, height / 2.0f + lineLength);
    }
}

void WaveformDisplay::drawWaveformSampleBySample(juce::Graphics& g,
                                                 int width,
                                                 int height,
                                                 int numSamplesToDisplay,
                                                 const float* bufferToDraw){
    int sampleIndex;
    
    for (int x = 0; x < width; ++x)
    {
        sampleIndex = juce::jmap<float>(x, 0, width, mDisplayStartSample, mDisplayStartSample + numSamplesToDisplay);

        auto sampleValue = bufferToDraw[sampleIndex];
        float topCoordinate = sampleValue >= 0.0f ? (1.0f - (sampleValue * 0.5f + 0.5f)) * height : height / 2.0f;
        float bottomCoordinate = sampleValue >= 0.0f ? height / 2.0f : (1.0f - (sampleValue * 0.5f + 0.5f)) * height;
        
        if (pVectorThatShowsWhichSamplesAreCommitted && (*pVectorThatShowsWhichSamplesAreCommitted)[sampleIndex]){
            g.setColour(juce::Colour::fromRGB(120, 120, 110));
        } else {
            g.setColour(juce::Colour::fromRGB(220, 220, 200));
        }

        g.drawVerticalLine(x, topCoordinate, bottomCoordinate);
    }

}

void WaveformDisplay::drawZeroCrossingCirclesAndHintLines(juce::Graphics& g,
                                                          int width,
                                                          int height,
                                                          int numSamplesToDisplay,
                                                          const float* bufferToDraw){
    auto startIndexInZeroCrossings = mDisplayStartSample;
    auto endIndexInZeroCrossings = mDisplayStartSample + numSamplesToDisplay;
    
    for (int index = startIndexInZeroCrossings; index < endIndexInZeroCrossings; ++index)
    {
        if((*pZeroCrossings)[index] == true)
        {
            // Map the sample index to an x-coordinate on the screen
            auto x = juce::jmap<float>(index, mDisplayStartSample, mDisplayStartSample + numSamplesToDisplay, 0, width);

            g.setColour(juce::Colour::fromRGB(10, 180, 255));
            g.fillEllipse(x - 2, height / 2.0f - 2, 4, 4);

            if((*pClosestZeroCrossingStart) == index)
            {
                g.setColour(juce::Colour::fromRGB(255, 200, 0));
                g.drawEllipse(x - 11, height / 2.0f - 11, 22, 22, 1);
                g.drawEllipse(x - 8, height / 2.0f - 8, 16, 16, 1);
            }
            else if((*pClosestZeroCrossingEnd) == index)
            {
                g.setColour(juce::Colour::fromRGB(255, 0, 0));
                g.drawEllipse(x - 11, height / 2.0f - 11, 22, 22, 1);
                g.drawEllipse(x - 8, height / 2.0f - 8, 16, 16, 1);
            }
        }
    }

    g.setColour(juce::Colour::fromRGB(120, 120, 120));
    g.drawVerticalLine(width / 4.0f, height / 4.0f, height / 4.0f * 3.0f);
    g.drawVerticalLine((width / 4.0f) * 3, height / 4.0f, height / 4.0f * 3.0f);
//    drawTeardropShape(g, width / 4 - 10, height / 2 - 160, 20, 90, false, false);
//    drawTeardropShape(g, (width / 4) * 3 - 10, height / 2 - 160, 20, 90, false, false);
}

void WaveformDisplay::drawTeardropShape(juce::Graphics& g, int x, int y, int width, int height, bool centered, bool reversed)
{
    // when centered, it places the teardrop with the x and y at the center
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    juce::Path teardrop;

    if(centered){
        teardrop.startNewSubPath(x, y - halfHeight); // Start at the top center
        teardrop.quadraticTo(x + halfWidth, y - halfHeight, x, y + halfHeight); // Right curve to bottom center
        teardrop.quadraticTo(x - halfWidth, y - halfHeight, x, y - halfHeight); // Left curve to top center
    } else {
        teardrop.startNewSubPath(x + halfWidth, y); // Start at the top center
        teardrop.quadraticTo(x + width, y, x + halfWidth, y + height); // Right curve to bottom center
        teardrop.quadraticTo(x, y, x + halfWidth, y); // Left curve to top center
    }

    g.setColour(juce::Colour::fromRGB(150, 150, 50));
    g.fillPath(teardrop);
//    juce::PathStrokeType strokeType(1.0f);
//    g.strokePath(teardrop, strokeType);
}
