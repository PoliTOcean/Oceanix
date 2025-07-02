#include "system_monitor.hpp"

SystemMonitor::SystemMonitor()  //TODO add logger
    : status_ok(true), cpu_temp_celsius(0.0f),
      cpu_usage_percent(0.0f), ram_total_mb(0.0f), ram_used_mb(0.0f),
      prev_cpu_total_time(0), prev_cpu_idle_time(0) {
    // Initial read to populate prev_cpu_xxx_time for first cpu usage calculation
    std::ifstream stat_file("/proc/stat");
    std::string line;
    if (std::getline(stat_file, line) && line.rfind("cpu ", 0) == 0) {
        std::istringstream iss(line.substr(4)); // Skip "cpu "
        unsigned long long val;
        std::vector<unsigned long long> times;
        while(iss >> val) times.push_back(val);

        if (times.size() >= 4) { // user, nice, system, idle, [iowait, irq, softirq, steal]
            prev_cpu_idle_time = times[3] + (times.size() > 4 ? times[4] : 0); // idle + iowait
            for(unsigned long long t : times) prev_cpu_total_time += t;
        } else {
            status_ok = false; // Failed to initialize CPU times
        }
    } else {
        status_ok = false; // Failed to read /proc/stat
    }
}

void SystemMonitor::read_info() {
    cpu_temp_celsius = read_cpu_temperature_from_file();
    cpu_usage_percent = calculate_cpu_usage();
    read_ram_info_from_file();
}

float SystemMonitor::get_cpu_temperature() const { return cpu_temp_celsius; }
float SystemMonitor::get_cpu_usage() const { return cpu_usage_percent; }
float SystemMonitor::get_ram_total_mb() const { return ram_total_mb; }
float SystemMonitor::get_ram_used_mb() const { return ram_used_mb; }

float SystemMonitor::read_cpu_temperature_from_file() {
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    if (temp_file.is_open()) {
        int temp_millicelsius;
        temp_file >> temp_millicelsius;
        if (temp_file.fail()) {
            status_ok = false; return -1.0f;
        }
        temp_file.close();
        return static_cast<float>(temp_millicelsius) / 1000.0f;
    }
    status_ok = false;
    return -1.0f; // Error value
}

float SystemMonitor::calculate_cpu_usage() {
    std::ifstream stat_file("/proc/stat");
    std::string line;
    unsigned long long current_total_time = 0, current_idle_time = 0;

    if (std::getline(stat_file, line) && line.rfind("cpu ", 0) == 0) {
        std::istringstream iss(line.substr(4));
        std::vector<unsigned long long> times;
        unsigned long long time_val;
        while (iss >> time_val) times.push_back(time_val);

        if (times.size() >= 4) {
            current_idle_time = times[3] + (times.size() > 4 ? times[4] : 0); // idle + iowait
            for(unsigned long long t : times) current_total_time += t;

            unsigned long long total_diff = current_total_time - prev_cpu_total_time;
            unsigned long long idle_diff = current_idle_time - prev_cpu_idle_time;

            prev_cpu_total_time = current_total_time;
            prev_cpu_idle_time = current_idle_time;

            if (total_diff == 0) return cpu_usage_percent; // No change or error, return previous
            return 100.0f * (total_diff - idle_diff) / total_diff;
        }
    }
    status_ok = false;
    return -1.0f; // Error value
}

void SystemMonitor::read_ram_info_from_file() {
    std::ifstream meminfo_file("/proc/meminfo");
    std::string line;
    long mem_total_kb = -1, mem_free_kb = -1, buffers_kb = -1, cached_kb = -1; // SReclaimable could be added for accuracy

    while (std::getline(meminfo_file, line)) {
        std::string key;
        long value;
        // Minimal parsing: "Key: Value kB"
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;
        key = line.substr(0, colon_pos);
        std::istringstream val_ss(line.substr(colon_pos + 1));
        val_ss >> value;

        if (key == "MemTotal") mem_total_kb = value;
        else if (key == "MemFree") mem_free_kb = value;
        else if (key == "Buffers") buffers_kb = value;
        else if (key == "Cached") cached_kb = value;
    }
    meminfo_file.close();

    if (mem_total_kb != -1 && mem_free_kb != -1 && buffers_kb != -1 && cached_kb != -1) {
        long used_kb = mem_total_kb - mem_free_kb - buffers_kb - cached_kb;
        ram_total_mb = static_cast<float>(mem_total_kb) / 1024.0f;
        ram_used_mb = static_cast<float>(used_kb) / 1024.0f;
    } else {
        status_ok = false;
        ram_total_mb = -1.0f;
        ram_used_mb = -1.0f;
    }
}

json SystemMonitor::get_status() {
    json status;

    status["cpu_temp"] = floatToStringWithDecimals(get_cpu_temperature(), 1);
    status["cpu_usage"] = floatToStringWithDecimals(get_cpu_usage(), 1);
    status["ram_total_mb"] = floatToStringWithDecimals(get_ram_total_mb(), 0);
    status["ram_used_mb"] = floatToStringWithDecimals(get_ram_used_mb(), 0);

    return status;
}