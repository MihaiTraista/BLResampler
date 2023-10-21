#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():
    mWaveformDisplay(&mAudioFileBuffer,
                     &mZeroCrossings,
                     &mVectorThatShowsWhichSamplesAreCommitted,
                     &mStartSampleIndex,
                     &mCycleLenHint,
                     &mClosestZeroCrossingStart,
                     &mClosestZeroCrossingEnd),
    mOriginalWaveform(&mAudioFileBuffer)
{
    setSize (800, 600);

    // Enable drag-and-drop
    setInterceptsMouseClicks (true, true);
    setWantsKeyboardFocus (true);
    addKeyListener(this);
    
    addAndMakeVisible(mWaveformDisplay);
    addAndMakeVisible(mOriginalWaveform);

    // read audio file and display waveform
    // /Users/mihaitraista/5.Sound Libraries/fl1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    juce::File audioFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav");
    
    updateBufferAndRecalculateZeroCrossings(audioFile);
    
    addSlidersButtonsAndLabels();
    
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

void MainComponent::addSlidersButtonsAndLabels(){
    // Start Sample Index Slider and Label
    mStartSampleIndexSlider.setSliderStyle(juce::Slider::LinearBar);
    // the audio file is already loaded, so we can set the range to the length of the file
    if(mAudioFileBuffer.getNumSamples() > 0){
        mStartSampleIndexSlider.setRange(0, mAudioFileBuffer.getNumSamples() - mCycleLenHint * 2);
        mStartSampleIndexSlider.setValue(mAudioFileBuffer.getNumSamples() / 2, juce::sendNotification);
    } else {
        mStartSampleIndexSlider.setRange(0, 1, 1);
        mStartSampleIndexSlider.setValue(0);
    }

    mStartSampleIndexSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mStartSampleIndexSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 50));
    mStartSampleIndexSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    mStartSampleIndexSlider.addListener(this);
    addAndMakeVisible(&mStartSampleIndexSlider);

    // Cycle Len Hint Slider and Label
    addAndMakeVisible(&mCycleLenHintSlider);
    mCycleLenHintSlider.setSliderStyle(juce::Slider::LinearBar);
    // backgroundColourId none
    mCycleLenHintSlider.setRange(10, 4000, 1);
    mCycleLenHintSlider.setValue(200);
    mCycleLenHintSlider.addListener(this);
    
    addAndMakeVisible(mCycleLenHintSliderLabel);
    mCycleLenHintSliderLabel.setText("Zoom", juce::dontSendNotification);
    mCycleLenHintSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    mCycleLenHintSliderLabel.attachToComponent(&mCycleLenHintSlider, true);
    
    // Commit Cycle Button
    mCommitButton.setButtonText("Commit");
    mCommitButton.addListener(this);
    addAndMakeVisible(mCommitButton);
    
    // Save Resampled Button
    mSaveResampledFileButton.setButtonText("Save");
    mSaveResampledFileButton.addListener(this);
    addAndMakeVisible(mSaveResampledFileButton);
    
    // Clear Resampled Cycles Button
    mClearResampledCyclesButton.setButtonText("Clear");
    mClearResampledCyclesButton.addListener(this);
    addAndMakeVisible(mClearResampledCyclesButton);
    
    // Normalize Button
    mNormalizeButton.setButtonText("Normalize");
    mNormalizeButton.addListener(this);
    addAndMakeVisible(mNormalizeButton);

    // Show Resampled Length Label
    mResampledLengthLabel.setText("Resampled Buffer Length: 0 samples, 0 cycles", juce::dontSendNotification);
    mResampledLengthLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(mResampledLengthLabel);
    
    // mEventConfirmationLabel
    mEventConfirmationLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    // the text will be set before the label is made visible
    // it will be either "Cycle Committed!" or "File Saved!"
    mEventConfirmationLabel.setText("", juce::dontSendNotification);
    mEventConfirmationLabel.setFont(juce::Font("Arial", "Bold", 16.0f));
    addAndMakeVisible(mEventConfirmationLabel);
    mEventConfirmationLabel.setVisible(false);
    
    // mInstructionsLabel
    mInstructionsLabel.setText("Drag & Drop an audio file\n"
                               "Keyboard Shortcuts\n"
                               "G - previous cycle:\n"
                               "H - previous sample\n"
                               "J - next sample\n"
                               "K - next cycle\n"
                               "U - zoom out\n"
                               "I - zoom in\n"
                               "C - commit",
                               juce::dontSendNotification);
    mInstructionsLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    mInstructionsLabel.setJustificationType(juce::Justification::topLeft);
    mInstructionsLabel.setEditable(false, false, true);
    mInstructionsLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(mInstructionsLabel);
    
    // mResampledCycleLengthComboBox
    mResampledCycleLengthComboBox.addItem("512", 1);
    mResampledCycleLengthComboBox.addItem("1024", 2);
    mResampledCycleLengthComboBox.addItem("2048", 3);
    mResampledCycleLengthComboBox.addItem("4096", 4);
    mResampledCycleLengthComboBox.setSelectedId(2);
    mResampledCycleLengthComboBox.addListener(this);
    addAndMakeVisible(mResampledCycleLengthComboBox);
    
    // mResampledCycleLengthComboBoxLabel
    mResampledCycleLengthComboBoxLabel.setText("Resampled Cycle Len", juce::dontSendNotification);
    mResampledCycleLengthComboBoxLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    mResampledCycleLengthComboBoxLabel.attachToComponent(&mResampledCycleLengthComboBox, true);
    addAndMakeVisible(mResampledCycleLengthComboBoxLabel);
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    grabKeyboardFocus();
}

