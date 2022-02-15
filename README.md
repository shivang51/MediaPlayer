# Media Player

> Under Development

C++ media player which uses **ffmepg** for decoding frames,
**opengl** to display video, **portaudio** to play audio and **dear imgui** for graphical user interface (GUI) .

![Media Player Screenshot](./assets/images/app.png "media player playing a video")

## **To get started**

- ## **Clone it**

  Make sure to clone it with ( `--recurse-submodules` ):

  ```bat
  git clone --recurse-submodules https://github.com/shivang51/MediaPlayer.git
  ```

  Now you should have a folder **MediaPlayer**.

- ## **Build it**

  To build it you will require `cmake VERSION 3.10 (Minimum)`.

  After cloning the repository move into it by,

  ```bat
  cd MediaPlayer
  ```

  After moving into **Media Player**, make folder **build** and move into it by,

  ```bat
  mkdir build && cd build
  ```

  And now use `cmake` to generate files.

  ```bat
  cmake ..
  ```

  For building the genrated files you can use

  ```bat
  cmake --build .
  ```

- ## **Before Running**

  After building generated files don't forget to copy **assets** from `MediaPlayer` and **dlls** from `MediaPlayer\vendors\ffmpeg\bin\x64` into app build directory i.e. `MediaPlayer\build\bin\Debug` in this case.
