#include <future>

#include <json.hpp>

const std::string KEYGEN_BASE_URL =
	std::string{"https://api.keygen.sh/v1/accounts/"};
const std::string ACCOUNT_ID =
	std::string{"72d3fb49-6c80-4694-9175-73a2e1ee4add"};
const std::string PRODUCT_ID =
	std::string{"bf38932a-fb89-463f-ba17-e0fcca1d69e4"};

size_t wfun(char *contents, size_t size, size_t nmemb, void *userp) {
	((std::string *)userp)->append((char *)contents, size * nmemb);

	return size * nmemb;
}

// Verify a license key using the validate-key action
// https://keygen.sh/docs/api/licenses/#licenses-actions-validate-key
RESTCallResponse validate_license_key(const std::string license_key,
									  const std::string fingerprint) {
	RESTCallResponse result;

	auto			   curl = curl_easy_init();
	struct curl_slist *headers = nullptr;

	if (curl) {
		::nlohmann::json body = {
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

		headers = curl_slist_append(headers,
									"Content-Type: application/vnd.api+json");
		headers =
			curl_slist_append(headers, "Accept: application/vnd.api+json");

		std::string url =
			KEYGEN_BASE_URL + ACCOUNT_ID + "/licenses/actions/validate-key";

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wfun);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, result.error);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

		result.errorCode = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.httpCode);

		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);
	}

	return result;
}

// Activate this machine for a given license key
// https://keygen.sh/docs/api/machines/#machines-create
RESTCallResponse activate_machine(const std::string licensID,
								  const std::string fingerprint) {
	RESTCallResponse result;

	auto			   curl = curl_easy_init();
	struct curl_slist *headers = nullptr;

	if (curl) {
		std::string bearerToken =
			"dev-9ee22e4aa9e8ca711b214f73797aa10a0c107464016b7d26f6fa8a1519ae5798v3";

		::nlohmann::json body = {
			{"data",
			 {
				 {"type", "machines"},
				 {"attributes",
				  {
					  {"fingerprint", nullptr},
				  }},
				 {"relationships",
				  {
					  {"license",
					   {
						   {"data",
							{
								{"type", "licenses"},
								{"id", nullptr},
							}},
					   }},
				  }},
			 }},
		};
		body["data"]["attributes"]["fingerprint"] = fingerprint;
		body["data"]["relationships"]["license"]["data"]["id"] = licensID;
		std::string jsonBody = body.dump();

		headers = curl_slist_append(
			headers,
			(std::string{"Authorization: Bearer "} + bearerToken).c_str());
		headers = curl_slist_append(headers,
									"Content-Type: application/vnd.api+json");
		headers =
			curl_slist_append(headers, "Accept: application/vnd.api+json");

		std::string url = KEYGEN_BASE_URL + ACCOUNT_ID + "/machines";
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wfun);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, result.error);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

		result.errorCode = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.httpCode);

		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);
	}

	return result;
}

// Deactivate this machine for a given license key
RESTCallResponse deactivate_machine(const std::string fingerprint) {
	RESTCallResponse result;

	auto			   curl = curl_easy_init();
	struct curl_slist *headers = nullptr;

	if (curl) {
		std::string bearerToken =
			"dev-9ee22e4aa9e8ca711b214f73797aa10a0c107464016b7d26f6fa8a1519ae5798v3";

		headers = curl_slist_append(
			headers,
			(std::string{"Authorization: Bearer "} + bearerToken).c_str());
		headers = curl_slist_append(headers,
									"Content-Type: application/vnd.api+json");
		headers =
			curl_slist_append(headers, "Accept: application/vnd.api+json");

		std::string url = KEYGEN_BASE_URL + std::string(ACCOUNT_ID)
						  + "/machines/" + fingerprint;
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wfun);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, result.error);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

		result.errorCode = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.httpCode);

		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);
	}

	return result;
}

