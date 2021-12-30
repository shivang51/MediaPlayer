#include <iostream>
#include "Media.h"
#include "Audio.h"
#include "Datatypes.h"

int main()
{
	Media::Decoder decoder;

	DataType::File file;
	file.location = "F:\\Videos\\Proj\\v2.mp4";

	auto err = decoder.OpenFile(&file);

	if ( err != Media::Error::None )
	{
		return -1;
	}

	std::cout << decoder.GetBestFileTitle().c_str() << std::endl;

	auto audioStream = decoder.GetStream(Media::StreamType::Audio);
	decoder.SetCurrentAudioStream(audioStream.at(0));

	auto frame = decoder.GetAudioFrame(5);

	Player::Audio audioPlayer;
	audioPlayer.Init();

	
	return 0;
}