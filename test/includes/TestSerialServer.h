#pragma once

#include "utility.h"
#include "SerialServerBase.h"

class TestSerialServer : public SerialServerBase
{
private:
    boost::asio::streambuf dataBuffer;
    bool toggleSignal = true;

public:
    TestSerialServer(boost::asio::io_context &io_context, SerialPortInformation &portInformation);
    ~TestSerialServer();

    void readData(char endChar, std::vector<char> &inputVector);
    void writeData(std::vector<char> &sendString);
    void manageModemStatus(unsigned int signal) override;

    void getBufferData(std::vector<char> &inputVector);
};
