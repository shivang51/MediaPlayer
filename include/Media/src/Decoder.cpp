#include "Decoder.h"

#include <stdio.h> 

#pragma warning( disable : 26812 )
namespace Media
{
	Decoder::Decoder()
	{
		this->terminated = false;

		this->latestFrame = av_frame_alloc();
		this->latestPacket = av_packet_alloc();
	}

	Decoder::~Decoder()
	{
		this->TerminateDecoder();
	}

	void Decoder::FrameUnref(DataType::Frame* frame)
	{
		av_frame_unref(frame);
	}

	void Decoder::PacketUnref(DataType::Packet* packet)
	{
		av_packet_unref(packet);
	}

	void Decoder::TerminateDecoder()
	{
		if ( !this->terminated )
		{
			av_frame_free(&latestFrame);
			av_packet_free(&latestPacket);
			this->CloseFile();

			this->terminated = true;
		}
	}

	Error Decoder::OpenFile(DataType::File* file)
	{
		int err;

		if ( this->openedfile.open )
		{
			this->CloseFile(&openedfile);
		}

		if ( ( err = avformat_open_input(&fileFormatCtx, file->location.c_str(), nullptr, nullptr)) < 0 )
		{
			if ( err == -2 )
			{
				return Error::FileNotFound;
			}
			else
			{
				return Error::Unknown;
			}
		}

		this->openedfile = *file;
		this->openedfile.open = true;

		if ( avformat_find_stream_info(fileFormatCtx, 0) < 0 )
		{
			return Error::StreamInfoNotFound;
		}

		return Error::None;
	}

	void Decoder::CloseFile(DataType::File* file)
	{
		if ( file == nullptr )
		{
			file = &this->openedfile;
		}
		if ( !file->open )
		{
			return;
		}

		if ( this->swrCtx )
		{
			swr_free(&this->swrCtx);
		}

		if ( this->swsCtx )
			sws_freeContext(this->swsCtx);

		avcodec_free_context(&audioDecoder);
		avcodec_free_context(&videoDecoder);
		avformat_close_input(&fileFormatCtx);
		file->open = false;
		file->location = "";
	}

	float Decoder::GetDuration()
	{
		return (float) ( this->fileFormatCtx )->duration / (float) AV_TIME_BASE;
	}

	std::string Decoder::GetBestFileTitle()
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

	StreamType Decoder::GetStreamType(DataType::Stream* stream)
	{
		if (!stream->codecpar)
			throw std::invalid_argument("codecpar of stream was null");

		if ( stream->codecpar->codec_type == (AVMediaType) StreamType::Audio )
		{
			return StreamType::Audio;
		}
		else if ( stream->codecpar->codec_type == (AVMediaType) StreamType::Video )
		{
			return StreamType::Video;
		}
		else if ( stream->codecpar->codec_type == (AVMediaType) StreamType::Subtitle )
		{
			return StreamType::Subtitle;
		}
		else
		{
			return StreamType();
		}
	}

	DataType::Streams Decoder::GetStream(StreamType mediatype)
	{
		DataType::Streams streams = {};
		DataType::Stream* stream;
		for ( unsigned int i = 0; i < ( this->fileFormatCtx )->nb_streams; i++ )
		{
			stream = this->fileFormatCtx->streams [i];

			if (!stream->codecpar)
				throw std::invalid_argument("codecpar of stream was null");

			if ( stream->codecpar->codec_type == (AVMediaType) mediatype )
			{
				streams.push_back(stream);
			}
		}
		return streams;
	}

	DataType::StreamInfo Decoder::GetStreamInfo(DataType::Stream* stream)
	{

		if (!stream->codecpar)
			throw std::invalid_argument("codecpar of stream was null");

		DataType::StreamInfo info;
		AVDictionaryEntry* entry = NULL;

		while ( ( entry = av_dict_get(stream->metadata, "", entry, AV_DICT_IGNORE_SUFFIX) ) )
		{
			if ( std::strcmp(entry->key, "language") == 0 )
			{
				info.language = entry->value;
			}
			else if ( std::strcmp(entry->key, "title") == 0 )
			{
				info.title = entry->value;
			}
		}
		return info;
	}

	Error Decoder::SetStreamDecoder(DataType::Stream* stream, DataType::StreamDecoder** decoder)
	{

		if (!stream->codecpar)
			throw std::invalid_argument("codecpar of stream was null");

		auto streamCodec = avcodec_find_decoder(stream->codecpar->codec_id);
		*decoder = avcodec_alloc_context3(streamCodec);

		if ( avcodec_parameters_to_context(*decoder, stream->codecpar) < 0 )
		{
			throw std::runtime_error("Failed to load stream parameters");
		}

		if ( avcodec_open2(*decoder, streamCodec, NULL) < 0 )
		{
			throw std::runtime_error("Failed to open decoder");
		}

		return Error::None;
	}

