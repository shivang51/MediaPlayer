#include "Player_Audio.h"


namespace Player
{
	bool Audio::intialized = false;

	Audio::Audio()
	{
		if (!this->intialized)
			if (Pa_Initialize() != paNoError)
			{
				throw AudioError::FailedToInit;
			}

		this->intialized = true;

	}

	Audio::~Audio()
	{
		this->close_device();
		Pa_Terminate();
	}

	AudioError Audio::init_device(int sampleRate)
	{
		outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
		if (outputParameters.device == paNoDevice)
		{
			throw std::runtime_error("No Output Device Found");
		}
		outputParameters.channelCount = 2;
		outputParameters.sampleFormat = paInt16;
		outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		err = Pa_OpenStream(
			&stream,
			NULL, /* no input parameters*/
			&outputParameters,
			sampleRate,
			1024,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			nullptr,
			nullptr);

		return AudioError::None;
	}

	void Audio::close_device()
	{
		this->stop();
		Pa_CloseStream(stream);
	}

	void Audio::play(DataType::AudioFrame* frame)
	{

		size_t size = (int) ( frame->data.size() / 2 );
		int16_t* buffer = frame->data.data();

		if (!Pa_IsStreamActive(stream))
			Pa_StartStream(stream);

		Pa_WriteStream(stream, buffer, (unsigned long) size);
	}

	void Audio::stop()
	{
		Pa_StopStream(stream);
	}

	std::vector<DataType::AudioDevice> Audio::get_devices()
	{
		auto total_devices = Pa_GetDeviceCount();
		PaDeviceInfo* info = nullptr;
		DataType::AudioDevice device;
		std::vector<DataType::AudioDevice> devices;
		for (int i = 0; i < total_devices; i++)
		{
			info = (PaDeviceInfo*) Pa_GetDeviceInfo(i);

			device.name = std::string(info->name);
			device.in_channels = info->maxInputChannels;
			device.out_channels = info->maxOutputChannels;

			if (device.in_channels > 0)
			{
				device.type = Media::DeviceType::Output;
			}
			if (device.out_channels > 0)
			{
				device.type = ( device.type != Media::DeviceType::None ) ?
					Media::DeviceType::In_Out : Media::DeviceType::Output;
			}

			devices.push_back(device);
		}

		return devices;
	}
}