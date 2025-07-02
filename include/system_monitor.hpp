#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>
#include <string>
#include <cstdlib>
#include <json.hpp>
#include "utils.hpp"
// #include <ctime>  // Not strictly needed if std::srand is called once globally

class SystemMonitor {
public:
    SystemMonitor();
    void read_info();
    float get_cpu_temperature() const;
    float get_cpu_usage() const;
    float get_ram_total_mb() const;
    float get_ram_used_mb() const;
    json get_status();

private:
    bool status_ok;
    float cpu_temp_celsius;
    float cpu_usage_percent;
    float ram_total_mb;
    float ram_used_mb;

    unsigned long long prev_cpu_total_time;
    unsigned long long prev_cpu_idle_time;

    float read_cpu_temperature_from_file();
    float calculate_cpu_usage();
    void read_ram_info_from_file();
};

#endif // SYSTEM_MONITOR_HPP