	Error Decoder::SetSwrContext(DataType::StreamDecoder* audioDecoder)
	{
		if ( !swrCtx )
		{
			this->swrCtx = swr_alloc();
		}
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
		return Error::None;
	}

	Error Decoder::SetSwsContext(DataType::StreamDecoder* videoDecoder)
	{
		this->swsCtx = sws_getContext(
			videoDecoder->width, videoDecoder->height, videoDecoder->pix_fmt, //Input Parameters
			videoDecoder->width, videoDecoder->height, AV_PIX_FMT_RGBA, //Output Parameters
			SWS_BILINEAR, NULL, NULL, NULL
		);

		return Error::None;
	}

	void Decoder::SeekTo(float time)
	{
		time = time / (float) av_q2d(this->CurrentAudioStream.timebase);
		av_seek_frame(this->fileFormatCtx, this->CurrentAudioStream.index, (int64_t) time, AVSEEK_FLAG_ANY);
	}

	void Decoder::SetCurrentAudioStream(DataType::Stream* audioStream)
	{
		this->CurrentAudioStream.index = audioStream->index;
		this->CurrentAudioStream.timebase = audioStream->time_base;
		this->SetStreamDecoder(audioStream, &this->audioDecoder);
		this->SetSwrContext(this->audioDecoder);
	}

	void Decoder::SetCurrentVideoStream(DataType::Stream* videoStream)
	{
		this->CurrentVideoStream.index = videoStream->index;
		this->CurrentVideoStream.timebase = videoStream->time_base;
		this->SetStreamDecoder(videoStream, &this->videoDecoder);
		//this->SetSwsContext(this->videoDecoder);
	}

	DecodingError Decoder::DecodeAudioPacket(DataType::Packet* audioPacket, DataType::AudioFrame* audioFrame)
	{
		avcodec_send_packet(this->audioDecoder, audioPacket);

		int err = avcodec_receive_frame(this->audioDecoder, this->latestFrame);

		if ( err == AVERROR(EAGAIN) )
		{
			FrameUnref(this->latestFrame);
			return DecodingError::InsufficientData;
		}

		int nb = 0;
		audioFrame->pts = (float) ( this->latestFrame )->pts * (float) av_q2d(this->CurrentAudioStream.timebase);

		if ( this->audioDecoder->sample_fmt != AV_SAMPLE_FMT_S16 )
		{
			nb = ( this->latestFrame )->nb_samples * 2;
			audioFrame->data.resize(nb);
			uint8_t* buffer = (uint8_t*)audioFrame->data.data();
			swr_convert(this->swrCtx, &buffer, nb, (const uint8_t**) ( this->latestFrame )->extended_data, ( this->latestFrame )->nb_samples);
			audioFrame->buffersize = nb;
		}
		else
		{
			nb = ( this->latestFrame )->linesize [0];
			audioFrame->data = std::vector<int16_t>(
				*( this->latestFrame->extended_data ),
				*( this->latestFrame->extended_data + nb ));
			audioFrame->buffersize = nb;
		}
		this->currentTime = audioFrame->pts;
		FrameUnref(this->latestFrame);
		return DecodingError::None;
	}

	DecodingError Decoder::DecodeVideoPacket(DataType::Packet* videoPacket, DataType::VideoFrame* videoFrame)
	{
		avcodec_send_packet(this->videoDecoder, videoPacket);
		int err = avcodec_receive_frame(this->videoDecoder, this->latestFrame);
		if ( err == AVERROR(EAGAIN) )
		{
			FrameUnref(this->latestFrame);
			return DecodingError::InsufficientData;
		}

		videoFrame->pts = (float) ( this->latestFrame )->pts * (float) av_q2d(this->CurrentVideoStream.timebase);
				  

		videoFrame->yuv_size = std::vector<int>(latestFrame->linesize, latestFrame->linesize + 3);

		videoFrame->width = latestFrame->width;
		videoFrame->height = this->latestFrame->height;

		uint8_t** data = latestFrame->data;
		for ( int i = 0; i < 3; i++ )
		{
			int height = ( i == 0 ) ? videoFrame->height : videoFrame->height / 2;
			videoFrame->data.push_back(std::vector<uint8_t>(data [i], data [i] + (videoFrame->yuv_size.at(i) * height)));
		}

		this->currentTime = videoFrame->pts;

		FrameUnref(this->latestFrame);
		return DecodingError::None;
	}

