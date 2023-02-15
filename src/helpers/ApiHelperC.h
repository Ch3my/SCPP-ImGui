#include <mutex>

class ApiHelperC {
public:
	Json::Value fn(std::string url, Json::Value json_args, std::string method);
};