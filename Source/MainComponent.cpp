#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():
    mLargeWaveform(mDataModel.getReferenceForOrigAudioDataVector(),
                   mDataModel.getReferenceForZeroCrossingsVector(),
                   mDataModel.getReferenceForVectorThatShowsWhichSamplesAreCommitted(),
                   mDataModel.getReferenceOfClosestZeroCrossingStart(),
                   mDataModel.getReferenceOfClosestZeroCrossingEnd(),
                   0,
                   500),
    mSmallWaveform(mDataModel.getReferenceForOrigAudioDataVector(), 0, 0),

//    mDragDropAreaOriginal("Original",
//                          mOrigAudioData,
//                          mOriginalFileName,
//                          [this](bool isResampled) {
//                              this->newFileWasDropped(isResampled);
//                          }),
//    mDragDropAreaResampled("Resampled",
//                           mResampledCycles,
//                           mOriginalFileName,
//                           [this](bool isResampled) {
//                               this->newFileWasDropped(isResampled);
//                           }),

    mUI(this,
        mDataModel.getSizeOfOrigAudioData(),
        mDataModel.getCycleLenHint()),

    pPlayback(std::make_unique<Playback>(mDataModel.getReferenceForOrigAudioDataVector(), 0, 0))
{
    setSize (800, 600);

    // Enable drag-and-drop
    setInterceptsMouseClicks (true, true);
    setWantsKeyboardFocus (true);
    addKeyListener(this);
    
    // read audio file and display waveform
    // /Users/mihaitraista/5.Sound Libraries/fl1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    
    mDataModel.readAudioFileAndCopyToVector("/Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav");
    
    updateVectors();
    
    addAndMakeVisible(mLargeWaveform);
    addAndMakeVisible(mSmallWaveform);
//    addAndMakeVisible(mDragDropAreaOriginal);
//    addAndMakeVisible(mDragDropAreaResampled);

    addAndMakeVisible(mUI);
    
    // ask for audio input permission
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}



void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if(mPlaybackState == PlaybackStates::Playing)
        pPlayback->readSamplesFromVector(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//    juce::Image titleImage = juce::ImageCache::getFromMemory(BinaryData::BLResampler_Title2_png, BinaryData::BLResampler_Title2_pngSize);
//
//    juce::Rectangle<float> titleImageArea (200, 20, 280, 16);
//    g.drawImage(titleImage, titleImageArea, juce::RectanglePlacement::stretchToFit);    

    grabKeyboardFocus();
}

void MainComponent::resized()
{
    float aAreaY = 0;
    float bAreaY = 150;
    float cAreaY = 550;
    float aAreaHeight = 150;
    float bAreaHeight = 400;
    float cAreaHeight = 50;
    float width = getWidth();
    float height = getHeight();
    float gap = 5;
    float buttonsYOffset = 50;
    float bigButtonWidth = 86;
    float bigButtonHeight = 30;
    
    mLargeWaveform.setBounds(0, bAreaY, width, bAreaHeight);
    mSmallWaveform.setBounds(0, cAreaY, width, cAreaHeight);
    
//    mDragDropAreaOriginal.setBounds(553, buttonsYOffset - 45, 116, 40);
//    mDragDropAreaResampled.setBounds(673, buttonsYOffset - 45, 116, 40);
    
    mUI.setBounds(0, 0, width, height);
}

