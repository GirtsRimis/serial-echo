#include <iostream>
#include "AsyncSerialServer.h"

AsyncSerialServer::AsyncSerialServer(boost::asio::io_context& io_context, SerialPortInformation& portInformation)
    : SerialServerBase(io_context, portInformation)
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "AsyncSerialServer created!" << std::endl;

    this->modemStatusManagmentWorkerThread = boost::thread(boost::bind(&AsyncSerialServer::modemStatusManagementThread, this));

    startRead();
}

AsyncSerialServer::~AsyncSerialServer()
{
    if (this->portInformation.debugLevel == 1)
        std::cout << "Closing serialPort!" << std::endl;

    this->serialPort.close();

    if (this->portInformation.debugLevel == 1)
        std::cout << "Closed serialPort!" << std::endl;

    if (this->portInformation.debugLevel == 1)
        std::cout << "Killing Thread!" << std::endl;

    pthread_kill(this->modemStatusManagmentWorkerThread.native_handle(), SIGTERM);

    if (this->portInformation.debugLevel == 1)
        std::cout << "Thread killed!" << std::endl;

    if (this->portInformation.debugLevel == 1)
        std::cout << "AsyncSerialServer destroyed!" << std::endl;
}

void AsyncSerialServer::startRead()
{
    this->serialPort.async_read_some(boost::asio::buffer(this->dataBuffer, BUFFER_SIZE),
        boost::bind(&AsyncSerialServer::handleRead, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void AsyncSerialServer::startWrite(size_t length)
{
    boost::asio::async_write(this->serialPort, boost::asio::buffer(this->dataBuffer, length),
        boost::bind(&AsyncSerialServer::handleWrite, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

// void AsyncSerialServer::manageModemStatus(unsigned int signal, unsigned int recievedSignal)
// {
//     this->modemStatus = getModemStatus();

//     if (this->modemStatus != 0)
//     {
//         setModemStatus(signal, this->modemStatus & recievedSignal);
//     }
//     else
//         if (this->portInformation.debugLevel == 1)
//             std::cout << "manageModemStatus: the same" << std::endl;
// }

void AsyncSerialServer::handleRead(const boost::system::error_code& error, size_t length)
{
    if (this->portInformation.debugLevel == 1)
        printInformation("Read", error, length);

    startWrite(length);
}

void AsyncSerialServer::handleWrite(const boost::system::error_code& error, size_t length)
{
    if (this->portInformation.debugLevel == 1)
        printInformation("Write", error, length);

    startRead();
}

void AsyncSerialServer::printInformation(const char* messageType, const boost::system::error_code& error, size_t length)
{
    if (!error)
    {
        boost::io::ios_all_saver  ias( std::cout );

        std::cout << messageType << " message: ";
        for (size_t i = 0; i < length; i++)
        {
            if(isprint(this->dataBuffer[i]))
                std::cout << this->dataBuffer[i];
            else
            {
                auto ch = static_cast<int>(0xFF&this->dataBuffer[i]);
                switch (ch)
                {
                case 0:
                    std::cout << "[\\0]";
                    break;

                case 10:
                    std::cout << "[\\n]";
                    break;

                case 13:
                    std::cout << "[\\r]";
                    break;

                case 9:
                    std::cout << "[\\t]";
                    break;

                case 11:
                    std::cout << "[\\v]";
                    break;

                case 8:
                    std::cout << "[\\b]";
                    break;

                case 12:
                    std::cout << "[\\f]";
                    break;

                case 7:
                    std::cout << "[\\a]";
                    break;

                default:
                    std::cout << "[" << std::hex << std::uppercase << ch << "]";
                    break;
                }
            }
        }

        ias.restore();

        std::cout << " | Message length: " << length << std::endl;
    }
    else
    {
        std::cerr << "[Error]: Handle " << messageType << "! | " << "Error: " << error <<
            " | modemData length: " << length << " - Must be " << BUFFER_SIZE << " bytes!" << std::endl;
        throw error;
    }
}

void AsyncSerialServer::modemStatusManagementThread()
{
    try
    {
        int returnCode = 0;
        int modemData = 0;
        int prevModemData = 0;

        if (this->portInformation.debugLevel == 1)
            std::cout << "Thread started" << std::endl;

        while (returnCode >= 0)
        {
            returnCode = ioctl(this->fd, TIOCMGET, &modemData);    

            if ((prevModemData & TIOCM_CTS) != (modemData & TIOCM_CTS))
                setModemStatus(TIOCM_RTS, modemData & TIOCM_CTS);
            
            if ((prevModemData & TIOCM_DSR) != (modemData & TIOCM_DSR))
                setModemStatus(TIOCM_DTR, modemData & TIOCM_DSR);
            
            prevModemData = modemData;
            returnCode = ioctl( this->fd, TIOCMIWAIT, TIOCM_CTS|TIOCM_DSR );

            // if (returnCode < 0)
            //     throw boost::system::system_error(returnCode, boost::system::system_category(), "Failed to TIOCMIWAIT");
            
            // if (returnCode < 0)
            //     throw boost::system::system_error(returnCode, boost::system::system_category(), "Failed to TIOCMGET");         
        }

        if (this->portInformation.debugLevel == 1)
            std::cout << "Thread finished" << std::endl;
    }
    catch(const boost::system::system_error& e)
    {
        std::cerr << "[ERROR]: " << e.what() << ": " << e.code() << " - " << e.code().message() << std::endl;
    }
}
