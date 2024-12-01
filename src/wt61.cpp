#include "wt61.hpp"

Wt61::Wt61() : status(0), temperature(0.0), roll(0.0), pitch(0.0), yaw(0.0), acc({0,0,0}), gyro({0,0,0}) {
    char const *dev = "/dev/i2c-1";
    status = WT61P_begin(const_cast<char*>(dev), 0x50);
    if (status != 0)
        printLog(logERROR, "imu init fail");
    else
        printLog(logINFO, "imu init fail");
}

void Wt61::read_sensor() {
    status = WT61P_read_all();
    if (status != 0)
        printLog(logERROR, "imu read fail");

    double roll_new = WT61P_get_roll();
	double pitch_new = WT61P_get_pitch();
    double yaw_new = WT61P_get_yaw();

    //if no connection read are zeros, so exclude them and keep the previous
    if (roll_new != 0)
        pitch = roll_new;  //swap pitch roll
    if (yaw_new != 0)
        yaw = yaw_new;
    if (pitch_new != 0)
        roll = pitch_new;

    acc[0] = WT61P_get_AX();
    acc[1] = WT61P_get_AY();
    acc[2] = WT61P_get_AZ();

    gyro[0] = WT61P_get_GX();
    gyro[1] = WT61P_get_GY();
    gyro[2] = WT61P_get_GZ();

    temperature = WT61P_get_temp();
}

int Wt61::get_status() {
    return status;
}

float Wt61::get_temperature() {
    return temperature;
}

float Wt61::get_roll() {
    return roll;
}

float Wt61::get_pitch() {
    return pitch;
}

float Wt61::get_yaw() {
    return yaw;
}

float* Wt61::get_acc() {
    return acc;
}

float* Wt61::get_gyro() {
    return gyro;
}

void Wt61::printLog(logLevel logtype, std::string message){
    Logger::printLog("IMU   ", logtype, message);
}