#include <iostream>
#include <string>
#include <vector>

namespace Utilities {
	std::vector<std::string> SplitString(
		std::string str,
		std::string delimeter)
	{
		std::vector<std::string> splittedStrings = {};
		size_t pos = 0;

		while ((pos = str.find(delimeter)) != std::string::npos)
		{
			std::string token = str.substr(0, pos);
			if (token.length() > 0)
				splittedStrings.push_back(token);
			str.erase(0, pos + delimeter.length());
		}

		if (str.length() > 0)
			splittedStrings.push_back(str);
		return splittedStrings;
	}

	std::string get_digits(const std::string& input, const bool& keep_dot)
	{
		// Elimina todo lo que no sea un numero de un string o punto. lo conservamos
		// para decimales
		// si parseamos como int y tiene punto por el separador de miles nos elimina todas las centenas
		// podria ser util tener el punto en otras operaciones asi que dejamos la opcion
		std::string result;
		result.reserve(input.size());

		for (auto c : input) {
			if (keep_dot) {
				if (std::isdigit(c) || c == '.') {
					result.push_back(c);
				}
				continue;
			}
			if (std::isdigit(c)) {
				result.push_back(c);
			}
		}
		// Devuelve como string, receptor tiene que parsear como int o float o lo que sea
		return result;
	}

	std::string remove_letters(const std::string& input)
	{
		// Elimina las letras de un string, conserva numeros y simbolos
		std::string result;
		result.reserve(input.size());

		for (auto c : input) {
			if (!std::isalpha(c)) {
				result.push_back(c);
			}
		}

		return result;
	}

	void SetTmDate(tm& date, int& year, int& month, int& day) {
		memset(&date, 0, sizeof(tm));     // Mandatory for emscripten. Please do not remove!
		date.tm_isdst = -1;
		date.tm_sec = 0;		//	 Seconds.	[0-60] (1 leap second)
		date.tm_min = 0;		//	 Minutes.	[0-59]
		date.tm_hour = 0;		//	 Hours.	[0-23]
		date.tm_wday = 0;     //	 Day of week.	[0-6]
		date.tm_yday = 0;		//	 Days in year.[0-365]

		// El año se cuenta relativo a 1900. Entonces si estamos en 2023 el año es 123 (2023 - 1900)
		date.tm_year = year - 1900;
		// El mes comienza el mes 0
		date.tm_mon = month - 1;
		date.tm_mday = day;
	}

}