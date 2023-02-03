#pragma once

#include <future>
#include <functional>

namespace TimerC {
	std::future<int> fn(int delay, std::function<int()> func);
}

