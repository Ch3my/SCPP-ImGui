#include <string>
#include <iostream>
#include <mutex>
#include <map>
#include <curl/curl.h>
#include <json/json.h>

#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_ios.h>

class ApiHelperC {
public:
	Json::Value fn(std::string url, Json::Value json_args, std::string method);
	std::string api_call_get(std::string url, Json::Value json_args);
	std::string json_to_url_params(Json::Value json);
	std::string api_call_custom(std::string url, Json::Value json_args, const char* method);
	std::string json_to_string(Json::Value json);
};

std::string ApiHelperC::json_to_string(Json::Value json) {
	Json::StreamWriterBuilder builder;
	builder["indentation"] = ""; // If you want whitespace-less output
	const std::string output = Json::writeString(builder, json);
	return output;
}

std::string ApiHelperC::json_to_url_params(Json::Value json) {
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
	return result;
}

std::string ApiHelperC::api_call_custom(std::string url, Json::Value json_args, const char* method) {
	std::ostringstream response_string;
	// Create a curl_ios object, passing the stream object.
	curl::curl_ios<std::ostringstream> writer(response_string);
	curl::curl_easy easy(writer);
	// Tenemos que enviar un JSON
	std::string parsed_args = json_to_string(json_args);

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	easy.add<CURLOPT_HTTPHEADER>(headers);
	easy.add<CURLOPT_CUSTOMREQUEST>(method);
	easy.add<CURLOPT_POSTFIELDS>(parsed_args.c_str());

	easy.add<CURLOPT_URL>(url.c_str());

	try {
		easy.perform();
	}
	catch (curl::curl_easy_exception& error) {
		// If you want to print the last error.
		std::cerr << error.what() << std::endl;
	}

	return response_string.str();
}

std::string ApiHelperC::api_call_get(std::string url, Json::Value json_args) {
	std::ostringstream response_string;
	std::string processed_req_string = url + "?" + json_to_url_params(json_args);

	// Create a curl_ios object, passing the stream object.
	curl::curl_ios<std::ostringstream> writer(response_string);
	curl::curl_easy easy(writer);

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	easy.add<CURLOPT_HTTPHEADER>(headers);
	easy.add<CURLOPT_URL>(processed_req_string.c_str());

	try {
		easy.perform();
	}
	catch (curl::curl_easy_exception& error) {
		// If you want to print the last error.
		std::cerr << error.what() << std::endl;
	}

	return response_string.str();
}

Json::Value ApiHelperC::fn(std::string url, Json::Value json_args, std::string method) {
	std::string json_string;
	Json::Value json_data;
	Json::Reader json_reader;

	if (method == "POST" || method == "DELETE" || method == "PUT") {
		json_string = api_call_custom(url, json_args, method.c_str());
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
		std::cout << "Error parsing JSON calling: " << method << " " << url << std::endl;
		std::cout << json_args << std::endl;
		std::cout << "API respondio " << json_string << std::endl;
	}
	return json_data;
}