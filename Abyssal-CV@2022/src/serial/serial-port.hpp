//
// Created by sleepingmachine on 2022/2/26.
//

#ifndef ABYSSAL_CV_2022_SERIAL_PORT_HPP
#define ABYSSAL_CV_2022_SERIAL_PORT_HPP

#include "../asset/RoboMasterConfig.hpp"

#include <opencv2/core/persistence.hpp>

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string>

class SerialPort{
public:
    SerialPort();
    ~SerialPort();
    static void getHitPointData(int hitPointData_x, int hitPointData_y);

    static void SendData(int* sentData);

private:
    static std::string read_device;
    static std::string write_device;
    static int baud_write;
    static int baud_read;

    static char testData[5];

    static void checkPortAvailability();
    static void getSerialInfo();

};


#endif //ABYSSAL_CV_2022_SERIAL_PORT_HPP