bool MainComponent::keyPressed(const juce::KeyPress& key, Component* originatingComponent){
    if (key.getTextCharacter() == 'C' || key.getTextCharacter() == 'c')
    {
        handleCommitButton();
        return true;
    } else if (key.getTextCharacter() == 'H' || key.getTextCharacter() == 'h'){
//        mPrevCycleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'L' || key.getTextCharacter() == 'l'){
//        mNextCycleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'J' || key.getTextCharacter() == 'j'){
//        mPrevSampleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'K' || key.getTextCharacter() == 'k'){
//        mNextSampleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'I' || key.getTextCharacter() == 'i'){
//        int newVal = mCycleLenHintSlider.getValue() - 4;
//        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'U' || key.getTextCharacter() == 'u'){
//        int newVal = mCycleLenHintSlider.getValue() + 4;
//        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    }
    return false;
}

void MainComponent::handleCommitButton(){
//    const float* mAudioBufferData = mOrigAudioData.data();
//
//    int originalLengthOfCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
//
//    if(originalLengthOfCycle < 3)
//        return;
//
//    // set to true all indexes from the original file that are going to be used
//    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
//        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
//    }
//
//    // make a temp origCycle and copy the sampled from orig file
//    std::vector<float> origCycle(originalLengthOfCycle, 0.0f);
//    for(int i = 0; i < originalLengthOfCycle; ++i){
//        origCycle[i] = mAudioBufferData[i + mClosestZeroCrossingStart];
//    }
//
//    // create the three temp vectors
//    std::vector<float> resampled = std::vector<float>(WTSIZE, 0.0f);
//
//    pResampler->resizeCycle(origCycle, resampled);
//
//    mResampledCycles.insert(mResampledCycles.end(), resampled.begin(), resampled.end());
//
//    addResynthesizedCycle(resampled);
//
//    mUI.setRangeOfResampledZoomSlider(mResampledCycles.size() / WTSIZE);
//    updateLengthInfoLabel();
//    mUI.setEventConfirmationLabelTextAndVisibility("Cycle Committed!", true);
//
//    repaint();
}

void MainComponent::addResynthesizedCycle(const std::vector<float>& resampledCycle){
//    std::vector<float> polarValues = std::vector<float>(WTSIZE * 2, 0.0f);
//    std::vector<float> resynthesized = std::vector<float>(WTSIZE, 0.0f);
//
//    Fourier::fillDftPolar(resampledCycle, polarValues);
//
//    // harmonicsLimit = 512 because Niquist(22050) / freq(43) = 512 (1024 samples is 43 Hz)
//
//    for(int band = 0; band < N_WT_BANDS; band++){
//        float harmonicsLimit = 22050.0f / baseFrequencies[band];
//
//        Fourier::idft(polarValues, resynthesized, harmonicsLimit, 50.0f, 1);
//
//        mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(), resynthesized.begin(), resynthesized.end());
//    }
//
//    mPolarCycles.insert(mPolarCycles.end(), polarValues.begin(), polarValues.end());
}

void MainComponent::newFileWasDropped(bool isResampled){
//    std::cout << "newFileWasDropped message from parent class - >" << isResampled << std::endl;
//
//    if(isResampled){
//        // clear all vectors, except mResampledCycles which has just been filled by DragDropArea
//        mOrigAudioData.clear();
//        mPolarCycles.clear();
//        for(int i = 0; i < N_WT_BANDS; i++){
//            mResynthesizedCycles[i].clear();
//        }
//
//        std::vector<float> resampledCycle = std::vector<float>(WTSIZE, 0.0f);
//
//        size_t nCycles = mResampledCycles.size() / WTSIZE;
//
//        for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
//            for(int i = 0; i < WTSIZE; i++){
//                resampledCycle[i] = mResampledCycles[cycleIndex * WTSIZE + i];
//            }
//
//            addResynthesizedCycle(resampledCycle);
//        }
//
//        mUI.setRangeOfResampledZoomSlider(nCycles);
//        updateLengthInfoLabel();
//        mUI.setEventConfirmationLabelTextAndVisibility("New Resynthesized Cycles Added!", true);
//
//    } else {
//        calculateZeroCrossingsAndUpdateVectors();
//    }
//
//    repaint();
}

void MainComponent::updateLengthInfoLabel(){
    int len = mDataModel.getSizeOfResampledCycles();
    juce::String labelText = "Resampled Buffer Length: " + juce::String(len) + " samples, " + juce::String(len / static_cast<float>(WTSIZE)) + " cycles";
    mUI.setTextForResampledLengthLabel(labelText);
}

void MainComponent::updateVectors(){
    mDataModel.calculateZeroCrossingsAndUpdateVectors();
    
    pPlayback->setReadingStart(0);
    pPlayback->setReadingLength(mDataModel.getSizeOfOrigAudioData());
    
    mSmallWaveform.setDisplayLengthInSamples(mDataModel.getSizeOfOrigAudioData());
    mLargeWaveform.setDisplayLengthInSamples(DEFAULT_CYCLE_LEN_HINT * 2);
    
    int rangeOfSlider = mDataModel.getSizeOfOrigAudioData() - mDataModel.getCycleLenHint() * 2;
    mUI.setRangeOfStartSampleIndexSlider(rangeOfSlider , true);
}

void MainComponent::handleSliderValueChanged(juce::Slider* slider) {
//    juce::String id = slider->getComponentID();
//    if (id == "mStartSampleIndexSlider"){
//        mStartSampleIndex = mUI.getStartSampleIndexSliderValue();
//        mLargeWaveform.setDisplayStartSample(mStartSampleIndex);
//        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
//                                                                    mClosestZeroCrossingStart,
//                                                                    mClosestZeroCrossingEnd,
//                                                                    mStartSampleIndex,
//                                                                    mCycleLenHint);
//        mUI.setEventConfirmationLabelTextAndVisibility("hello", false);
//    } else if (id == "mCycleLenHintSlider"){
//        // update the mStartSampleIndex so that we keep the waveform centered while zooming
//
//        int newHint = mUI.getCycleLenHintSliderValue();
//        int diff = newHint - mCycleLenHint;
//        int halfDiff = diff / 2.0f;
//
//        mLargeWaveform.setDisplayLengthInSamples(newHint * 2);
//
//        mUI.setRangeOfStartSampleIndexSlider(mOrigAudioData.size() - newHint * 2, false);
//        
//        // update mCycleLenHint and find new zeroCrossings start and end points
//        mCycleLenHint = mUI.getCycleLenHintSliderValue();
//        
//        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
//                                                                    mClosestZeroCrossingStart,
//                                                                    mClosestZeroCrossingEnd,
//                                                                    mStartSampleIndex,
//                                                                    mCycleLenHint);
//
//        mUI.setEventConfirmationLabelTextAndVisibility("hello", false);
//    } else if (id == "mBandSlider"){
//        int band = mUI.getBandSliderValue();
//        
//        mSelectedBand = band;
//        
//        if(mUI.getModeResynthesizedButtonState()){
//            mLargeWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                          mUI.getResampledZoomSliderMin() * WTSIZE,
//                                          mUI.getResampledZoomSliderMax() * WTSIZE,
//                                          false);
//            mLargeWaveform.setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                          0,
//                                          static_cast<int>(mResynthesizedCycles[mSelectedBand].size()),
//                                          false);
//            pPlayback->setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                      mUI.getResampledZoomSliderMin() * WTSIZE,
//                                      (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE);
//        }
//    } else if (id == "mResampledZoomSlider"){
//        // we don't want to change the display length in ModeOriginal
//        
//        if(mUI.getModeOrigButtonState())
//            return;
//
//        int minVal = mUI.getResampledZoomSliderMin();
//        int maxVal = mUI.getResampledZoomSliderMax();
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


void MainComponent::handleButtonClicked(juce::Button* button) {
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


void MainComponent::handleComboBoxChanged(juce::ComboBox* box) {
//    if (box == &mCycleLengthComboBox)
//    {
//        int newValue = box->getText().getIntValue();
//        std::cout << "New WTSIZE = " << newValue << std::endl;
//        WTSIZE = newValue;
//    }
}
