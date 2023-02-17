#pragma once

#include <vector>
#include <string>
#include <json/json.h>

namespace Utilities {
	std::vector<std::string> SplitString(std::string str, std::string delimeter);
	std::string get_digits(const std::string& input, const bool& keep_dot);
	std::string remove_letters(const std::string& input);
	void SetTmDate(tm& date, const int& year, const int& month, const int& day);
	std::string get_current_time_and_date();
	std::vector<std::string> GetCurrentMonthRange();
	std::vector<std::string> GetCurrentYearRange();
	std::string FormatTm(const char* date_format, tm date);
	void SetTmFromTipoDoc(tm& fecha_inicio, tm& fecha_termino, const int& fk_tipo_doc);
	std::string json_to_string(const Json::Value& json);
}