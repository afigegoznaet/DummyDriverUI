#pragma once

#include <string>
#include <functional>

struct RESTCallResponse {
	int			errorCode{};
	std::string error;
	std::string license;
	std::string expiry;
};

RESTCallResponse validate_license_key(const std::string license_key,
									  const std::string machineFingerprint);
RESTCallResponse activate_machine(const std::string licenseID,
								  const std::string machineFingerprint);
RESTCallResponse deactivate_machine(const std::string machineFingerprint);

// void activation_logic(
// 	const std::string license_key, const std::string machineFingerprint,
// 	std::function<void(std::string, std::string, std::string, bool)>
// 		user_result_message);

// void validation_logic(
// 	const std::string license_key, const std::string machineFingerprint,
// 	std::function<void(std::string, std::string, std::string, bool)>
// 		user_result_message);

// void deactivation_logic(const std::string		  machineFingerprint,
// 						std::function<void(bool)> user_result_message);
