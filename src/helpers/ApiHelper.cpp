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

	std::string json_to_string(Json::Value json) {
		Json::StreamWriterBuilder builder;
		builder["indentation"] = ""; // If you want whitespace-less output
		const std::string output = Json::writeString(builder, json);
		return output;
	}

	std::string json_to_url_params(Json::Value json) {
		// como se pasan por URL finalmente todos son
		// strings, solo verificamos para evitar que se caiga
		// el bodyParser al otro lado deberia encargarse de entender que alguno 
		// puede ser numero
		std::string result;
		for (auto it = json.begin(); it != json.end(); ++it) {
			if (it->isString()) {
				result += it.name() + "=" + it->asString() + "&";
			}
			if (it->isInt()) {
				result += it.name() + "=" + std::to_string(it->asInt()) + "&";
			}
			if (it->isArray()) {
				std::string field_name = it.name();
				for (auto deep = it->begin(); deep != it->end(); deep++) {
					if (deep->isInt()) {
						result += field_name + "[]=" + std::to_string(deep->asInt()) + "&";
					}
				}
			}
		}
		// Elimna ultimo & que quedo
		result.pop_back();
		//std::cout << result << std::endl;
		return result;
	}

	std::string build_params(std::map<std::string, std::string> url_args) {
		std::string result;

		//  C++11 loop
		//for (auto const& x : url_args) {
		//	processed_req_string += x.first + "=" + x.second + "&";
		//}

		for (auto const& [key, val] : url_args)
		{
			result += key + "=" + val + "&";
		}
		// Elimina ultimo & que queda
		result.pop_back();
		std::cout << result << std::endl;
		return result;
	}

	std::string api_call_get(std::string url, Json::Value json_args)
	{
		// Como todo lo que se pase por url finalmente se pasa como string
		// lo tratamos como string, Ademas la unica manera de pasar parametros en un GET
		// usando libCurl es a traves de la URL

		CURL* curl;
		CURLcode res;
		std::string response_string;
		std::string processed_req_string = url + "?" + json_to_url_params(json_args);
		// std::cout << processed_req_string << std::endl;
		
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
		curl_global_cleanup();
		//std::cout << response_string;
		return response_string;
	}

	std::string api_call_post(std::string url, Json::Value json_args) {
		CURL* curl;
		CURLcode res;
		std::string response_string;

		// Libcurl es C no C++, no podemos usar std::string. 

		// Tenemos que enviar un JSON
		std::string parsed_args = json_to_string(json_args);

		/* In windows, this will init the winsock stuff */
		curl_global_init(CURL_GLOBAL_ALL);

		/* get a curl handle */
		curl = curl_easy_init();
		if (curl) {
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type: application/json");
			headers = curl_slist_append(headers, "charset: utf-8");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			/* Now specify the POST data */
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parsed_args.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			}

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();
		std::cout << response_string << std::endl;
		return response_string;
	}

	Json::Value fn(std::string url,
		Json::Value json_args,
		std::string method)
	{
		std::string json_string;
		Json::Value json_data;
		Json::Reader json_reader;

		if (method == "POST") {
			json_string = api_call_post(url, json_args);
		}
		if (method == "GET") {
			json_string = api_call_get(url, json_args);
		}

		if (json_reader.parse(json_string, json_data))
		{
			// json_data now contains the parsed JSON data
			/*for (auto it = json_data.begin(); it != json_data.end(); ++it) {
				if (it->isString()) {
					std::cout << it.key() << ": " << it->asString() << std::endl;
				}
			}*/
		}
		else
		{
			std::cout << "Error parsing JSON" << std::endl;
		}

		return json_data;
	}
}