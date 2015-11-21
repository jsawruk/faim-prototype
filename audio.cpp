// Audio.cpp
// Implementation file for audio class
#include "audio.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <portaudio.h>

static int lfoIndex;
static float lfoTable[LFO_SIZE];

// Constructor
Audio::Audio() {

	this->inputDeviceName = INPUT_DEVICE_NAME;
	this->outputDeviceName = OUTPUT_DEVICE_NAME;

	this->inputDeviceIndex = -1;
	this->outputDeviceIndex = -1;
}

// Destructor
Audio::~Audio() {

}

// Methods
int Audio::init() {

	printf("Initializing PortAudio: ");

	// Initialize PortAudio
	PaError err;
	err = Pa_Initialize();
	printf("%s \n", Pa_GetErrorText(err));

	// Enumerate devices
	int numberOfDevices = Pa_GetDeviceCount();
	for(int i = 0; i < numberOfDevices; i++) {
		const PaDeviceInfo *info = Pa_GetDeviceInfo((PaDeviceIndex) i);
		printf("%s\n", info->name);

		// Find input and output devices
		if (strcmp(info->name, this->inputDeviceName) == 0) {
			printf("Input device found! \n");
			this->inputDeviceIndex = i;
		}

		if (strcmp(info->name, this->outputDeviceName) == 0) {
			printf("Output device found! \n");
			this->outputDeviceIndex = i;
		}

		printf("Input channels:  %d\n", info->maxInputChannels);
		printf("Output channels: %d\n", info->maxOutputChannels);
		printf("------------------------\n");
	}

	if (this->inputDeviceIndex == -1 || this->outputDeviceIndex == -1) {
		printf("I/O devices not found\n");
		return -1;
	}

	// Set stream parameters

	memset(&this->inputParams, 0, sizeof(PaStreamParameters));
	this->inputParams.device = this->inputDeviceIndex;
	this->inputParams.channelCount = 1;					// MONO input
	this->inputParams.sampleFormat = paFloat32;

	memset(&outputParams, 0, sizeof(PaStreamParameters));
	this->outputParams.device = this->outputDeviceIndex;
	this->outputParams.channelCount = 2;				// STEREO output
	this->outputParams.sampleFormat = paFloat32; // | paNonInterleaved;	// Non-interleaved (separate buffers for left and right)

	Audio::initLFO();

	return 0;
}

int Audio::openStream() {

	PaError err;

	err = Pa_OpenStream(&this->streamHandle, &this->inputParams, &this->outputParams, 
		SAMPLE_RATE, BUFFER_SIZE, paNoFlag, &this->callback, NULL);

	if (err != paNoError) {
		return -1;
	}

	err = Pa_StartStream(this->streamHandle);

	if (err != paNoError) {
		return -1;
	}

	printf("Audio stream opened\n");

	return 0;
}

int Audio::closeStream() {

	Pa_StopStream(this->streamHandle);
	Pa_CloseStream(this->streamHandle);
	Pa_Terminate();

	printf("Audio stream closed\n");

	return 0;
}

// Precompute the LFO (sine) waveform
void Audio::initLFO() {

	// Amplitude is set to 0.5 so that max amount Left or Right is 1.0
	for (int i = 0; i < LFO_SIZE; i++) {
		lfoTable[i] = 0.5 * sin((i * TWO_PI) / LFO_SIZE);
	}

}

int Audio::callback(const void *inputBuffer, void *outputBuffer, unsigned long frameCount, 
    const PaStreamCallbackTimeInfo *timeinfo, PaStreamCallbackFlags statusFlags, void *userData) {

	const float *input = (const float *)inputBuffer;
	float *output = (float *)outputBuffer;

	for (int i = 0; i < frameCount; i++) {

		// Get amplitude from LFO
		float leftAmplitude = 0.5;
		float rightAmplitude = 0.5;

		float lfoValue = lfoTable[lfoIndex];

		if (lfoValue > 0.0) {
			// More left
			leftAmplitude += lfoValue;
			rightAmplitude -= lfoValue;
		}
		else if (lfoValue < 0.0) {
			// More right
			leftAmplitude -= lfoValue;
			rightAmplitude += lfoValue;
		}

		lfoIndex++;

		// Wrap around
		if (lfoIndex > LFO_SIZE) {
			lfoIndex = 0;
		}

		//printf("Left : %f.2\n", leftAmplitude);
		//printf("Right: %f.2\n", rightAmplitude);

		float inputValue = *input++;	// Input is mono, so only increment the pointer once per loop
		
		// Output is stereo and interleaved, so increment its pointer twice per loop
		*output++ = leftAmplitude * inputValue;	// LEFT
		*output++ = rightAmplitude * inputValue;	// RIGHT
	}

	return paContinue;

}