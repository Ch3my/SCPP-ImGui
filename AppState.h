#pragma once

#include <stdio.h>
#include <string>

namespace AppState
{
	extern std::string route;
	extern std::string sessionHash;
	extern std::string apiPrefix;
	extern bool showConfig;
	extern bool showSingleDoc;
	extern bool showBarGraph;
	extern bool showLineGraph;

	void load_state_from_file();
	void save_state_to_file();
}