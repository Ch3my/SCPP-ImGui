#include <string>
#include <json/json.h>
#include <map>

namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp);
	Json::Value fn(std::string url, Json::Value json_args, std::string method);
	std::string api_call_get(std::string url, Json::Value json_args);
	std::string api_call_post(std::string url, Json::Value url_args);
	std::string json_to_string(Json::Value json);
}