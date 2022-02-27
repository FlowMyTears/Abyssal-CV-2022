//
// Created by sleepingmachine on 2022/2/26.
//

//DEMOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
#include "serial-port.hpp"

static SerialConfig serialConfig = SerialConfigFactory::getSerialConfig();
SerialPort::SerialPort() {}

std::string SerialPort::read_device;
std::string SerialPort::write_device;
int SerialPort::baud_write;
int SerialPort::baud_read;

void SerialPort::SerialCommunication() {
    getSerialInfo();
    checkPortAvailability();
}

char SerialPort::testData[6];

void SerialPort::sendData() {
    int fd; /*File Descriptor*/

    printf("\n +----------------------------------+");
    printf("\n |        Serial Port Write         |");
    printf("\n +----------------------------------+");

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) /* Error Checking */
        printf("\n  Error! in Opening ttyUSB0  ");
    else
        printf("\n  ttyUSB0 Opened Successfully ");

    struct termios SerialPortSettings;

    tcgetattr(fd, &SerialPortSettings);

    //设置波特率
    cfsetispeed(&SerialPortSettings, B115200);
    cfsetospeed(&SerialPortSettings, B115200);

    //设置没有校验
    SerialPortSettings.c_cflag &= ~PARENB;

    //停止位 = 1
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;

    //设置数据位 = 8
    SerialPortSettings.c_cflag |= CS8;

    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;

    //关闭软件流动控制
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);

    //设置操作模式
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    SerialPortSettings.c_oflag &= ~OPOST;

    if ((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0)
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");

    //定义传输内容
    char write_buffer[] = "Hello World";
    //传输字节数
    int bytes_written = 0;

    //串口写数据
    bytes_written = write(fd, SerialPort::testData, sizeof(write_buffer));
    printf("\n  %s written to ttyUSB0", SerialPort::testData);
    printf("\n  %d Bytes written to ttyUSB0", bytes_written);
    printf("\n +----------------------------------+\n\n");

    close(fd);
}

void SerialPort::getHitPointData(int hitPointData) {
    for (int i = 0; i < 6; i++)
    {
        SerialPort::testData[i] = (hitPointData >> (8 * i)) & 0xff;
    }
    sendData();
}

void SerialPort::checkPortAvailability()
{
    std::cout << "目前设定的读出串口 : " << read_device << "\n";
    std::cout << "目前设定的写入串口 : " << write_device << "\n";

    int fd   = open(read_device.c_str(), O_EXCL, NULL);
    bool ret = false;
    if(fd < 0)
    {
        std::cout << "读出串口 [" + read_device + "] 无响应" << "\n";
        ret = true;
    }
    close(fd);

    fd = open(write_device.c_str(), O_EXCL, NULL);
    if(fd < 0)
    {
        std::cout << "写入串口 [" + write_device + "] 无响应" << "\n";
        ret = true;
    }
    close(fd);

    if(ret)
    {
        std::cout << "当前在线的串口 : " << "\n";
        system("ls -l /dev/ttyUSB*");
        //exit(0);
    }
}

void SerialPort::getSerialInfo() {
    read_device = serialConfig.readPortPath;
    write_device = serialConfig.writePortPath;
    baud_read = serialConfig.baud_readPort;
    baud_write = serialConfig.baud_writePort;
    std::cout << "getinfo" << " " << "baud set:" << baud_read << std::endl;
}
