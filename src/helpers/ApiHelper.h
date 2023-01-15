#include <string>
#include <json/json.h>
#include <map>

namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp);
	Json::Value fn(std::string url, std::map<std::string, std::string> url_args, std::string method);
	std::string api_call_get(std::string url, std::map<std::string, std::string> url_args);
}