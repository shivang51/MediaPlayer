#include "Player.h"

Player::Audio::Audio()
{
}

Player::Audio::~Audio()
{
	
}

Player::AudioError Player::Audio::Init()
{
	if ( intialized )
	{
		return AudioError::AreadyInitialized;
	}
	else if ( Pa_Initialize() != paNoError )
	{
		return AudioError::FailedToInit;
	}
	this->intialized = true;
	
	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if ( outputParameters.device == paNoDevice )
	{
		throw "No Output Device Found";
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paUInt8; /* 8 bit unsigned int */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream,
		NULL, /* no input */
		&outputParameters,
		this->sampleRate,
		64,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		&StreamCallback,
		&this->sampleRate);


	return AudioError::None;
}

void Player::Audio::Close()
{
	Pa_Terminate();
}

int Player::Audio::StreamCallback(const void* input, void* output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	uint8_t* in = (uint8_t*) input;
	uint8_t* out = (uint8_t*) output;
	auto data = (UserData*) userData;

	return 0;
}

void Player::Audio::SetSampleRate(uint8_t sampleRate)
{
	this->sampleRate = sampleRate;
}

void Player::Audio::Play(DataType::AudioFrame* frame)
{
}

void Player::Audio::Stop()
{
}
