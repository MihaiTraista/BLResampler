#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():
    mLargeWaveform(&mOrigAudioData,
                     &mZeroCrossings,
                     &mVectorThatShowsWhichSamplesAreCommitted,
                     0,
                     500,
                     &mClosestZeroCrossingStart,
                     &mClosestZeroCrossingEnd),
    mSmallWaveform(&mOrigAudioData, 0, 0),

    mDragDropAreaOriginal("Original",
                          mOrigAudioData,
                          mOriginalFileName,
                          [this](bool isResampled) {
                              this->newFileWasDropped(isResampled);
                          }),
    mDragDropAreaResampled("Resampled",
                           mResampledCycles,
                           mOriginalFileName,
                           [this](bool isResampled) {
                               this->newFileWasDropped(isResampled);
                           }),

    mUI(&mEventHandler, mOrigAudioData, mResampledCycles, mPolarCycles, mCycleLenHint),

    pPlayback(std::make_unique<Playback>(&mOrigAudioData, 0, 0))
{
    setSize (800, 600);

    // Enable drag-and-drop
    setInterceptsMouseClicks (true, true);
    setWantsKeyboardFocus (true);
    addKeyListener(this);
    
    addAndMakeVisible(mLargeWaveform);
    addAndMakeVisible(mSmallWaveform);
    addAndMakeVisible(mDragDropAreaOriginal);
    addAndMakeVisible(mDragDropAreaResampled);

    addAndMakeVisible(mUI);
    
    // read audio file and display waveform
    // /Users/mihaitraista/5.Sound Libraries/fl1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    juce::File audioFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav");

    // this will be updated when the user drags and drops a file
    mOriginalFileName = audioFile.getFileName();
    
    pFileHandler->readAudioFileAndCopyToVector(audioFile, mOrigAudioData);

    calculateZeroCrossingsAndUpdateVectors();
    
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
    
    mDragDropAreaOriginal.setBounds(553, buttonsYOffset - 45, 116, 40);
    mDragDropAreaResampled.setBounds(673, buttonsYOffset - 45, 116, 40);
    
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
    const float* mAudioBufferData = mOrigAudioData.data();

    int originalLengthOfCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
    
    if(originalLengthOfCycle < 3)
        return;

    // set to true all indexed from the original file that are going to be used
    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
    }

    // make a temp origCycle and copy the sampled from orig file
    std::vector<float> origCycle(originalLengthOfCycle, 0.0f);
    for(int i = 0; i < originalLengthOfCycle; ++i){
        origCycle[i] = mAudioBufferData[i + mClosestZeroCrossingStart];
    }
    
    /* WIP start ---------------

    std::vector<float> origCycle(1024, 0.0f);
    juce::File audioFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/BLResampler/Resources/cello-res-dephased.wav");

    juce::AudioBuffer<float> tempBuff;
    pFileHandler->storeAudioFileInBuffer(audioFile, tempBuff);

    const float * tempBuffData = tempBuff.getReadPointer(0);
    std::cout << "duration = " << tempBuff.getNumSamples() << std::endl;

    for(int i = 0; i < 1024; ++i){
        origCycle[i] = tempBuffData[i];
    }

    mClosestZeroCrossingStart = 0;
    mClosestZeroCrossingEnd = 1023;

    WIP end --------------- */

    // create the three temp vectors
    std::vector<float> resampled = std::vector<float>(WTSIZE, 0.0f);

    pResampler->resizeCycle(origCycle, resampled);
    
    mResampledCycles.insert(mResampledCycles.end(), resampled.begin(), resampled.end());

    addResynthesizedCycle(resampled);

//    mResampledZoomSlider.setRange(0, mResampledCycles.size() / WTSIZE, 1);
//    mResampledZoomSlider.setMaxValue(mResampledCycles.size() / WTSIZE, juce::dontSendNotification);

    updateLengthInfoLabel();

//    mEventConfirmationLabel.setText("Cycle Committed!", juce::dontSendNotification);
//    mEventConfirmationLabel.setVisible(true);

    repaint();
}

