#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>

#include "src/helpers/ApiHelper.h"

namespace AppState
{
	std::string route = "/login";
	std::string sessionHash = "";
	std::string apiPrefix = "";
	bool showConfig = false;
	bool showSingleDoc = false;

	void create_localstorage_file() {
		// Crea el archivo y le pone datos vacios. Sera la primera vez que abren la App
		// y no tiene datos
		std::ofstream outdata;
		Json::Value data;

		data["sessionHash"] = "";
		data["apiPrefix"] = "";
		std::string str_data = ApiHelper::json_to_string(data);

		outdata.open("localstorage.json", std::ios::out | std::ios::trunc);

		outdata << str_data;
		outdata.close();
	}

	void save_state_to_file() {
		// Creamos un JSON con los datos que vamos a guardar y escribimos archivo
		std::ofstream outdata;
		// abrimos el archivo sin append, entonces limpia el archivo
		// ofstream file("filename.txt", ios::app); // with append
		outdata.open("localstorage.json", std::ios::out | std::ios::trunc);
		if (!outdata) {
			std::cout << "save_state_to_file: No se pudo arbrir el archivo localstorage.json" << std::endl;
			return;
		}

		// Creamos JSON y stringify para guardar en localstorage.json
		Json::Value data;
		data["sessionHash"] = sessionHash;
		data["apiPrefix"] = apiPrefix;

		std::string str_data = ApiHelper::json_to_string(data);

		outdata << str_data;
		outdata.close();
	}

	void load_state_from_file() {
		// Leemos archivo que contiene estado de la APP y cargamos en las variables
		Json::Value data;
		Json::Reader reader;
		const std::ifstream input_stream("localstorage.json", std::ios_base::binary);
		if (input_stream.fail()) {
			create_localstorage_file();
			std::cout << "load_state_from_file: Error abriendo archivo, ¿no existe? lo creamos" << std::endl;
			throw std::runtime_error("Failed to open file");
			return;
		}
		std::stringstream buffer;
		buffer << input_stream.rdbuf();

		bool parsingSuccessful = reader.parse(buffer.str(), data);
		if (!parsingSuccessful)
		{
			std::cout << "load_state_from_file: Error parsing the string" << std::endl;
			return;
		}
		// Cargamos datos al estado
		apiPrefix = data["apiPrefix"].asString();
		sessionHash = data["sessionHash"].asString();
	}
}