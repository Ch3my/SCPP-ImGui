#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp)
	{
		((std::string*)userp)->append((char*)response_string, size * nmemb);
		return size * nmemb;
	}

	std::string api_call(std::string url)
	{
		CURL* curl;
		CURLcode res;
		std::string response_string;

		curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		return response_string;
	}


	int fn()
	{
		std::string json_string = api_call("https://dummyjson.com/products/1");

		Json::Value json_data;
		Json::Reader json_reader;

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

		return 0;
	}
}