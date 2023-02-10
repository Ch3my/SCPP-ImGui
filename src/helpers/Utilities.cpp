#include <iostream>
#include <string>
#include <vector>
#include <chrono>

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

	void SetTmDate(tm& date, const int& year, const int& month, const int& day) {
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

	std::string get_current_time_and_date()
	{
		auto const time = std::chrono::current_zone()
			->to_local(std::chrono::system_clock::now());
		return std::format("{:%Y-%m-%d %X}", time);
	}

	std::vector<std::string> GetCurrentMonthRange() {
		// Entrega el primer y ultimo dia de el mes actual
		std::stringstream ss_inicio;
		std::stringstream ss_fin;

		// Se basa en la hora actual para tener el mes y año que corresponde
		const std::chrono::time_point now{ std::chrono::system_clock::now() };
		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(now) };

		// Creamos la fecha inicio
		const std::chrono::year_month_day inicio_mes{ ymd.year() / ymd.month() / 1 };

		// Creamos la fecha final 
		const std::chrono::year_month_day fin_mes{ ymd.year() / ymd.month() / std::chrono::last };

		// Guarda los strings en stream que vamos a accesar para obtener informacion
		ss_inicio << std::format("{:%Y-%m-%d}", inicio_mes);
		ss_fin << std::format("{:%Y-%m-%d}", fin_mes);

		// Crea array que va a devolver
		std::vector<std::string> result = { ss_inicio.str(), ss_fin.str() };
		return result;
	}
	std::vector<std::string> GetCurrentYearRange() {
		// Entrega el primer y ultimo dia de el año actual
		std::stringstream ss_inicio;
		std::stringstream ss_fin;

		// Se basa en la hora actual para tener el mes y año que corresponde
		const std::chrono::time_point now{ std::chrono::system_clock::now() };
		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(now) };

		// Creamos la fecha inicio
		const std::chrono::year_month_day inicio_year{ ymd.year() / 1 / 1 };

		// Creamos la fecha final 
		const std::chrono::year_month_day fin_year{ ymd.year() / 12 / std::chrono::last };

		// Guarda los strings en stream que vamos a accesar para obtener informacion
		ss_inicio << std::format("{:%Y-%m-%d}", inicio_year);
		ss_fin << std::format("{:%Y-%m-%d}", fin_year);

		// Crea array que va a devolver
		std::vector<std::string> result = { ss_inicio.str(), ss_fin.str() };
		return result;
	}

	std::string FormatTm(const char* date_format, tm date) {
		std::stringstream ss;
		// Se uso put_time para poder especificar formato en runtime
		ss << std::put_time(&date, date_format);
		return ss.str();
	}

	void SetTmFromTipoDoc(tm& fecha_inicio, tm& fecha_termino, const int& fk_tipo_doc) {
		// Setea las referencias que nos envian a las fechas que correspondan segun el tipo de Documento
		const std::chrono::time_point now{ std::chrono::system_clock::now() };
		const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(now) };

		//Gastos
		if (fk_tipo_doc == 1) {
			const std::chrono::year_month_day inicio_mes{ ymd.year() / ymd.month() / 1 };
			const std::chrono::year_month_day fin_mes{ ymd.year() / ymd.month() / std::chrono::last };

			SetTmDate(fecha_inicio,
				static_cast<int>(inicio_mes.year()),
				static_cast<unsigned>(inicio_mes.month()),
				static_cast<unsigned>(inicio_mes.day())
			);

			SetTmDate(fecha_termino,
				static_cast<int>(fin_mes.year()),
				static_cast<unsigned>(fin_mes.month()),
				static_cast<unsigned>(fin_mes.day())
			);
		}

		// Ahorros y Ingresos
		if (fk_tipo_doc == 2 || fk_tipo_doc == 3) {
			// El rango de fechas es el año completo
			const std::chrono::year_month_day inicio_year{ ymd.year() / 1 / 1 };
			const std::chrono::year_month_day fin_year{ ymd.year() / 12 / std::chrono::last };

			SetTmDate(fecha_inicio, 
				static_cast<int>(inicio_year.year()), 
				static_cast<unsigned>(inicio_year.month()), 
				static_cast<unsigned>(inicio_year.day())
			);

			SetTmDate(fecha_termino, 
				static_cast<int>(fin_year.year()), 
				static_cast<unsigned>(fin_year.month()), 
				static_cast<unsigned>(fin_year.day())
			);
		}
	}
}