	DataType::AudioFrame Decoder::GetAudioFrame(float time)
	{
		if ( time != -1.0f )
		{
			this->SeekTo(time);
		}

		DataType::AudioFrame audioFrame;
		int err = 0;
		DecodingError derr;
		bool gotFrame = false;
		while ( !gotFrame )
		{
			err = av_read_frame(this->fileFormatCtx, this->latestPacket);
			if ( err == AVERROR_EOF )
			{
				audioFrame.err = Error::FileEnded;
				gotFrame = true;
			}
			else if ( this->latestPacket->stream_index == this->CurrentAudioStream.index )
			{
				derr = this->DecodeAudioPacket(this->latestPacket, &audioFrame);
				if ( derr == DecodingError::None )
				{
					gotFrame = true;
				}
			}

			FrameUnref(this->latestFrame);
			PacketUnref(this->latestPacket);
		}
		return audioFrame;
	}

	DataType::VideoFrame Decoder::GetVideoFrame(float time)
	{
		if ( time != -1.0f )
		{
			this->SeekTo(time);
		}

		DataType::VideoFrame videoFrame;
		int err = 0;
		DecodingError derr;
		bool gotFrame = false;
		while ( !gotFrame )
		{
			err = av_read_frame(this->fileFormatCtx, this->latestPacket);
			if ( err == AVERROR_EOF )
			{
				videoFrame.err = Error::FileEnded;
				gotFrame = true;
			}
			else if ( this->latestPacket->stream_index == this->CurrentVideoStream.index )
			{
				derr = this->DecodeVideoPacket(this->latestPacket, &videoFrame);
				if ( derr == DecodingError::None )
				{
					gotFrame = true;
				}
			}

			FrameUnref(this->latestFrame);
			PacketUnref(this->latestPacket);
		}
		return videoFrame;
	}

	std::vector<DataType::AudioFrame> Decoder::GetNAudioFrames(float time)
	{
		std::vector<DataType::AudioFrame> audioFrames;
		bool gotFrame = false;
		if (currentTime > time)
			return audioFrames;
		do
		{
			audioFrames.push_back(GetAudioFrame());
		} while ( this->currentTime < time );
		return audioFrames;
	}

	std::vector<DataType::VideoFrame> Decoder::GetNVideoFrames(float time)
	{
		std::vector<DataType::VideoFrame> videoFrames;
		if (currentTime > time)
			return videoFrames;
		do
		{
			videoFrames.push_back(GetVideoFrame());
		} while (this->currentTime < time);
		return videoFrames;
	}

	std::vector<DataType::MediaFrame> Decoder::GetNFrames(float time)
	{
		std::vector<DataType::MediaFrame> frames{ };
		if (currentTime > time)
		{
			return frames;
		}
		do
		{
			frames.push_back(GetNextFrame());
		} while (this->currentTime < time);
		return frames;
	}

	DataType::MediaFrame Decoder::GetNextFrame()
	{
		DataType::MediaFrame frame;
		int err = 0;
		DecodingError derr;
		bool gotFrame = false;
		while ( !gotFrame )
		{
			err = av_read_frame(this->fileFormatCtx, this->latestPacket);
			if ( err == AVERROR_EOF )
			{
				frame.err = Error::FileEnded;
				gotFrame = true;
			}
			else if ( this->latestPacket->stream_index == this->CurrentAudioStream.index )
			{
				derr = this->DecodeAudioPacket(this->latestPacket, &frame.audioFrame);
				if ( derr == DecodingError::None )
				{
					frame.type = Media::FrameType::Audio;
					frame.pts = frame.audioFrame.pts;
					gotFrame = true;
				}
			}
			else if ( this->latestPacket->stream_index == this->CurrentVideoStream.index )
			{
				derr = this->DecodeVideoPacket(this->latestPacket, &frame.videoFrame);
				if ( derr == DecodingError::None )
				{
					frame.type = Media::FrameType::Video;
					frame.pts = frame.videoFrame.pts;
					gotFrame = true;
				}
			}

			FrameUnref(this->latestFrame);
			PacketUnref(this->latestPacket);

		}
		return frame;
	}

	int Decoder::GetSampleRate()
	{
		return this->audioDecoder->sample_rate;
	}

	DataType::Size Decoder::GetVideoSize()
	{
		DataType::Size size;
		if( this->videoDecoder->codec != nullptr ){
			size.width = this->videoDecoder->width;
			size.height = this->videoDecoder->height;
		}
		return size;
	}
	
	float Decoder::GetAspectRatio()
	{
		return (float)this->videoDecoder->width / (float)this->videoDecoder->height;
	}
}
