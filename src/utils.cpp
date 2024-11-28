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

bool checkJsonFormat(const std::string msg, json ref_json) {
    int correspond = 0;
    if (!isJsonParseable(msg))
            return false;

    json msg_json = json::parse(msg);

    std::set<std::string> keys_msg;
    std::set<std::string> keys_json;

    for (auto it = msg_json.begin(); it!=msg_json.end(); ++it)
        keys_msg.insert(it.key());
    for (auto it = ref_json.begin(); it!=ref_json.end(); ++it)
        keys_json.insert(it.key());

    if (keys_msg == keys_json) {
        for (const auto& str : keys_json) {
            if(ref_json[str].is_object())
                if(!checkJsonFormat(msg_json[str].dump(), ref_json[str]))
                    return false;

            if (msg_json[str].type() == ref_json[str].type()) correspond++;
        }
    }

    // return keys_msg == keys_json
    return correspond == keys_json.size();
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