void MainComponent::addResynthesizedCycle(const std::vector<float>& resampledCycle){
    std::vector<float> polarValues = std::vector<float>(WTSIZE * 2, 0.0f);
    std::vector<float> resynthesized = std::vector<float>(WTSIZE, 0.0f);

    Fourier::fillDftPolar(resampledCycle, polarValues);

    // harmonicsLimit = 512 because Niquist(22050) / freq(43) = 512 (1024 samples is 43 Hz)

    for(int band = 0; band < N_WT_BANDS; band++){
        float harmonicsLimit = 22050.0f / baseFrequencies[band];

        Fourier::idft(polarValues, resynthesized, harmonicsLimit, 50.0f, 1);
        
        mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(), resynthesized.begin(), resynthesized.end());
    }

    mPolarCycles.insert(mPolarCycles.end(), polarValues.begin(), polarValues.end());
}

void MainComponent::newFileWasDropped(bool isResampled){
    std::cout << "newFileWasDropped message from parent class - >" << isResampled << std::endl;

    if(isResampled){
        // clear all vectors, except mResampledCycles which has just been filled by DragDropArea
        mOrigAudioData.clear();
        mPolarCycles.clear();
        for(int i = 0; i < N_WT_BANDS; i++){
            mResynthesizedCycles[i].clear();
        }
        
        std::vector<float> resampledCycle = std::vector<float>(WTSIZE, 0.0f);
        
        size_t nCycles = mResampledCycles.size() / WTSIZE;

        for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
            for(int i = 0; i < WTSIZE; i++){
                resampledCycle[i] = mResampledCycles[cycleIndex * WTSIZE + i];
            }
            
            addResynthesizedCycle(resampledCycle);
        }

//        mResampledZoomSlider.setRange(0, nCycles, 1);
//        mResampledZoomSlider.setMaxValue(nCycles, juce::dontSendNotification);

        updateLengthInfoLabel();

//        mEventConfirmationLabel.setText("New Resynthesized Cycles Added!", juce::dontSendNotification);
//        mEventConfirmationLabel.setVisible(true);
    } else {
        calculateZeroCrossingsAndUpdateVectors();
    }
    
    repaint();
}

void MainComponent::updateLengthInfoLabel(){
    int len = static_cast<int>(mResampledCycles.size());
    juce::String labelText = "Resampled Buffer Length: " + juce::String(len) + " samples, " + juce::String(len / static_cast<float>(WTSIZE)) + " cycles";
//    mResampledLengthLabel.setText(labelText, juce::dontSendNotification);
}

void MainComponent::calculateZeroCrossingsAndUpdateVectors(){
    mClosestZeroCrossingStart = 0;
    mClosestZeroCrossingEnd = 0;
    mSelectedBand = 0;
    mStartSampleIndex = 0;
    mCycleLenHint = DEFAULT_CYCLE_LEN_HINT;

    mZeroCrossings.clear();
    mVectorThatShowsWhichSamplesAreCommitted.resize(mOrigAudioData.size(), false);

    mResampledCycles.clear();
    mPolarCycles.clear();
    
    for(int i = 0; i < N_WT_BANDS; i++){
        mResynthesizedCycles[i].clear();
    }

    pPlayback->setReadingStart(0);
    pPlayback->setReadingLength(static_cast<int>(mOrigAudioData.size()));
    
    mSmallWaveform.setDisplayLengthInSamples(static_cast<int>(mOrigAudioData.size()));
    mLargeWaveform.setDisplayLengthInSamples(DEFAULT_CYCLE_LEN_HINT * 2);
    
    pZeroCrossingFinder->calculateZeroCrossings(mOrigAudioData, mZeroCrossings);

    pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                mClosestZeroCrossingStart,
                                                                mClosestZeroCrossingEnd,
                                                                mStartSampleIndex,
                                                                mCycleLenHint);

//    mStartSampleIndexSlider.setRange(0, mOrigAudioData.size() - mCycleLenHint * 2);
//    mStartSampleIndexSlider.setValue(mOrigAudioData.size() / 2, juce::sendNotification);
//    mStartSampleIndexSlider.setValue(DEFAULT_CYCLE_LEN_HINT, juce::sendNotification);
}
