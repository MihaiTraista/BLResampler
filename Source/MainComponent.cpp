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
        mUI.triggerClickPrevCycleButton();
        return true;
    } else if (key.getTextCharacter() == 'L' || key.getTextCharacter() == 'l'){
        mUI.triggerClickNextCycleButton();
        return true;
    } else if (key.getTextCharacter() == 'J' || key.getTextCharacter() == 'j'){
        mUI.triggerClickPrevSampleButton();
        return true;
    } else if (key.getTextCharacter() == 'K' || key.getTextCharacter() == 'k'){
        mUI.triggerClickNextSampleButton();
        return true;
    } else if (key.getTextCharacter() == 'I' || key.getTextCharacter() == 'i'){
        mUI.zoomOutOneUnit();
        return true;
    } else if (key.getTextCharacter() == 'U' || key.getTextCharacter() == 'u'){
        mUI.zoomInOneUnit();
        return true;
    }
    return false;
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

void MainComponent::handleCommitButton(){
    mDataModel.commit();

    mUI.setRangeOfResampledZoomSlider(mDataModel.getSizeOfResampledCycles() / WTSIZE);
    updateLengthInfoLabel();
    mUI.setEventConfirmationLabelTextAndVisibility("Cycle Committed!", true);

    repaint();
}

void MainComponent::handleSliderValueChanged(juce::Slider* slider) {
    juce::String id = slider->getComponentID();
    
    if (id == "mStartSampleIndexSlider"){
        int startSample = mUI.getStartSampleIndexSliderValue();
        mDataModel.setStartSampleIndex(startSample);
        mDataModel.findClosestZeroCrossings();

        mLargeWaveform.setDisplayStartSample(startSample);

        mUI.setEventConfirmationLabelTextAndVisibility("hello", false);
    }else if (id == "mCycleLenHintSlider"){
        // update the mStartSampleIndex so that we keep the waveform centered while zooming

        int newHint = mUI.getCycleLenHintSliderValue();
        int diff = newHint - mDataModel.getCycleLenHint();
        int halfDiff = diff / 2.0f;

        mDataModel.setCycleLenHint(newHint);
        
        mLargeWaveform.setDisplayLengthInSamples(newHint * 2);

        mUI.setRangeOfStartSampleIndexSlider(mDataModel.getSizeOfOrigAudioData() - newHint * 2, false);

        mDataModel.findClosestZeroCrossings();

        mUI.setEventConfirmationLabelTextAndVisibility("hello", false);
    } else if (id == "mBandSlider"){
        int band = mUI.getBandSliderValue();
        
        mDataModel.setSelectedBand(band);
        
        if(mUI.getModeResynthesizedButtonState()){
            mLargeWaveform.setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                          mUI.getResampledZoomSliderMin() * WTSIZE,
                                          mUI.getResampledZoomSliderMax() * WTSIZE,
                                          false);
            mLargeWaveform.setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                          0,
                                          mDataModel.getSizeOfResynthesizedCycles(),
                                          false);
//            pPlayback->setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                      mUI.getResampledZoomSliderMin() * WTSIZE,
//                                      (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE);
        }
    } else if (id == "mResampledZoomSlider"){

        int minVal = mUI.getResampledZoomSliderMin();
        int maxVal = mUI.getResampledZoomSliderMax();
        int differenceVal = maxVal - minVal;
        
        mLargeWaveform.setDisplayStartSample(minVal * WTSIZE);
        mLargeWaveform.setDisplayLengthInSamples(differenceVal * WTSIZE);

        pPlayback->setReadingStart(minVal * WTSIZE);
        pPlayback->setReadingLength(differenceVal * WTSIZE);
    }
    repaint();
}


void MainComponent::handleButtonClicked(juce::Button* button) {
    juce::String id = button->getComponentID();
    
    if (id == "mCommitButton"){
        handleCommitButton();
    } else if (id == "mSaveButton"){
//        pFileHandler->exportFiles(mOrigAudioData,
//                                  mResampledCycles,
//                                  mPolarCycles,
//                                  mResynthesizedCycles,
//                                  mOriginalFileName);
//
//        mEventConfirmationLabel.setText("File Saved!", juce::dontSendNotification);
//        mEventConfirmationLabel.setVisible(true);
    } else if (id == "mClearButton"){
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

    } else if (id == "mDeleteButton"){
    } else if (id == "mPlayButton"){
//        bool playButtonState = mPlayButton.getToggleState();
//
//        if(playButtonState){
//            mPlaybackState = PlaybackStates::Playing;
//        } else {
//            mPlaybackState = PlaybackStates::Stopped;
//        }
    } else if(id == "mModeOrigButton"){
        
        mUI.setMode(UI::Modes::ORIG);

        mLargeWaveform.setAudioVector(mDataModel.getReferenceForOrigAudioDataVector(),
                                      mDataModel.getStartSampleIndex(),
                                      mDataModel.getCycleLenHint() * 2,
                                      true);
        mSmallWaveform.setAudioVector(mDataModel.getReferenceForOrigAudioDataVector(),
                                      0,
                                      mDataModel.getSizeOfOrigAudioData(),
                                      false);
//        pPlayback->setAudioVector(&mOrigAudioData, 0, static_cast<int>(mOrigAudioData.size()));

    } else if(id == "mModeResampledButton"){

        mUI.setMode(UI::Modes::RESAMPLED);
        mLargeWaveform.setAudioVector(mDataModel.getReferenceForResampledCyclesVector(),
                                      mUI.getResampledZoomSliderMin() * WTSIZE,
                                      mUI.getResampledZoomSliderMax() * WTSIZE,
                                      false);
        mSmallWaveform.setAudioVector(mDataModel.getReferenceForResampledCyclesVector(),
                                      0,
                                      mDataModel.getSizeOfResampledCycles(),
                                      false);

//        pPlayback->setAudioVector(&mResampledCycles,
//                                  mResampledZoomSlider.getMinValue() * WTSIZE,
//                                  (mResampledZoomSlider.getMaxValue() - mResampledZoomSlider.getMinValue()) * WTSIZE);
//
    } else if(id == "mModeResynthesizedButton"){
        mUI.setMode(UI::Modes::RESYNTHESIZED);

        mLargeWaveform.setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                      mUI.getResampledZoomSliderMin() * WTSIZE,
                                      mUI.getResampledZoomSliderMax() * WTSIZE,
                                      false);
        mLargeWaveform.setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                      0,
                                      mDataModel.getSizeOfResynthesizedCycles(),
                                      false);

//        pPlayback->setAudioVector(&mResynthesizedCycles[mSelectedBand],
//                                  mResampledZoomSlider.getMinValue() * WTSIZE,
//                                  (mResampledZoomSlider.getMaxValue() - mResampledZoomSlider.getMinValue()) * WTSIZE);

    } else if(id == "mPrevCycleButton"){
        mUI.goToPrevCycle(mDataModel.getActualCycleLen());        
    } else if(id == "mPrevSampleButton"){
        mUI.goToPrevSample();
    } else if(id == "mNextSampleButton"){
        mUI.goToNextSample();
    } else if(id == "mNextCycleButton"){
        mUI.goToNextCycle(mDataModel.getActualCycleLen(), mDataModel.getSizeOfOrigAudioData());
    }
    repaint();
}


void MainComponent::handleComboBoxChanged(juce::ComboBox* box) {
//    if (box == &mCycleLengthComboBox)
//    {
//        int newValue = box->getText().getIntValue();
//        std::cout << "New WTSIZE = " << newValue << std::endl;
//        WTSIZE = newValue;
//    }
}
