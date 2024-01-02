# BL Resampler (Band Limited Resampler)

### author Mihai Traista, last update Jan 2024

![band limited resampler screenshot, JUCE C++](https://firebasestorage.googleapis.com/v0/b/platosolids-742bb.appspot.com/o/projects-images%2FBLResampler_Screenshot.png?alt=media&token=763fe136-922e-4eae-891a-0eca8e423de8 "Band Limited Resampler Screenshot")

## How to use
1. Drag and Drop a mono .wav file in the area "Drag & Drop Original"
2. Use the zoom and pan buttons to select a portion of the audio waveform
3. Once you decided on the beginning and the end of the waveform, click "Commit"
4. Repeat for all the waveforms that you're like to resample
5. Once committed, the waveform will be resampled to a length of 1024 samples.
6. In addition, the application creates 10 band limited waveforms for the following base frequencise in Hz: 25.0, 50.0, 100.0, 200.0, 400.0, 800.0, 1600.0, 3200.0, 6400.0, 12800.0
7. Click "Save" to store your resamples wavform as well as the 10 resynthesized band-limited wavetables.

## Licence
This project is licensed under the MIT License