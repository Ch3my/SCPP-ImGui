#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <map>

namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp)
	{
		((std::string*)userp)->append((char*)response_string, size * nmemb);
		return size * nmemb;
	}

	std::string api_call_get(std::string url, std::map<std::string, std::string> url_args)
	{
		// Como todo lo que se pase por url finalmente se pasa como string
		// lo tratamos como string, Ademas la unica manera de pasar parametros en un GET
		// usando libCurl es a traves de la URL

		CURL* curl;
		CURLcode res;
		std::string response_string;
		std::string processed_req_string = url + "?";

		// Agregamos parametros a la URL
		//  C++11 loop
		for (auto const& x : url_args) {
			//printf("x.first %s, x.second %s \n", x.first.c_str(), x.second.c_str());
			processed_req_string += x.first + "=" + x.second + "&";
		}
		//std::cout << processed_req_string << std::endl;

		// Iniciamos cURL y configuramos headers y todo lo necesario
		// para un GET
		curl = curl_easy_init();

		if (curl)
		{
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type: application/json");
			headers = curl_slist_append(headers, "charset: utf-8");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_URL, processed_req_string.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if (http_code >= 300 && res == CURLE_ABORTED_BY_CALLBACK) {
				std::cout << "Fallo al comsumir la API: " << http_code << std::endl;
			}
		}
		//std::cout << response_string;
		return response_string;
	}


	Json::Value fn(std::string url, std::map<std::string, std::string> url_args, std::string method)
	{
		std::string json_string = api_call_get(url, url_args);

		Json::Value json_data;
		Json::Reader json_reader;

		printf("ApiHelper.fn method %s", method.c_str());

		if (json_reader.parse(json_string, json_data))
		{
			// json_data now contains the parsed JSON data
			for (auto it = json_data.begin(); it != json_data.end(); ++it) {
				if (it->isString()) {
					std::cout << it.key() << ": " << it->asString() << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Error parsing JSON" << std::endl;
		}

		return json_data;
	}
}