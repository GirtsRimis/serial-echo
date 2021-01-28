#define BOOST_TEST_MODULE
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>
#include "TestSerialServer.h"
#include "Logger.h"

struct TestSerialServerFixture
{
public:
    // io_context must be on top to avoid any errors of it being passed into serialServer while not existing.
    boost::asio::io_context io_context;

    SerialPortInformation portInformation;
    TestSerialServer serialServer;

public:
    TestSerialServerFixture()
        : portInformation(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv)
        , serialServer(this->io_context, this->portInformation)
    {
        if (this->portInformation.debugLevel == 1)
            std::cout << "Fixture created!" << std::endl;
    }

    ~TestSerialServerFixture()
    {
        if (this->portInformation.debugLevel == 1)
            std::cout << "Fixture destroyed!" << std::endl;
    }

    void CompareEcho(const char *testString)
    {
        std::cout << "CompareEcho test" << std::endl;

        std::vector<char> bufferData, sendString, testDataVector;

        unsigned int testStringLenght = strlen(testString) + 1;
        char endChar = testString[strlen(testString) - 1];

        sendString.assign(testString, testString + testStringLenght);

        this->serialServer.writeData(sendString);
        this->serialServer.readData(endChar, bufferData);

        makeVector(testDataVector, testString, testStringLenght);

        BOOST_CHECK_EQUAL_COLLECTIONS(bufferData.begin(), bufferData.end(), testDataVector.begin(), testDataVector.end());
    }

    void Test_Modem_Pairing(const char *testString, unsigned int signal, unsigned int recieveSignal, unsigned int expectedValue)
    {
        std::cout << "Test_Modem_Pairing test" << std::endl;

        std::vector<char> bufferData, sendString, testDataVector;

        int modemSignals = 0;
        unsigned int testStringLenght = strlen(testString) + 1;
        char endChar = testString[strlen(testString) - 1];

        sendString.assign(testString, testString + testStringLenght);
        makeVector(testDataVector, testString, testStringLenght);

        this->serialServer.manageModemStatus(signal);

        this->serialServer.writeData(sendString);

        this->serialServer.readData(endChar, bufferData);
        
        modemSignals = this->serialServer.getModemStatus();

        BOOST_CHECK_EQUAL(modemSignals & recieveSignal, expectedValue);

        BOOST_CHECK_EQUAL_COLLECTIONS(bufferData.begin(), bufferData.end(), testDataVector.begin(), testDataVector.end());
    }

    void Test_Modem_Pairing_No_Data(unsigned int signal, unsigned int recieveSignal, unsigned int expectedValue)
    {
        std::cout << "Test_Modem_Pairing test" << std::endl;

        int modemSignals = 0;

        this->serialServer.manageModemStatus(signal);

        sleep(1);
        
        modemSignals = this->serialServer.getModemStatus();

        BOOST_CHECK_EQUAL(modemSignals & recieveSignal, expectedValue);
    }
};

BOOST_FIXTURE_TEST_SUITE(test_data_transfer, TestSerialServerFixture)

BOOST_AUTO_TEST_CASE(test_less_than_buffer_size)
{
    CompareEcho("test_conn!");
}

BOOST_AUTO_TEST_CASE(test_more_than_buffer_size)
{
    CompareEcho("test_connection$");
}

BOOST_AUTO_TEST_CASE(test_null)
{
    CompareEcho("tēst_\0čo#");
}

BOOST_AUTO_TEST_CASE(test_non_ASCII)
{
    CompareEcho("tēst_\x01č@");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(test_modem, TestSerialServerFixture)

BOOST_AUTO_TEST_CASE(test_CTS_with_data)
{
    Test_Modem_Pairing("RTS1", TIOCM_RTS, TIOCM_CTS, TIOCM_CTS);
    Test_Modem_Pairing("RTS0", TIOCM_RTS, TIOCM_CTS, 0);
    Test_Modem_Pairing("RTS1", TIOCM_RTS, TIOCM_CTS, TIOCM_CTS);
    Test_Modem_Pairing("RTS0", TIOCM_RTS, TIOCM_CTS, 0);
}

BOOST_AUTO_TEST_CASE(test_DSR_with_data)
{
    Test_Modem_Pairing("DTR1", TIOCM_DTR, TIOCM_DSR, TIOCM_DSR);
    Test_Modem_Pairing("DTR0", TIOCM_DTR, TIOCM_DSR, 0);
    Test_Modem_Pairing("DTR1", TIOCM_DTR, TIOCM_DSR, TIOCM_DSR);
    Test_Modem_Pairing("DTR0", TIOCM_DTR, TIOCM_DSR, 0);
}

BOOST_AUTO_TEST_CASE(test_CTS_with_no_data)
{
    Test_Modem_Pairing_No_Data(TIOCM_RTS, TIOCM_CTS, TIOCM_CTS);
    Test_Modem_Pairing_No_Data(TIOCM_RTS, TIOCM_CTS, 0);
    Test_Modem_Pairing_No_Data(TIOCM_RTS, TIOCM_CTS, TIOCM_CTS);
    Test_Modem_Pairing_No_Data(TIOCM_RTS, TIOCM_CTS, 0);
}

BOOST_AUTO_TEST_CASE(test_DSR_with_no_data)
{
    Test_Modem_Pairing_No_Data(TIOCM_DTR, TIOCM_DSR, TIOCM_DSR);
    Test_Modem_Pairing_No_Data(TIOCM_DTR, TIOCM_DSR, 0);
    Test_Modem_Pairing_No_Data(TIOCM_DTR, TIOCM_DSR, TIOCM_DSR);
    Test_Modem_Pairing_No_Data(TIOCM_DTR, TIOCM_DSR, 0);
}

BOOST_AUTO_TEST_SUITE_END()

int main(int argc, char* argv[])
{
    Log syslog("serial-echo-test", LOG_LOCAL0);

    // attach std::clog to syslog
    Attach_rdbuf attach( std::clog, &syslog );

    // output to clog will appear in syslog
    // output to cout will appear in stdout and syslog
    Tee tee( std::cout, std::clog );

    return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
}
