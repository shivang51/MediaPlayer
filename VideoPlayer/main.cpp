#include <iostream>

#include "Media.h"

int main()
{

	Media::File file;
	file.location = "F:\\Movies\\4_5875089835827398390.mkv";
	//file.location = "F:\\Movies\\4_5875089835827398378.mp4";

	Media::Decoder decoder;
	decoder.OpenFile(&file);

	Media::Streams audios = decoder.GetStream(Media::StreamType::Audio);
	//Media::StreamInfo info = decoder.GetStreamInfo(audios[0]);

	decoder.SetCurrentAudioStream(audios [0]);
	Media::AudioFrame frame = decoder.GetAudioFrame();

	return 0;
}