#include <iostream>
#include "SerialServerBase.h"

SerialServerBase::SerialServerBase(boost::asio::io_context& io_context, SerialPortInformation& portInformation)
    : portInformation(portInformation)
    , serialPort(io_context, portInformation.portName)
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "SerialServerBase created!" << std::endl;

    setupPort(this->serialPort, this->portInformation.baudRate);
}

SerialServerBase::~SerialServerBase()
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "SerialServerBase destroyed!" << std::endl;
}

void SerialServerBase::setupPort(boost::asio::serial_port& serialPort, unsigned long baudRate)
{
    serialPort.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
    serialPort.set_option(boost::asio::serial_port_base::character_size(8));
    serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    this->fd = serialPort.native_handle();
}

void SerialServerBase::setModemStatus(unsigned int signal, bool value)
{
    int returnCode = ioctl(this->fd, value ? TIOCMBIS : TIOCMBIC, &signal);

    std::string signalType = modemStatusBitsToString(signal);

    std::string action = value ? (" ("+OutputBitsFormat::EMPTY+") ->"+signalType) : (signalType+"-> ("+OutputBitsFormat::EMPTY+")");

    if (returnCode < 0)
        throw boost::system::system_error(returnCode, boost::system::system_category(), action + " couldn\'t be applied");

    if (this->portInformation.debugLevel == 1)
        std::cout << "ModemStatus SET: " << action << std::endl;
}

int SerialServerBase::getModemStatus()
{
    int modemData = 0;
    int returnCode = ioctl(this->fd, TIOCMGET, &modemData);

    if (returnCode < 0)
        throw boost::system::system_error(returnCode, boost::system::system_category(), "Failed to TIOCMGET");

    if (this->portInformation.debugLevel == 1)
        std::cout << "ModemStatus CUR: " << modemStatusBitsToString(modemData,OutputBitsFormat::eShowHex|OutputBitsFormat::eShowZeroBits) << std::endl;

    return modemData;
}

int SerialServerBase::getPortFD() const
{
    return this->fd;
}