void validation_logic(
	const std::string license_key, const std::string machineFingerprint,
	std::function<void(std::string, std::string, std::string, bool)>
		user_result_message) {
	std::string expiryDate = "";

	auto response = std::async(std::launch::async, &validate_license_key,
							   std::string(license_key), machineFingerprint)
						.get();

	if (response.errorCode != CURLE_OK) {
		user_result_message(response.error, "Error", expiryDate, false);
		return;
	}

	if (!::nlohmann::json::accept(response.body)) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "JSON Error on input: " << response.body;
#else
		ss << "An unexpected error occurred";
#endif
		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	auto json = ::nlohmann::json::parse(response.body);

	if (json.contains("errors")) {
		::nlohmann::json err = json["errors"][0];
		user_result_message(err["detail"].get<std::string>(),
							err["title"].get<std::string>(), expiryDate, false);
		return;
	}

	auto data = json["data"];
	auto meta = json["meta"];

	if (!meta.contains("valid") || !meta["valid"].get<bool>()) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "Wrong JSON response body: " << response.body;
#else
		ss << "Invalid license";
#endif

		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	// Check expiry
	if (data.contains("attributes") && data["attributes"].contains("expiry")) {
		if (!data["attributes"]["expiry"].is_null()) {
			expiryDate = data["attributes"]["expiry"].get<std::string>();
			if (!isDatePastToday(expiryDate)) {
				user_result_message("This license key is expired",
									"License Expired", expiryDate, false);
				return;
			}
		}
	}

	// Check if meta data is included
	if (data.contains("attributes")
		&& data["attributes"].contains("metadata")) {
		// Check license key version
		if (data["attributes"]["metadata"].contains("version")
			&& data["attributes"]["metadata"]["version"].get<int>()
				   != PRODUCT_VERSION_MAJOR) {
			user_result_message(
				"You have entered a key for another version of this product.",
				"Wrong Key Version", expiryDate, false);
			return;
		}
	} else {
		user_result_message("You have entered an invalid license key.",
							"Invalid license key", expiryDate, false);
		return;
	}

	user_result_message("License Key activated for this device.", "Success!",
						expiryDate, true);
}

void activation_logic(
	const std::string license_key, const std::string machineFingerprint,
	std::function<void(std::string, std::string, std::string, bool)>
		user_result_message) {
	RESTCallResponse response;
	::nlohmann::json json, meta, data;

	// step 1: Ask for license validity
	response = std::async(std::launch::async, &validate_license_key,
						  std::string(license_key), machineFingerprint)
				   .get();

	std::string expiryDate = "";

	if (response.errorCode != CURLE_OK) {
		user_result_message(response.error, "Error", expiryDate, false);
		return;
	}

	if (!::nlohmann::json::accept(response.body)) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "JSON Error on input: " << response.body;
#else
		ss << "An unexpected error occurred.";
#endif

		user_result_message(ss.str().c_str(), "Error", expiryDate, false);
		return;
	}

	json = ::nlohmann::json::parse(response.body);

	if (json.contains("errors")) {
		::nlohmann::json err = json["errors"][0];
		user_result_message(err["detail"].get<std::string>(),
							err["title"].get<std::string>(), expiryDate, false);
		return;
	}

	if ((!json.contains("data")) || (!json.contains("meta"))) {
		user_result_message("Server response error on validation.", "Error",
							expiryDate, false);
		return;
	}

	data = json["data"];
	meta = json["meta"];

	bool isTrialLicense = data.contains("attributes")
						  && data["attributes"].contains("expiry")
						  && !data["attributes"]["expiry"].is_null();

	// Check product ID, must be ASIO NDI Virtual Soundcard driver's ID
	if (data.contains("relationships")
		&& data["relationships"].contains("product")
		&& data["relationships"]["product"].contains("data")
		&& data["relationships"]["product"]["data"].contains("id")
		&& data["relationships"]["product"]["data"]["id"].get<std::string>()
			   != PRODUCT_ID) {
		user_result_message("Wrong product for the given key.", "Error",
							expiryDate, false);
		return;
	}

	// Check license expiry
	if (isTrialLicense) {
		expiryDate = data["attributes"]["expiry"].get<std::string>();
		if (!isDatePastToday(expiryDate)) {
			user_result_message("This license key is expired.",
								"License Expired", expiryDate, false);
			return;
		}
	}

	// Check if meta data is included
	if (data.contains("attributes")
		&& data["attributes"].contains("metadata")) {
		// Check license key version
		if (data["attributes"]["metadata"].contains("version")
			&& data["attributes"]["metadata"]["version"].get<int>()
				   != PRODUCT_VERSION_MAJOR) {
			user_result_message(
				"You have entered a key for another version of this product.",
				"Wrong Key Version", expiryDate, false);
			return;
		}
	} else {
		user_result_message("You have entered an invalid license key.",
							"Invalid license key", expiryDate, false);
		return;
	}

	// License is valid, nothing to do
	if (meta.contains("valid") && meta["valid"].get<bool>()) {
		user_result_message("License Key is valid.", "Success!", expiryDate,
							true);
		return;
	}

	// Step 2: If license has no associated machines, is not expired and
	// has a valid product version, then activate it
	if (!meta.contains("constant")
		|| meta["constant"].get<std::string>() != "NO_MACHINE") {
		user_result_message("Unexpected license validation state.", "Error",
							expiryDate, false);
		return;
	}

	response = std::async(std::launch::async, &activate_machine,
						  data["id"].get<std::string>(), machineFingerprint)
				   .get();

	if (response.errorCode != CURLE_OK) {
		user_result_message(response.error, "Error", expiryDate, false);
		return;
	}

	if (!::nlohmann::json::accept(response.body)) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "JSON Error on input: " << response.body;
