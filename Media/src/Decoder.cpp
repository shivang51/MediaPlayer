#include "Decoder.h"

#include <stdio.h> 

#pragma warning( disable : 26812 )

Media::StreamDecoder Media::MakeStreamDecoder(AVCodecContext* dec)
{
	return StreamDecoder(dec);
}

Media::Decoder::Decoder()
{
	this->terminated = false;
	av_dict_set(this->mediaOptions.get(), "pixel_format", "rgb24", 0);
}
 
Media::Decoder::~Decoder()
{
	this->TerminateDecoder();
}

void Media::Decoder::TerminateDecoder()
{
	if ( !this->terminated )
	{
		av_dict_free(this->mediaOptions.get());
		av_frame_free(this->latestFrame.get());
		av_packet_free(this->latestPacket.get());
		this->CloseFile();

		this->terminated = true;
	}
}

Media::Error Media::Decoder::OpenFile(Media::File* file)
{
	int err;

	if ( this->openedfile.open )
	{
		this->CloseFile(&openedfile);
	}

	if ( ( err = avformat_open_input(this->fileFormatCtx.get(), file->location.c_str(), NULL, this->mediaOptions.get()) ) < 0 )
	{
		if ( err == -2 )
		{
			return Media::Error::FileNotFound;
		}
		else
		{
			return Media::Error::Unknown;
		}
	}

	this->openedfile = *file;
	this->openedfile.open = true;

	if ( avformat_find_stream_info(*this->fileFormatCtx, 0) < 0 )
	{
		return Error::StreamInfoNotFound;
	}

	Media::Streams audioStreams = this->GetStream(Media::StreamType::Audio);
	this->audioDecoder = this->GetStreamDecoder(audioStreams [0]);


	return Error::None;
}

void Media::Decoder::CloseFile(Media::File* file)
{
	if ( file == nullptr )
	{
		file = &this->openedfile;
	}
	if ( file->open )
	{
		avformat_close_input(this->fileFormatCtx.get());
		file->open = false;
		file->location = "";
	}
	if ( swr_is_initialized(*this->swrCtx) )
	{
		swr_free(this->swrCtx.get());
	}

	sws_freeContext(this->swsCtx);
}

float Media::Decoder::GetCurrentFileDuration()
{
	return (float) (*this->fileFormatCtx)->duration / (float) AV_TIME_BASE;
}

std::string Media::Decoder::GetBestFileTitle()
{
	std::string name;
	AVDictionaryEntry* entry = av_dict_get(( *this->fileFormatCtx )->metadata, "title", NULL, AV_DICT_IGNORE_SUFFIX);
	if ( entry )
	{
		name = entry->value;
		av_freep(entry);
		return name;
	}
	else
	{
		return this->openedfile.name();
	}
}

Media::StreamType Media::Decoder::GetStreamType(Media::Stream stream)
{
	if ( stream->codecpar->codec_type == (AVMediaType) Media::StreamType::Audio )
	{
		return Media::StreamType::Audio;
	}
	else if ( stream->codecpar->codec_type == (AVMediaType) Media::StreamType::Video )
	{
		return Media::StreamType::Video;
	}
	else if ( stream->codecpar->codec_type == (AVMediaType) Media::StreamType::Subtitle )
	{
		return Media::StreamType::Subtitle;
	}
	else
	{
		return Media::StreamType();
	}
}

Media::Streams Media::Decoder::GetStream(Media::StreamType mediatype)
{
	Media::Streams streams = {};
	std::shared_ptr<AVStream> stream;
	for ( unsigned int i = 0; i < (*this->fileFormatCtx)->nb_streams ; i++ )
	{
		stream = std::make_shared<AVStream>(*( *this->fileFormatCtx )->streams[i]);
		if ( stream->codecpar->codec_type == (AVMediaType)mediatype )
		{
			streams.push_back(stream);
		}
	}
	return streams;
}

Media::StreamInfo Media::Decoder::GetStreamInfo(Media::Stream stream)
{
	Media::StreamInfo info;
	AVDictionaryEntry* entry = NULL;

	while ( ( entry = av_dict_get(stream->metadata, "", entry, AV_DICT_IGNORE_SUFFIX) ) )
	{
		if ( std::strcmp(entry->key, "language") == 0)
		{
			info.language = entry->value;
		}
		else if ( std::strcmp(entry->key, "title") == 0)
		{
			info.title = entry->value;
		}
	}
	return info;
}

