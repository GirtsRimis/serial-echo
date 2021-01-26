#pragma once

#include "utility.h"
#include "SerialServerBase.h"

class TestSerialServer : public SerialServerBase
{
private:
    boost::asio::streambuf dataBuffer;
    bool toggleRTS = true;

public:
    TestSerialServer(boost::asio::io_context &io_context, SerialPortInformation &portInformation);
    ~TestSerialServer();

    void readData(char endChar, std::vector<char> &inputVector);
    void writeData(std::vector<char> &sendString);
    void manageRTS() override;

    void getBufferData(std::vector<char> &inputVector);
};
