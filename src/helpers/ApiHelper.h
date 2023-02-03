#include <string>
#include <json/json.h>
#include <map>

namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp);
	Json::Value fn(std::string url, Json::Value json_args, std::string method);
	std::string api_call_get(std::string url, Json::Value json_args);
	std::string api_call_custom(std::string url, Json::Value json_args, const char* method);
	std::string json_to_string(Json::Value json);
}