void MainComponent::resized()
{
    mWaveformDisplay.setBounds(0, 0, getWidth(), getHeight());
    mOriginalWaveform.setBounds(0, getHeight() - 50, getWidth(), 50);
    
    // the mStartSampleIndexSlider should overlap mOriginalWaveform
    mStartSampleIndexSlider.setBounds(0, getHeight() - 50, getWidth(), 50);
    mCycleLenHintSlider.setBounds(200, 10, getWidth() - 220, 20);
    
    mCommitButton.setBounds(200, 40, 80, 30);
    mSaveResampledFileButton.setBounds(290, 40, 80, 30);
    mClearResampledCyclesButton.setBounds(380, 40, 80, 30);
    mNormalizeButton.setBounds(470, 40, 80, 30);
    mResampledCycleLengthComboBox.setBounds(700, 40, 80, 30);
    
    mResampledLengthLabel.setBounds(200, 80, getWidth() - 20, 20);
    mEventConfirmationLabel.setBounds(10, 10, 300, 20);
    mInstructionsLabel.setBounds(10, 40, getWidth(), getHeight());
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mStartSampleIndexSlider){
        mStartSampleIndex = mStartSampleIndexSlider.getValue();
        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                    mClosestZeroCrossingStart,
                                                                    mClosestZeroCrossingEnd,
                                                                    mStartSampleIndex,
                                                                    mCycleLenHint);
        mEventConfirmationLabel.setVisible(false);
    } else if (slider == &mCycleLenHintSlider){
        // update the mStartSampleIndex so that we keep the waveform centered while zooming

        int newHint = mCycleLenHintSlider.getValue();
        int diff = newHint - mCycleLenHint;
        int halfDiff = diff / 2.0f;
        
        mStartSampleIndexSlider.setRange(0, mAudioFileBuffer.getNumSamples() - newHint * 2);
        mStartSampleIndexSlider.setValue(mStartSampleIndexSlider.getValue() - halfDiff, juce::sendNotification);

        // update mCycleLenHint and find new zeroCrossings start and end points
        mCycleLenHint = mCycleLenHintSlider.getValue();
        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                    mClosestZeroCrossingStart,
                                                                    mClosestZeroCrossingEnd,
                                                                    mStartSampleIndex,
                                                                    mCycleLenHint);

        mEventConfirmationLabel.setVisible(false);
    }
    repaint();
}

void MainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &mResampledCycleLengthComboBox)
    {
        int newValue = box->getText().getIntValue();
        std::cout << "New WTSIZE = " << newValue << std::endl;
        WTSIZE = newValue;
    }
}

void MainComponent::buttonClicked(juce::Button* button){
    if (button == &mCommitButton){
        handleCommitButton();
    } else if (button == &mSaveResampledFileButton){
        pFileHandler->saveResampledFileOnDisk(mResampledCycles);
        
        mEventConfirmationLabel.setText("File Saved!", juce::dontSendNotification);
        mEventConfirmationLabel.setVisible(true);
    } else if (button == &mClearResampledCyclesButton){
        mResampledCycles.clear();
        updateLengthInfoLabel();
        mVectorThatShowsWhichSamplesAreCommitted.assign(mVectorThatShowsWhichSamplesAreCommitted.size(), false);
    } else if (button == &mNormalizeButton){
        normalizeBuffer(mAudioFileBuffer);
    }
}

