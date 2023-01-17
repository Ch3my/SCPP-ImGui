# pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>

namespace FormatNumber {
	std::string format(int number, int outputWidth, char widthFill);
}