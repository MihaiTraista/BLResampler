/*
  ==============================================================================

    EventHandler.cpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#include "EventHandler.hpp"

EventHandler::EventHandler(){}

EventHandler::~EventHandler(){}

void EventHandler::handleSliderValueChanged(juce::Slider* slider) {
//    if (slider == &mStartSampleIndexSlider){
//        mStartSampleIndex = mStartSampleIndexSlider.getValue();
//        mLargeWaveform.setDisplayStartSample(mStartSampleIndex);
//        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
//                                                                    mClosestZeroCrossingStart,
//                                                                    mClosestZeroCrossingEnd,
//                                                                    mStartSampleIndex,
//                                                                    mCycleLenHint);
//        mEventConfirmationLabel.setVisible(false);
//    } else if (slider == &mCycleLenHintSlider){
//        // update the mStartSampleIndex so that we keep the waveform centered while zooming
//
//        int newHint = mCycleLenHintSlider.getValue();
//        int diff = newHint - mCycleLenHint;
//        int halfDiff = diff / 2.0f;
//
//        mLargeWaveform.setDisplayLengthInSamples(newHint * 2);
//
//        mStartSampleIndexSlider.setRange(0, mOrigAudioData.size() - newHint * 2);
//        mStartSampleIndexSlider.setValue(mStartSampleIndexSlider.getValue() - halfDiff, juce::sendNotification);
//
//        // update mCycleLenHint and find new zeroCrossings start and end points
//        mCycleLenHint = mCycleLenHintSlider.getValue();
//        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
//                                                                    mClosestZeroCrossingStart,
//                                                                    mClosestZeroCrossingEnd,
//                                                                    mStartSampleIndex,
//                                                                    mCycleLenHint);
//
//        mEventConfirmationLabel.setVisible(false);
//    } else if (slider == &mBandSlider){
//        int band = mBandSlider.getValue();
//        mSelectedBand = band;
//        if(mModeResynthesizedButton.getToggleState()){
//            mLargeWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                          mResampledZoomSlider.getMinValue() * WTSIZE,
//                                          mResampledZoomSlider.getMaxValue() * WTSIZE,
//                                          false);
//            mLargeWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                          0,
//                                          static_cast<int>(mResynthesizedCycles[mSelectedBand].size()),
//                                          false);
//            pPlayback->setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                      mResampledZoomSlider.getMinValue() * WTSIZE,
//                                      (mResampledZoomSlider.getMaxValue() - mResampledZoomSlider.getMinValue()) * WTSIZE);
//        }
//    } else if (slider == &mResampledZoomSlider){
//        // we don't want to change the display length in ModeOriginal
//        if(mModeOrigButton.getToggleState())
//            return;
//
//        int minVal = mResampledZoomSlider.getMinValue();
//        int maxVal = mResampledZoomSlider.getMaxValue();
//        int differenceVal = maxVal - minVal;
//
//        mLargeWaveform.setDisplayStartSample(minVal * WTSIZE);
//        mLargeWaveform.setDisplayLengthInSamples(differenceVal * WTSIZE);
//
//        pPlayback->setReadingStart(minVal * WTSIZE);
//        pPlayback->setReadingLength(differenceVal * WTSIZE);
//    }
//    repaint();
}


void EventHandler::handleButtonClicked(juce::Button* button) {
//    if (button == &mCommitButton){
//        handleCommitButton();
//    } else if (button == &mSaveButton){
//        pFileHandler->exportFiles(mOrigAudioData,
//                                  mResampledCycles,
//                                  mPolarCycles,
//                                  mResynthesizedCycles,
//                                  mOriginalFileName);
//
//        mEventConfirmationLabel.setText("File Saved!", juce::dontSendNotification);
//        mEventConfirmationLabel.setVisible(true);
//    } else if (button == &mClearButton){
//        mResampledCycles.clear();
//        mPolarCycles.clear();
//
//        for(int i = 0; i < N_WT_BANDS; i++){
//            mResynthesizedCycles[i].clear();
//        }
//
//        updateLengthInfoLabel();
//        mVectorThatShowsWhichSamplesAreCommitted.assign(mVectorThatShowsWhichSamplesAreCommitted.size(), false);
//        mVectorThatShowsWhichSamplesAreCommitted.clear();
//
//        mModeOrigButton.setToggleState(true, juce::sendNotification);
//
//    } else if (button == &mDeleteButton){
//    } else if (button == &mPlayButton){
//        bool playButtonState = mPlayButton.getToggleState();
//
//        if(playButtonState){
//            mPlaybackState = PlaybackStates::Playing;
//        } else {
//            mPlaybackState = PlaybackStates::Stopped;
//        }
//    } else if(button == &mModeOrigButton){
//        mModeOrigButton.setToggleState(true, juce::dontSendNotification);
//        mModeResampledButton.setToggleState(false, juce::dontSendNotification);
//        mModeResynthesizedButton.setToggleState(false, juce::dontSendNotification);
//        mLargeWaveform.setAudioVector(&mOrigAudioData,
//                                      mStartSampleIndex,
//                                      mCycleLenHint * 2,
//                                      true);
//        mSmallWaveform.setAudioVector(&mOrigAudioData,
//                                      0,
//                                      static_cast<int>(mOrigAudioData.size()),
//                                      false);
//        pPlayback->setAudioVector(&mOrigAudioData, 0, static_cast<int>(mOrigAudioData.size()));
//
//        mResampledZoomSlider.setVisible(false);
//        mResampledZoomSliderLabel.setVisible(false);
//        mCycleLenHintSlider.setVisible(true);
//        mCycleLenHintSliderLabel.setVisible(true);
//        mBandSlider.setVisible(false);
//        mBandSliderLabel.setVisible(false);
//        mStartSampleIndexSlider.setVisible(true);
//
//        repaint();
//    } else if(button == &mModeResampledButton){
//        mModeOrigButton.setToggleState(false, juce::dontSendNotification);
//        mModeResampledButton.setToggleState(true, juce::dontSendNotification);
//        mModeResynthesizedButton.setToggleState(false, juce::dontSendNotification);
//
//        mLargeWaveform.setAudioVector(&mResampledCycles,
//                                      mResampledZoomSlider.getMinValue() * WTSIZE,
//                                      mResampledZoomSlider.getMaxValue() * WTSIZE,
//                                      false);
//        mSmallWaveform.setAudioVector(&mResampledCycles,
//                                      0,
//                                      static_cast<int>(mResampledCycles.size()),
//                                      false);
//
//        pPlayback->setAudioVector(&mResampledCycles,
//                                  mResampledZoomSlider.getMinValue() * WTSIZE,
//                                  (mResampledZoomSlider.getMaxValue() - mResampledZoomSlider.getMinValue()) * WTSIZE);
//
//        mResampledZoomSlider.setVisible(true);
//        mResampledZoomSliderLabel.setVisible(true);
//        mCycleLenHintSlider.setVisible(false);
//        mCycleLenHintSliderLabel.setVisible(false);
//        mBandSlider.setVisible(false);
//        mBandSliderLabel.setVisible(false);
//        mStartSampleIndexSlider.setVisible(false);
//
//        repaint();
//    } else if(button == &mModeResynthesizedButton){
//        mModeOrigButton.setToggleState(false, juce::dontSendNotification);
//        mModeResampledButton.setToggleState(false, juce::dontSendNotification);
//        mModeResynthesizedButton.setToggleState(true, juce::dontSendNotification);
//
//        mLargeWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                      mResampledZoomSlider.getMinValue() * WTSIZE,
//                                      mResampledZoomSlider.getMaxValue() * WTSIZE,
//                                      false);
//        mSmallWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                      0,
//                                      static_cast<int>(mResynthesizedCycles[mSelectedBand].size()),
//                                      false);
//
//        pPlayback->setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                  mResampledZoomSlider.getMinValue() * WTSIZE,
//                                  (mResampledZoomSlider.getMaxValue() - mResampledZoomSlider.getMinValue()) * WTSIZE);
//
//        mResampledZoomSlider.setVisible(true);
//        mResampledZoomSliderLabel.setVisible(true);
//        mCycleLenHintSlider.setVisible(false);
//        mCycleLenHintSliderLabel.setVisible(false);
//        mBandSlider.setVisible(true);
//        mBandSliderLabel.setVisible(true);
//        mStartSampleIndexSlider.setVisible(false);
//
//        repaint();
//    } else if(button == &mPrevCycleButton){
//        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
//        int newVal = mStartSampleIndexSlider.getValue() - cycleLen;
//        if(newVal < 0)
//            newVal = 0;
//        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
//    } else if(button == &mPrevSampleButton){
//        int newVal = mStartSampleIndexSlider.getValue() - 2;
//        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
//    } else if(button == &mNextSampleButton){
//        int newVal = mStartSampleIndexSlider.getValue() + 2;
//        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
//    } else if(button == &mNextCycleButton){
//        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
//        int newVal = mStartSampleIndexSlider.getValue() + cycleLen;
//        if(newVal > mOrigAudioData.size() - cycleLen)
//            newVal = static_cast<int>(mOrigAudioData.size()) - cycleLen;
//        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
//    }
}


void EventHandler::handleComboBoxChanged(juce::ComboBox* box) {
//    if (box == &mCycleLengthComboBox)
//    {
//        int newValue = box->getText().getIntValue();
//        std::cout << "New WTSIZE = " << newValue << std::endl;
//        WTSIZE = newValue;
//    }
}