bool MainComponent::keyPressed(const juce::KeyPress& key, Component* originatingComponent){
    if (key.getTextCharacter() == 'C' || key.getTextCharacter() == 'c')
    {
        handleCommitButton();
        return true;
    } else if (key.getTextCharacter() == 'H' || key.getTextCharacter() == 'h'){
        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
        int newVal = mStartSampleIndexSlider.getValue() - cycleLen;
        if(newVal < 0)
            newVal = 0;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'L' || key.getTextCharacter() == 'l'){
        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
        int newVal = mStartSampleIndexSlider.getValue() + cycleLen;
        if(newVal > mAudioFileBuffer.getNumSamples() - cycleLen)
            newVal = mAudioFileBuffer.getNumSamples() - cycleLen;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'J' || key.getTextCharacter() == 'j'){
        int newVal = mStartSampleIndexSlider.getValue() - 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'K' || key.getTextCharacter() == 'k'){
        int newVal = mStartSampleIndexSlider.getValue() + 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'U' || key.getTextCharacter() == 'u'){
        int newVal = mCycleLenHintSlider.getValue() - 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'I' || key.getTextCharacter() == 'i'){
        int newVal = mCycleLenHintSlider.getValue() + 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    }
    return false;
}

void MainComponent::handleCommitButton(){
    const float* mAudioBufferData = mAudioFileBuffer.getReadPointer(0);

    int originalLengthOfCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;

    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
    }

    std::vector<float> origCycle(originalLengthOfCycle, 0.0f);
    for(int i = 0; i < originalLengthOfCycle; ++i){
        origCycle[i] = mAudioBufferData[i + mClosestZeroCrossingStart];
    }

    // store the resampled cycle in a new vector and then insert it at the end of mResampledCycles
//    std::vector<float> resampled(mResampledCycleLength, 0.0f);
    
    
    std::vector<float> resizedWaveform = std::vector<float>(WTSIZE, 0.0f);
    std::vector<float> polarValues = std::vector<float>(WTSIZE * 2, 0.0f);
    std::vector<float> resynthesized = std::vector<float>(WTSIZE, 0.0f);

    pResampler->resizeCycle(origCycle, resizedWaveform);
    
    Fourier::fillDftPolar(resizedWaveform, polarValues);

    Fourier::idft(polarValues, resynthesized);

    pFileHandler->saveVectorAsAudioFileToDesktop(resynthesized, "resynthesized.wav");
    
//    Fourier::rotateWavetableToNearestZero(resynthesized);

    mResampledCycles.insert(mResampledCycles.end(), resynthesized.begin(), resynthesized.end());

    updateLengthInfoLabel();

    mEventConfirmationLabel.setText("Cycle Committed!", juce::dontSendNotification);
    mEventConfirmationLabel.setVisible(true);

    repaint();
}

void MainComponent::updateLengthInfoLabel(){
    int len = static_cast<int>(mResampledCycles.size());
    juce::String labelText = "Resampled Buffer Length: " + juce::String(len) + " samples, " + juce::String(len / static_cast<float>(WTSIZE)) + " cycles";
    mResampledLengthLabel.setText(labelText, juce::dontSendNotification);
}

void MainComponent::filesDropped (const juce::StringArray& files, int x, int y)
{
    std::cout << "filesDropped" << std::endl;

    juce::File audioFile(files[0]);
    
    updateBufferAndRecalculateZeroCrossings(audioFile);
}

void MainComponent::updateBufferAndRecalculateZeroCrossings(juce::File& audioFile){
    pFileHandler->storeAudioFileInBuffer(audioFile, mAudioFileBuffer);
    
    mVectorThatShowsWhichSamplesAreCommitted.resize(mAudioFileBuffer.getNumSamples(), false);
    
    pZeroCrossingFinder->calculateZeroCrossings(mAudioFileBuffer, mZeroCrossings);

    pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                mClosestZeroCrossingStart,
                                                                mClosestZeroCrossingEnd,
                                                                mStartSampleIndex,
                                                                mCycleLenHint);

    mStartSampleIndexSlider.setRange(0, mAudioFileBuffer.getNumSamples() - mCycleLenHint * 2);
    mStartSampleIndexSlider.setValue(mAudioFileBuffer.getNumSamples() / 2, juce::sendNotification);
}

void MainComponent::normalizeBuffer(juce::AudioBuffer<float>& buffer){
    float* data = buffer.getWritePointer(0);
    float maxVal = 0.0f;
    
    for(int i = 0; i < buffer.getNumSamples(); ++i){
        if(fabs(data[i] > maxVal))
            maxVal = fabs(data[i]);
    }
    
    float scaler = 1.0f / maxVal;
    scaler *= 0.95; //  leave a bit of headroom
    
    for(int i = 0; i < buffer.getNumSamples(); ++i){
        data[i] = data[i] * scaler;
    }
    
    repaint();
}
