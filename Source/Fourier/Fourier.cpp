#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>

#include "Fourier.hpp"

stftpitchshift::RFFT Fourier::smJuriHock;
Fourier::Cache Fourier::smCache;

void Fourier::execute(const std::vector<float>& resampledCycle,
                      std::vector<float>& mPolarCycles,
                      std::array<std::vector<float>, N_WT_BANDS>& mResynthesizedCycles){
    
    calculateCacheRect(resampledCycle);
    calculateCachePolar();
    calculateCacheBandLimited();
    
    appendCachesToVectors(mPolarCycles, mResynthesizedCycles);
}

void Fourier::calculateCacheRect(const std::vector<float>& resampledCycle){
    smJuriHock.fft(resampledCycle, smCache.rect);

    // THIS IS VERY IMPORTANT !!! The DC and Niquist must be zeroed out
    smCache.rect[0] = smCache.rect[smCache.rect.size() - 1] = 0;
}
void Fourier::calculateCachePolar(){
    convertCartesianToPolar(smCache.rect, smCache.polar);
}

void Fourier::calculateCacheBandLimited(){
    for(int band = 0; band < N_WT_BANDS; band++){
        clearCacheBandLimited();
        float harmonicLimitFloat = 22050.0f / baseFrequencies[band];
        float rollOffPercent = 50.0f;
        // we divide the limit by 2 because
        harmonicLimitFloat /= 2;
        int harmonicLimitInt = std::max(1, static_cast<int>(harmonicLimitFloat));
        
        idftBand(smCache.resynthesized[band],
                 harmonicLimitInt,
                 rollOffPercent);
    }
}

void Fourier::clearCacheBandLimited(){
    std::fill(smCache.rectTempBL.begin(),
              smCache.rectTempBL.end(),
              0.0f);
    std::fill(smCache.polarTempBL.begin(),
              smCache.polarTempBL.end(),
              0.0f);
}

void Fourier::appendCachesToVectors(std::vector<float>& mPolarCycles,
                                    std::array<std::vector<float>, N_WT_BANDS>& mResynthesizedCycles){
    
    mPolarCycles.insert(mPolarCycles.end(), smCache.polar.begin(), smCache.polar.end());
    
    for(int band = 0; band < N_WT_BANDS; band++){
        mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(),
                                          smCache.resynthesized[band].begin(),
                                          smCache.resynthesized[band].end());
    }
}

void Fourier::idftBand(std::vector<float>& resynthesized,
                       int harmonicLimit,
                       float rollOffPercent){

    float amp, phase, roll;
    float startRolloff = harmonicLimit - harmonicLimit * (rollOffPercent / 100.0f);
    
    for(int i = 0; i < WTSIZE / 2; i++){
        float magnitude = smCache.polar[i * 2];
        float phase = smCache.polar[i * 2 + 1];
        
        if(i >= startRolloff && i < harmonicLimit){
            roll = 1 - (static_cast<int>(i) - startRolloff) / (harmonicLimit - startRolloff);
            roll *= roll; // square roll for exponential roll off
            magnitude *= roll;
        } else if(i > harmonicLimit){
            break;
        }
                    
        smCache.polarTempBL[i * 2] = magnitude;
        smCache.polarTempBL[i * 2 + 1] = phase;
    }
    
    convertPolarToCartesian(smCache.polarTempBL, smCache.rectTempBL);
    smJuriHock.ifft(smCache.rectTempBL, resynthesized);
    
    // ??
//    applyLinearSlant(tempResynthesized, nCycles);
}

void Fourier::convertCartesianToPolar(std::vector<std::complex<float>>& dft, std::vector<float>& polar){
    assert(dft.size() >= WTSIZE / 2);
    assert(polar.size() == WTSIZE);
    
    for(int i = 0; i < WTSIZE / 2; i++){
        polar[i * 2] = std::abs(dft[i]);        // magnitude
        polar[i * 2 + 1] = std::arg(dft[i]);    // phase
    }
}

void Fourier::convertPolarToCartesian(std::vector<float>& polar, std::vector<std::complex<float>>& dft){
    assert(dft.size() >= WTSIZE / 2);
    assert(polar.size() == WTSIZE);

    for(int i = 0; i < WTSIZE / 2; i++){
        float magnitude = polar[i * 2];
        float phase = polar[i * 2 + 1];
        
        dft[i] = std::polar(magnitude, phase);
    }
}

void Fourier::applyLinearSlant(std::vector<float>& waveform, int nCycles) {
    for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
        float startValue = waveform[cycleIndex * WTSIZE];
        float endValue = waveform[(cycleIndex + 1) * WTSIZE - 1];
        float delta = endValue - startValue;

        for (int i = 0; i < WTSIZE; ++i) {
            float ramp = i / static_cast<float>(WTSIZE - 1);
            waveform[cycleIndex * WTSIZE + i] -= (startValue + delta * ramp);
        }
    }
}

