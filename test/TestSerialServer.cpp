#include "TestSerialServer.h"

TestSerialServer::TestSerialServer(boost::asio::io_context &io_context, SerialPortInformation &portInformation)
    : SerialServerBase(io_context, portInformation)
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "TestSerialServer created!" << std::endl;
}

TestSerialServer::~TestSerialServer()
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "TestSerialServer destroyed!" << std::endl;
}

void TestSerialServer::readData(char endChar, std::vector<char> &inputVector)
{
    boost::system::error_code error;
    boost::asio::read_until(this->serialPort, this->dataBuffer, endChar, error);
    getBufferData(inputVector);

    if (error) throw error;
}

void TestSerialServer::writeData(std::vector<char> &sendString)
{
    boost::system::error_code error;
    boost::asio::write(this->serialPort, boost::asio::buffer(sendString, sendString.size()), error);
    
    if (error) throw error;
}

void TestSerialServer::manageModemStatus(unsigned int signal)
{
    this->modemStatus = getModemStatus();
    
    if (this->modemStatus != 0)
    {   
        setModemStatus(signal, this->toggleSignal);
        this->toggleSignal =  !this->toggleSignal;
    }
    else
        if (this->portInformation.debugLevel == 1) 
            std::cout << "Skipped signal" << std::endl;
}

void TestSerialServer::getBufferData(std::vector<char> &inputVector)
{
    std::istream inputStream(&this->dataBuffer);
    inputVector.assign(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>());
}
