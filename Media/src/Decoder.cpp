#include "Decoder.h"

#include <stdio.h> 

#pragma warning( disable : 26812 )

Media::Decoder::Decoder()
{
	this->terminated = false;
	av_dict_set(this->mediaOptions.get(), "pixel_format", "rgb24", 0);
	this->swrCtx = swr_alloc();

	this->latestFrame = std::unique_ptr<AVFrame>(av_frame_alloc());
	this->latestPacket = std::unique_ptr<AVPacket>(av_packet_alloc());
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
		auto frame = this->latestFrame.release();
		av_frame_free(&frame);
		auto packet = this->latestPacket.release();
		av_packet_free(&packet);
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

	AVFormatContext* ffCtx = nullptr;

	if ( ( err = avformat_open_input(&ffCtx, file->location.c_str(), NULL, this->mediaOptions.get()) ) < 0 )
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

	this->fileFormatCtx = std::unique_ptr<AVFormatContext>(ffCtx);

	if ( avformat_find_stream_info(this->fileFormatCtx.get(), 0) < 0 )
	{
		return Error::StreamInfoNotFound;
	}

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
		auto ffCtx = this->fileFormatCtx.get();
		avformat_close_input(&ffCtx);
		file->open = false;
		file->location = "";
	}

	auto audioDecBuffer = this->audioDecoder.release();
	avcodec_free_context(&audioDecBuffer);

	if ( swr_is_initialized(this->swrCtx) )
	{
		swr_free(&this->swrCtx);
	}

	sws_freeContext(this->swsCtx);
}

float Media::Decoder::GetCurrentFileDuration()
{
	return (float) (this->fileFormatCtx)->duration / (float) AV_TIME_BASE;
}

std::string Media::Decoder::GetBestFileTitle()
{
	std::string name;
	AVDictionaryEntry* entry = av_dict_get(( this->fileFormatCtx )->metadata, "title", NULL, AV_DICT_IGNORE_SUFFIX);
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

Media::StreamType Media::Decoder::GetStreamType(Media::Stream& stream)
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
	for ( unsigned int i = 0; i < (this->fileFormatCtx)->nb_streams ; i++ )
	{
		stream = std::make_shared<AVStream>(*( this->fileFormatCtx )->streams[i]);
		if ( stream->codecpar->codec_type == (AVMediaType)mediatype )
		{
			streams.push_back(stream);
		}
	}
	return streams;
}

Media::StreamInfo Media::Decoder::GetStreamInfo(Media::Stream& stream)
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

Media::Error Media::Decoder::SetStreamDecoder(Media::Stream& stream, Media::StreamDecoder& decoder)
{
	std::unique_ptr<const AVCodec> streamCodec(avcodec_find_decoder(stream->codecpar->codec_id));
	decoder = MakeStreamDecoder(avcodec_alloc_context3(streamCodec.get()));
	
	if ( avcodec_parameters_to_context(this->audioDecoder.get(), stream->codecpar) < 0 )
	{
		
	}

	if ( avcodec_open2(this->audioDecoder.get(), streamCodec.get(), NULL) < 0 )
	{
		
	}

	av_free((AVCodec*)streamCodec.release());

	return Error::None;
}

Media::Error Media::Decoder::SetSwrContext(Media::StreamDecoder& audioDecoder)
{
	 
	if ( swr_is_initialized(this->swrCtx) )
	{
		swr_free(&this->swrCtx);
	}
	swr_alloc_set_opts(
		this->swrCtx,
		AV_CH_LAYOUT_STEREO,
		AV_SAMPLE_FMT_S16,
		this->audioDecoder->sample_rate,
		this->audioDecoder->channel_layout,
		this->audioDecoder->sample_fmt,
		this->audioDecoder->sample_rate,
		0,
		nullptr
	);
	swr_init(this->swrCtx);
	return Media::Error::None;
}

Media::Error Media::Decoder::SetSwsContext(Media::StreamDecoder& videoDecoder)
{
	this->swsCtx = MakeSwsCtx(sws_getContext(
		videoDecoder->width, videoDecoder->height, videoDecoder->pix_fmt,
		videoDecoder->width, videoDecoder->height, AV_PIX_FMT_RGBA,
		SWS_BILINEAR, NULL, NULL, NULL
	));

	return Media::Error::None;
}

void Media::Decoder::SeekTo(float time)
{
	time = time / (float) av_q2d(this->CurrentAudioStream.timebase);
	av_seek_frame(this->fileFormatCtx.get(), this->CurrentAudioStream.index, (int64_t)time, AVSEEK_FLAG_ANY);
}

void Media::Decoder::SetCurrentAudioStream(Media::Stream& audioStream)
{
	this->CurrentAudioStream.index = audioStream->index;
	this->CurrentAudioStream.timebase = audioStream->time_base;
	this->SetStreamDecoder(audioStream, this->audioDecoder);
	this->SetSwrContext(this->audioDecoder);
}

void Media::Decoder::SetCurrentVideoStream(Media::Stream& videoStream)
{
	this->CurrentVideoStream.index = videoStream->index;
	this->CurrentVideoStream.timebase = videoStream->time_base;
	this->SetStreamDecoder(videoStream, this->videoDecoder);
	this->SetSwsContext(this->videoDecoder);
}

Media::DecodingError Media::Decoder::DecodeAudioPacket(Media::Packet& audioPacket, Media::AudioFrame& audioFrame)
{
	avcodec_send_packet(this->audioDecoder.get(), audioPacket.get());

	int err = avcodec_receive_frame(this->audioDecoder.get(), this->latestFrame.get());

	if ( err == AVERROR(EAGAIN) )
	{
		Media::FrameUnref(this->latestFrame);
		return Media::DecodingError::InsufficientData;
	}

	int nb = 0;
	audioFrame.pts = (float) ( this->latestFrame )->pts * (float) av_q2d(this->CurrentAudioStream.timebase);

	if ( this->audioDecoder->sample_fmt != AV_SAMPLE_FMT_S16 )
	{
		nb = ( this->latestFrame )->nb_samples * 4;
		audioFrame.data.resize(nb);
		uint8_t* buffer = audioFrame.data.data();
		swr_convert(this->swrCtx, &buffer, nb, (const uint8_t**) ( this->latestFrame )->extended_data, ( this->latestFrame )->nb_samples);
		audioFrame.buffersize = nb;
	}
	else
	{
		nb = ( this->latestFrame )->linesize [0];
		audioFrame.data = std::vector<uint8_t>(
			*( this->latestFrame->extended_data ),
			*( this->latestFrame->extended_data + nb ));
		audioFrame.buffersize = nb;
	}
	Media::FrameUnref(this->latestFrame);
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
		err = av_read_frame(this->fileFormatCtx.get(), this->latestPacket.get());
		if ( err == AVERROR_EOF )
		{
			audioFrame.err = Media::Error::FileEnded;
			gotFrame = true;
		}
		else if ( this->latestPacket->stream_index == this->CurrentAudioStream.index )
		{
			derr = this->DecodeAudioPacket(this->latestPacket, audioFrame);
			if ( derr == Media::DecodingError::None )
			{
				gotFrame = true;
			}
		}

		Media::FrameUnref(this->latestFrame);
		Media::PacketUnref(this->latestPacket);
	}
	return audioFrame;
}
