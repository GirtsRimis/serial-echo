#include <signal.h>

#include "Logger.h"
#include "AsyncSerialServer.h"

volatile sig_atomic_t done = 0;

void term(int signum)
{
    done = 1;
}

int main(int argc, char* argv[])
{
    // struct sigaction action;
    // memset(&action, 0, sizeof(struct sigaction));
    // action.sa_handler = term;
    // sigaction(SIGTERM, &action, NULL);

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

        std::cout << "Closing port" << std::endl;
    }
    catch(const boost::system::system_error& e)
    {
        std::cerr << "[ERROR]: " << e.what() << ": " << e.code() << " - " << e.code().message() << std::endl;
    }

    return 0;
}