#else
		ss << "An unexpected error occurred";
#endif

		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	json = ::nlohmann::json::parse(response.body);

	if (json.contains("errors")) {
		auto err = json["errors"][0];
		user_result_message(err["detail"].get<std::string>(),
							err["title"].get<std::string>(), expiryDate, false);
		return;
	}

	// expect 201 Created
	if (response.httpCode != 201L) {
		std::stringstream ss;
		ss << "Wrong HTTP response code: " << response.httpCode;
		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	// Step 3: Validate the newly created license against this machine, again
	response = std::async(std::launch::async, &validate_license_key,
						  std::string(license_key), machineFingerprint)
				   .get();

	if (response.errorCode != CURLE_OK) {
		user_result_message(response.error, "Error", expiryDate, false);
		return;
	}

	if (!::nlohmann::json::accept(response.body)) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "JSON Error on input: " << response.body;
#else
		ss << "An unexpected error occurred";
#endif
		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	json = ::nlohmann::json::parse(response.body);

	if (json.contains("errors")) {
		::nlohmann::json err = json["errors"][0];
		user_result_message(err["detail"].get<std::string>(),
							err["title"].get<std::string>(), expiryDate, false);
		return;
	}

	data = json["data"];
	meta = json["meta"];

	if (!meta.contains("valid") || !meta["valid"].get<bool>()) {
		std::stringstream ss;

#ifdef _DEBUG
		ss << "Wrong JSON response body: " << response.body;
#else
		ss << "Invalid license";
#endif

		user_result_message(ss.str(), "Error", expiryDate, false);
		return;
	}

	if (data.contains("attributes") && data["attributes"].contains("expiry")) {
		if (!data["attributes"]["expiry"].is_null()) {
			expiryDate = data["attributes"]["expiry"].get<std::string>();
			if (!isDatePastToday(expiryDate)) {
				user_result_message("This license key is expired",
									"License Expired", expiryDate, false);
				return;
			}
		}
	}

	user_result_message("License Key activated for this device.", "Success!",
						expiryDate, true);
}

void deactivation_logic(const std::string		  machineFingerprint,
						std::function<void(bool)> user_result_message) {
	RESTCallResponse response;

	response =
		std::async(std::launch::async, &deactivate_machine, machineFingerprint)
			.get();
	user_result_message(response.httpCode == 204);
}
