#include "utils.hpp"

bool isJsonParseable(const std::string& str) {
    try {
        // Try to parse the string as JSON
        json::parse(str);
        return true;
    } catch (const json::parse_error& e) {
        // If a parse error is thrown, the string is not valid JSON
        return false;
    }
}

bool checkJsonFormat(const json& msg_json, const json& ref_json) {
    // Extract keys from both JSON objects
    std::set<std::string> keys_msg;
    std::set<std::string> keys_json;

    for (auto it = msg_json.begin(); it != msg_json.end(); ++it)
        keys_msg.insert(it.key());
    for (auto it = ref_json.begin(); it != ref_json.end(); ++it)
        keys_json.insert(it.key());

    // Check if both sets of keys are equal
    if (keys_msg != keys_json) {
        return false;
    }

    // Check type of each key's value
    for (const auto& key : keys_json) {
        if (ref_json[key].is_object()) {
            // Recursively check nested objects
            if (!checkJsonFormat(msg_json[key], ref_json[key])) {
                return false;
            }
        } else {
            // Check if the types match for non-object values
            if (msg_json[key].type() != ref_json[key].type()) {
                return false;
            }
        }
    }

    // All keys and their types match
    return true;
}


std::string motorID_to_string(MotorID id) {
    switch (id) {
        case MotorID::FSX:    return "FSX";
        case MotorID::FDX:    return "FDX";
        case MotorID::RSX:    return "RSX";
        case MotorID::RDX:    return "RDX";
        case MotorID::UPFSX:  return "UPFSX";
        case MotorID::UPFDX:  return "UPFDX";
        case MotorID::UPRSX:  return "UPRSX";
        case MotorID::UPRDX:  return "UPRDX";
        default:              return "DEFAULT";
    }
}

std::string floatToStringWithDecimals(float value, int n) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << value;
    return out.str();
}
