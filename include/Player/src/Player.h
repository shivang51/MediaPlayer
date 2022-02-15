#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <functional>

#include "Media.h"
#include "Video.h"
#include "Datatype.h"
#include "Audio.h"

namespace Player
{
    class Player {
    public:
        Player();
        ~Player();

        Error load_file(const std::string& file_location );

        const std::string get_file_title();
        Error play();
        float get_video_ar();
        float get_duration();

        void draw();
        void update_frame();
    public:
        void pause();
        void unpause();
        void stop();

        bool is_playing();
        bool is_paused();

    public:

        void set_on_time_change(TimeChangeCallBack callback);
        void set_on_toggle_play(TogglePlayCallBack callback);
        void set_on_file_load(VoidCallback callback);
        void set_on_play_starts(VoidCallback callback);
        void set_on_play_stops(VoidCallback callback);

    private:
        bool file_exist(const std::string& name);

        //Callbacks

        TimeChangeCallBack cb_on_time_change = nullptr;
        TogglePlayCallBack cb_on_toggle_play = nullptr;
        VoidCallback cb_on_file_load = nullptr;
        VoidCallback cb_on_play_start = nullptr;
        VoidCallback cb_on_play_stop = nullptr;

    private:
        DataType::File active_file { };
        Frame frame{ };
        std::vector<Frame> frames{ };

        Media::Decoder* decoder = nullptr;

        Audio* audio = nullptr;
        Video* video = nullptr;

        bool first_frame = true;
        bool paused = false;
        bool playing = false;

        float duration = 0.0f;
        float current_time = 0.0f;
        float aspect_ratio = 0.0f;

        float cache_time = 10.0f;

        Time::time_point start_time{ };
        Time::time_point pause_time{ };

        void decoding_();
        std::thread decoding_thread;
    };
}
