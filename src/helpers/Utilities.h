#pragma once

#include <vector>
#include <string>

namespace Utilities {
	std::vector<std::string> SplitString(std::string str, std::string delimeter);
	std::string get_digits(const std::string& input, const bool& keep_dot);
	std::string remove_letters(const std::string& input);
	void SetTmDate(tm& date, int& year, int& month, int& day);
	std::string get_current_time_and_date();
	std::vector<std::string> GetCurrentMonthRange();
	std::vector<std::string> GetCurrentYearRange();
}