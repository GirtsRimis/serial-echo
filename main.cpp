#include "Logger.h"
#include "AsyncSerialServer.h"

int main(int argc, char* argv[])
{
    Log syslog("serial-echo", LOG_LOCAL0);

    // attach std::clog to syslog
    Attach_rdbuf attach( std::clog, &syslog );

    // output to clog will appear in syslog
    // output to cout will appear in stdout and syslog
    Tee tee( std::cout, std::clog );

    try
    {
        SerialPortInformation portInformation(argc, argv);

        std::cout << "Opening port: " << portInformation.portName << std::endl;

        boost::asio::io_context io_context;

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(boost::bind(&boost::asio::io_context::stop, &io_context));

        AsyncSerialServer serialPort(io_context, portInformation);
        io_context.run();

        std::cout << "Exiting try catch in main!" << std::endl;
    }
    catch(const boost::system::system_error& e)
    {
        std::cerr << "[ERROR]: " << e.what() << ": " << e.code() << " - " << e.code().message() << std::endl;
    }

    std::cout << "Closing app" << std::endl;

    return 0;
}