Media::StreamDecoder Media::Decoder::GetStreamDecoder(Media::Stream stream)
{
	std::shared_ptr<const AVCodec> streamCodec(avcodec_find_decoder(stream->codecpar->codec_id));
	tempDecoder = Media::MakeStreamDecoder(avcodec_alloc_context3(streamCodec.get()));
	
	if ( avcodec_parameters_to_context(tempDecoder.get(), stream->codecpar) < 0 )
	{
		
	}

	if ( avcodec_open2(tempDecoder.get(), streamCodec.get(), NULL) < 0 )
	{
		
	}

	return tempDecoder;
}

Media::Error Media::Decoder::SetSwrContext(Media::StreamDecoder audioDecoder)
{
	if ( swr_is_initialized(*this->swrCtx) )
	{
		swr_free(this->swrCtx.get());
	}
	swr_alloc_set_opts(
		*this->swrCtx,
		AV_CH_LAYOUT_STEREO,
		AV_SAMPLE_FMT_S16,
		this->audioDecoder->sample_rate,
		this->audioDecoder->channel_layout,
		this->audioDecoder->sample_fmt,
		this->audioDecoder->sample_rate,
		0,
		nullptr
	);
	swr_init(*this->swrCtx);
	return Media::Error::None;
}

Media::Error Media::Decoder::SetSwsContext(Media::StreamDecoder videoDecoder)
{
	this->swsCtx = sws_getContext(
		videoDecoder->width, videoDecoder->height, videoDecoder->pix_fmt,
		videoDecoder->width, videoDecoder->height, AV_PIX_FMT_RGBA,
		SWS_BILINEAR, NULL, NULL, NULL
	);

	return Media::Error::None;
}

void Media::Decoder::SeekTo(float time)
{
	time = time / (float) av_q2d(this->CurrentAudioStream.timebase);
	av_seek_frame(*this->fileFormatCtx, this->CurrentAudioStream.index, (int64_t)time, AVSEEK_FLAG_BACKWARD);
}

void Media::Decoder::SetCurrentAudioStream(Media::Stream audioStream)
{
	this->CurrentAudioStream.index = audioStream->index;
	this->CurrentAudioStream.timebase = audioStream->time_base;
	this->audioDecoder = this->GetStreamDecoder(audioStream);
	this->SetSwrContext(this->audioDecoder);
}

void Media::Decoder::SetCurrentVideoStream(Media::Stream videoStream)
{
	this->CurrentVideoStream.index = videoStream->index;
	this->CurrentVideoStream.timebase = videoStream->time_base;
	this->videoDecoder = this->GetStreamDecoder(videoStream);
	this->SetSwsContext(this->videoDecoder);
}

Media::DecodingError Media::Decoder::DecodeAudioPacket(Media::Packet audioPacket, Media::AudioFrame audioFrame)
{
	int err = avcodec_send_packet(this->audioDecoder.get(), *audioPacket);

	while ( err > 0 )
	{
		err = avcodec_receive_frame(this->audioDecoder.get(), *this->latestFrame);

		if ( err == AVERROR(EAGAIN) )
		{
			return Media::DecodingError::InsufficientData;
		}

		int nb = 0;
		audioFrame.pts = (float) ( *this->latestFrame )->pts * (float) av_q2d(this->CurrentAudioStream.timebase);

		if ( this->audioDecoder->sample_fmt != AV_SAMPLE_FMT_S16 )
		{
			nb = ( *this->latestFrame )->nb_samples * 4;
			audioFrame.data.resize(nb);
			uint8_t* buffer = audioFrame.data.data();
			swr_convert(*this->swrCtx, &buffer, nb, (const uint8_t**) ( *this->latestFrame )->extended_data, ( *this->latestFrame )->nb_samples);
			audioFrame.buffersize = nb;
		}
		else
		{
			nb = ( *this->latestFrame )->linesize [0];
			audioFrame.data = std::vector<uint8_t>(
				*(( *this->latestFrame )->extended_data), 
				*(( *this->latestFrame )->extended_data + nb));
			audioFrame.buffersize = nb;
		}
	}
	return Media::DecodingError::None;
}

Media::AudioFrame Media::Decoder::GetAudioFrame(float time)
{
	if ( time != -1.0f )
	{
		this->SeekTo(time);
	}

	AudioFrame audioFrame;
	int err = 0;
	Media::DecodingError derr;
	bool gotFrame = false;
	while ( !gotFrame )
	{
		err = av_read_frame(*this->fileFormatCtx, *this->latestPacket);
		if ( err == AVERROR_EOF )
		{
			audioFrame.err = Media::Error::FileEnded;
		}
		else if ( (*this->latestPacket)->stream_index == this->CurrentAudioStream.index)
		{
			derr = this->DecodeAudioPacket(this->latestPacket, audioFrame);
			if ( derr == Media::DecodingError::None )
			{
				gotFrame = true;
			}
		}
	} 
	return audioFrame;
}

