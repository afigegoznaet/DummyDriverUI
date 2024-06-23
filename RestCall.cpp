#include <Utilities.hpp>
#include <RestCall.hpp>
#include <JuceHeader.h>
#include <json.hpp>
using namespace std::string_literals;

constexpr auto KEYGEN_BASE_URL = "https://api.keygen.sh/v1/accounts/";
constexpr auto ACCOUNT_ID = "72d3fb49-6c80-4694-9175-73a2e1ee4add";
constexpr auto PRODUCT_ID = "bf38932a-fb89-463f-ba17-e0fcca1d69e4";
constexpr auto PRODUCT_VERSION_MAJOR = 1;

size_t wfun(char *contents, size_t size, size_t nmemb, void *userp) {
	((std::string *)userp)->append((char *)contents, size * nmemb);

	return size * nmemb;
}

// Verify a license key using the validate-key action
// https://keygen.sh/docs/api/licenses/#licenses-actions-validate-key
RESTCallResponse validate_license_key(const std::string license_key,
									  const std::string fingerprint) {
	RESTCallResponse result{};

	// auto			   curl = curl_easy_init();
	// struct curl_slist *headers = nullptr;
	auto   parameter_handling = URL::ParameterHandling::inPostData;
	URL	   url(std::string{} + KEYGEN_BASE_URL + ACCOUNT_ID
			   + "/licenses/actions/validate-key");
	String extra_headers =
		"Content-Type: application/vnd.api+json\nAccept: application/vnd.api+json";
	const auto options = URL::InputStreamOptions(parameter_handling)
							 .withExtraHeaders(extra_headers);
	nlohmann::json body = {
		{"meta",
		 {
			 {"key", nullptr},
			 {"scope",
			  {
				  {"fingerprint", nullptr},
			  }},
		 }},
	};
	body["meta"]["key"] = license_key;
	body["meta"]["scope"]["fingerprint"] = fingerprint;
	std::string jsonBody = body.dump();
	auto		stream = url.withPOSTData(jsonBody).createInputStream(options);
	String		response = stream->readString();

	if (!nlohmann::json::accept(response.toStdString())) {
		std::stringstream ss;

		result.errorCode = -1;
		result.error =
			std::format("Failed to parse json: {}", response.toStdString());
		return result;
	}

	auto jsonResp = nlohmann::json::parse(response.toStdString());

	if (jsonResp.contains("errors")) {
		auto err = jsonResp["errors"].front();
		result.errorCode = -1;
		result.error =
			std::format("{}\ndetails: {}", err["title"].get<std::string>(),
						err["detail"].get<std::string>());
		return result;
	}

	nlohmann::json data = jsonResp["data"];
	nlohmann::json meta = jsonResp["meta"];

	std::cerr << data.dump() << std::endl;
	std::cerr << std::endl << std::endl;
	std::cerr << meta.dump() << std::endl;
	if (!meta.contains("valid") || !meta["valid"].get<bool>()) {

		result.errorCode = -1;
		result.error =
			std::format("Details: {}\nLicense status: {}",
						meta["detail"].get<std::string>(),
						data["attributes"]["status"].get<std::string>());

		return result;
	}

	// Check expiry
	if (data.contains("attributes") && data["attributes"].contains("expiry")
		&& !data["attributes"]["expiry"].is_null()) {
		result.expiry = data["attributes"]["expiry"].get<std::string>();
		if (!isDatePastToday(result.expiry)) {
			result.errorCode = -1;
			result.error = std::format("Key expired on: {}", result.expiry);
			return result;
		}
	} else {
		result.errorCode = -1;
		result.error =
			std::format("Invalid data in response, expiration not parseable");
		return result;
	}

	// Check if meta data is included
	if (data.contains("attributes")
		&& data["attributes"].contains("metadata")) {
		// Check license key version
		if (data["attributes"]["metadata"].contains("version")
			&& data["attributes"]["metadata"]["version"].get<int>()
				   != PRODUCT_VERSION_MAJOR) {
			result.errorCode = -1;
			result.error = std::format(
				"You have entered a key for another version of this product.");
			return result;
		}
	} else {
		result.errorCode = -1;
		result.error = "You have entered an invalid license key.";
		return result;
	}

	return result;
}
