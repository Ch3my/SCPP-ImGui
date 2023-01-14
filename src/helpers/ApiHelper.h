#include <string>
namespace ApiHelper {
	int callback(void* response_string, size_t size, size_t nmemb, void* userp);
	int fn();
	std::string api_call(std::string url);
}