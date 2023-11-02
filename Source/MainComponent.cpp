#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():
    mUI(this,
        mDataModel.getReferenceForOrigAudioDataVector(),
        mDataModel.getReferenceForZeroCrossingsVector(),
        mDataModel.getReferenceForVectorThatShowsWhichSamplesAreCommitted(),
        mDataModel.getReferenceOfClosestZeroCrossingStart(),
        mDataModel.getReferenceOfClosestZeroCrossingEnd(),
        mDataModel.getSizeOfOrigAudioData(),
        mDataModel.getCycleLenHint(),
        [this](juce::File audioFile, bool isResampled) {
            this->newFileWasDropped(audioFile, isResampled);
        }),

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
    
    juce::File defaultFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav");

    mDataModel.readFileAndStoreDataInOrigAudioData(defaultFile);
    
    updateVectors();
    
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
    mUI.setBounds(0, 0, getWidth(), getHeight());
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

void MainComponent::newFileWasDropped(juce::File audioFile, bool isResampled){
    std::cout << "newFileWasDropped message from parent class - >" << isResampled << std::endl;
            
    if(isResampled){
        mDataModel.readFileAndStoreDataInResampledCycles(audioFile);

        int nCycles = mDataModel.clearVectorsAndResynthesizeAllCycles();
        
        mUI.setRangeOfResampledZoomSlider(nCycles);
        updateLengthInfoLabel();
        
        mUI.triggerClickModeResampled();
        
        mUI.setEventConfirmationLabelTextAndVisibility("New Resynthesized Cycles Added!", true);
    } else {
        mDataModel.readFileAndStoreDataInOrigAudioData(audioFile);

        mDataModel.setOrigFileName(audioFile.getFileName());

        updateVectors();
        
        mUI.triggerClickModeOrig();
    }

    repaint();
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
    
    int rangeOfSlider = mDataModel.getSizeOfOrigAudioData() - mDataModel.getCycleLenHint() * 2;
    mUI.setRangeOfStartSampleIndexSlider(rangeOfSlider , true);

    mUI.setSmallWaveformLength(mDataModel.getSizeOfOrigAudioData());
    mUI.setLargeWaveformLength(DEFAULT_CYCLE_LEN_HINT * 2);
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

        mUI.setLargeWaveformStart(startSample);

        mUI.setEventConfirmationLabelTextAndVisibility("", false);
    }else if (id == "mCycleLenHintSlider"){
        // update the mStartSampleIndex so that we keep the waveform centered while zooming

        int newHint = mUI.getCycleLenHintSliderValue();
        int diff = newHint - mDataModel.getCycleLenHint();
        int halfDiff = diff / 2.0f;
        int currentStart = mUI.getLargeWaveformStartSampleIndex();
        int updatedStart = currentStart - halfDiff;
        
        mUI.setStartSampleIndexSlider(updatedStart);
        
        mDataModel.setCycleLenHint(newHint);

        mUI.setLargeWaveformLength(newHint * 2);

        mUI.setRangeOfStartSampleIndexSlider(mDataModel.getSizeOfOrigAudioData() - newHint * 2, false);

        mDataModel.findClosestZeroCrossings();

        mUI.setEventConfirmationLabelTextAndVisibility("", false);
    } else if (id == "mBandSlider" && mUI.getMode() != UI::Modes::ORIG){
        int band = mUI.getBandSliderValue();
        
        mDataModel.setSelectedBand(band);
        
        if(mUI.getModeResynthesizedButtonState()){
            mUI.setLargeWaveformVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                       mUI.getResampledZoomSliderMin() * WTSIZE,
                                       (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE,
                                       false);

            
            pPlayback->setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                      mUI.getResampledZoomSliderMin() * WTSIZE,
                                      (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE);
        }
    } else if (id == "mResampledZoomSlider" && mUI.getMode() != UI::Modes::ORIG){

        int minVal = mUI.getResampledZoomSliderMin();
        int maxVal = mUI.getResampledZoomSliderMax();
        int differenceVal = maxVal - minVal;
        
        mUI.setLargeWaveformStart(minVal * WTSIZE);
        mUI.setLargeWaveformLength(differenceVal * WTSIZE);
        
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
        mDataModel.exportFiles();
        mUI.setEventConfirmationLabelTextAndVisibility("File Saved!", true);
    } else if (id == "mClearButton"){
        mDataModel.clearAllResampledAndResynthesized();

        updateLengthInfoLabel();

        mUI.triggerClickModeOrig();

    } else if (id == "mDeleteButton"){
    } else if (id == "mPlayButton"){
        bool playButtonState = mUI.getPlayButtonToggleState();
        
        if(playButtonState){
            mPlaybackState = PlaybackStates::Playing;
        } else {
            mPlaybackState = PlaybackStates::Stopped;
        }
    } else if(id == "mModeOrigButton"){
        
        mUI.setMode(UI::Modes::ORIG);

        mUI.setLargeWaveformVector(mDataModel.getReferenceForOrigAudioDataVector(),
                                   mDataModel.getStartSampleIndex(),
                                   mDataModel.getCycleLenHint() * 2,
                                   true);

        mUI.setSmallWaveformVector(mDataModel.getReferenceForOrigAudioDataVector(),
                                   0,
                                   mDataModel.getSizeOfOrigAudioData(),
                                   false);

        pPlayback->setAudioVector(mDataModel.getReferenceForOrigAudioDataVector(), 0, mDataModel.getSizeOfOrigAudioData());

    } else if(id == "mModeResampledButton"){

        mUI.setMode(UI::Modes::RESAMPLED);
        mUI.setLargeWaveformVector(mDataModel.getReferenceForResampledCyclesVector(),
                                   mUI.getResampledZoomSliderMin() * WTSIZE,
                                   mUI.getResampledZoomSliderMax() * WTSIZE,
                                   false);
        mUI.setSmallWaveformVector(mDataModel.getReferenceForResampledCyclesVector(),
                                   0,
                                   mDataModel.getSizeOfResampledCycles(),
                                   false);

        pPlayback->setAudioVector(mDataModel.getReferenceForResampledCyclesVector(),
                                  mUI.getResampledZoomSliderMin() * WTSIZE,
                                  (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE);
//
    } else if(id == "mModeResynthesizedButton"){
        mUI.setMode(UI::Modes::RESYNTHESIZED);

        mUI.setLargeWaveformVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                   mUI.getResampledZoomSliderMin() * WTSIZE,
                                   mUI.getResampledZoomSliderMax() * WTSIZE,
                                   false);
        mUI.setSmallWaveformVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                   0,
                                   mDataModel.getSizeOfResynthesizedCycles(),
                                   false);

        pPlayback->setAudioVector(mDataModel.getReferenceForResynthesizedCyclesVector(),
                                  mUI.getResampledZoomSliderMin() * WTSIZE,
                                  (mUI.getResampledZoomSliderMax() - mUI.getResampledZoomSliderMin()) * WTSIZE);

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
