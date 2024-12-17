#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <json.hpp>
#include "motorID.hpp"

using json = nlohmann::json;

std::string motorID_to_string(MotorID id);
std::string floatToStringWithDecimals(float value, int n);
bool isJsonParseable(const std::string& str);
bool checkJsonFormat(const json& msg_json, const json& ref_json);

#endif
