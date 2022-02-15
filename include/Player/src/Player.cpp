#include "Player.h"

namespace Player
{
    Player::Player()
    {
        this->audio = new Audio();
        this->video = new Video();
        this->decoder = new Media::Decoder();
    }

    Player::~Player()
    {
        if (this->decoding_thread.joinable())
        {
            this->playing = false;
            decoding_thread.join();
        }

        if (this->audio != nullptr)
        {
            delete this->audio;
        }

        if (this->video != nullptr)
        {
            delete this->video;
        }

        if (this->decoder)
        {
            delete this->decoder;
        }
    }

    bool Player::file_exist(const std::string& name)
    {
        struct stat buffer;
        return ( stat(name.c_str(), &buffer) == 0 );
    }

    Error Player::load_file(const std::string& file_location)
    {
        //Checking if file exist
        if (!file_exist(file_location))
        {
            return Error::File_Not_Found;
        }

        if (this->playing)
        {
            this->stop();
        }

        this->active_file.location = file_location;
        
        //Loading the decoder
        auto err = this->decoder->OpenFile(&active_file);

        if ( err != Media::Error::None )
            return Error::Unknown;

        this->active_file.open = true;

        //Geting best name for active file from decoder
        this->active_file.title = this->decoder->GetBestFileTitle();
        
        this->duration = decoder->GetDuration();

        //Setting Video Stream
        auto streams = decoder->GetStream(Media::StreamType::Video);
        this->decoder->SetCurrentVideoStream(streams.at(0));

        //Setting Video Stream
        streams = decoder->GetStream(Media::StreamType::Audio);
        this->decoder->SetCurrentAudioStream(streams.at(0));

        //Getting first frame
        auto frame = decoder->GetVideoFrame();
        decoder->SeekTo(0.0f);

        //Setting aspect ratio (width / height)
        this->aspect_ratio = decoder->GetAspectRatio();

        //Intitalizing video displaying element
        this->video->init({(size_t)frame.yuv_size[0], (size_t) frame.height });
        
        //Setting audio samplerate
        this->audio->init_device();

        if (cb_on_file_load)
        {
            cb_on_file_load();
        }

        return Error::None;
    }

    const std::string Player::get_file_title()
    {
        if ( this->active_file.open )
            return this->active_file.title;
        else
            return "";
    }

    Error Player::play()
    {
        if (!this->playing)
        {
            frames = decoder->GetNFrames(cache_time);
            this->playing = true;
            this->paused = false;
            this->first_frame = true;

            if (cb_on_play_start)
            {
                cb_on_play_start();
            }
            this->decoding_thread = std::thread(&Player::decoding_, this);
        }

        return Error::None;
    }

    void Player::decoding_()
    {
        float time_gap = 0.0f;
        while (this->playing)
        {
            while (frames.size() < 2 || time_gap < cache_time)
            {
                frames.push_back(decoder->GetNextFrame());
                if (frames.size() > 2)
                {
                    time_gap = frames.at(frames.size() - 1).pts - frames.at(0).pts;
                }
            }
        }
    }

    float Player::get_video_ar()
    {
        return this->aspect_ratio;
    }

    float Player::get_duration()
    {
        return this->duration;
    }

    void Player::draw()
    {
        this->video->draw();
    }

    void Player::update_frame()
    {
        if (frames.size() > 0 && this->current_time < this->duration && !this->paused && this->playing)
        {
            frame = frames.at(0);
            
            if (first_frame)
            {
                first_frame = false;
                start_time = Time::now();
            }

            auto diff = std::chrono::duration_cast<std::chrono::duration<float>>( Time::now() - start_time );

            if (frame.pts <= diff.count())
            {
                if (frame.type == Media::FrameType::Video)
                {
                    this->video->update_data(&frame.videoFrame);
                    this->video->draw();
                }
                else if (frame.type == Media::FrameType::Audio)
                {
                    audio->play(&frame.audioFrame);
                }

                this->current_time = frame.pts;
                if (this->cb_on_time_change)
                {
                    cb_on_time_change(this->current_time, this->duration);
                }

                frames.erase(frames.begin());
            }
        }
    }

    void Player::pause()
    {
        this->pause_time = Time::now();
        this->paused = true;
        if(this->cb_on_toggle_play)
        this->cb_on_toggle_play(this->paused);
    }

    void Player::unpause()
    {
        this->start_time = Time::time_point(Time::now() - this->pause_time);
        this->paused = false;
        if(this->cb_on_toggle_play)
        this->cb_on_toggle_play(this->paused);
    }

    void Player::stop()
    {
        this->playing = false;
        this->paused = true;
        this->first_frame = true;
        if (this->decoding_thread.joinable())
            this->decoding_thread.join();
        if (this->audio)
        this->audio->close_device();
        if(this->cb_on_play_stop)
        cb_on_play_stop();
    }

    bool Player::is_playing()
    {
        return this->playing;
    }

    bool Player::is_paused()
    {
        return this->paused;
    }

    void Player::set_on_time_change(TimeChangeCallBack callback)
    {
        this->cb_on_time_change = callback;
    }

    void Player::set_on_toggle_play(TogglePlayCallBack callback)
    {
        this->cb_on_toggle_play = callback;
    }
    
    void Player::set_on_file_load(VoidCallback callback)
    {
        this->cb_on_file_load = callback;
    }

    void Player::set_on_play_starts(VoidCallback callback)
    {
        this->cb_on_play_start = callback;
    }

    void Player::set_on_play_stops(VoidCallback callback)
    {
        this->cb_on_play_stop = callback;
    }
}