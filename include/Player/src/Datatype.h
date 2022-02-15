#pragma once

#include "Datatypes.h"
#include <chrono>
#include <functional>

namespace Player
{
	enum class Error
	{
		None = 0,
		File_Not_Found,
		Unknown
	};

	typedef std::function<void(float, float)> TimeChangeCallBack;
	typedef std::function<void(bool)> TogglePlayCallBack;
	typedef std::function<void()> FileLoadCallback;
	typedef std::function<void()> PlayStartCallback;

	typedef DataType::MediaFrame Frame;

	typedef std::chrono::high_resolution_clock Time;
}