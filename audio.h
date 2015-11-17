// Audio.h
// Header file for audio class

#pragma once

#import <portaudio.h>

// Define device names
// Default OS X
//#define INPUT_DEVICE_NAME "Built-in Input"
//#define OUTPUT_DEVICE_NAME "Built-in Output"

// Line6 TonePort UX2
#define INPUT_DEVICE_NAME "TonePort UX2"
#define OUTPUT_DEVICE_NAME "TonePort UX2"

#define TWO_PI  6.283185307179586476925287

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024

#define LFO_SIZE 441000	// 441000 samples = 0.1 Hz

class Audio
{
public:
	// Constructor
	Audio();
	~Audio();

	// Methods
	int init();
	int openStream();
	int closeStream();

	static void initLFO();

private:
	const char* inputDeviceName;
	const char* outputDeviceName;

	int inputDeviceIndex;
	int outputDeviceIndex;

	PaStreamParameters inputParams;
	PaStreamParameters outputParams;

	PaStream *streamHandle;

    // This method has to be static to use PortAudio callback mode
    static int callback(const void *inputBuffer, void *outputBuffer, unsigned long frameCount, 
    	const PaStreamCallbackTimeInfo *timeinfo, PaStreamCallbackFlags statusFlags, void *userData);
};