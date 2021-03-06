#pragma once

#include <streambuf>
#include <syslog.h>
#include <cstring>
#include <ostream>
#include "TeeBuffer.h"

enum LogPriority {
    kLogEmerg   = LOG_EMERG,   // system is unusable
    kLogAlert   = LOG_ALERT,   // action must be taken immediately
    kLogCrit    = LOG_CRIT,    // critical conditions
    kLogErr     = LOG_ERR,     // error conditions
    kLogWarning = LOG_WARNING, // warning conditions
    kLogNotice  = LOG_NOTICE,  // normal, but significant, condition
    kLogInfo    = LOG_INFO,    // informational message
    kLogDebug   = LOG_DEBUG    // debug-level message
};

std::ostream& operator<< (std::ostream& os, const LogPriority& log_priority);

class Log : public std::basic_streambuf<char, std::char_traits<char> > {
public:
    explicit Log(std::string ident, int facility);

protected:
    int sync();
    int overflow(int c);

private:
    friend std::ostream& operator<< (std::ostream& os, const LogPriority& log_priority);
    std::string buffer_;
    int facility_;
    int priority_;
    char ident_[50];
};

class Attach_rdbuf
{
    std::ostream & stream_;
    std::streambuf * const old_sb_;
public:
    Attach_rdbuf( std::ostream & stream, std::streambuf * new_sb )
        : stream_( stream )
        , old_sb_( stream.rdbuf() )
    {
       stream.rdbuf( new_sb );
    }

   ~Attach_rdbuf()
    {
       stream_.rdbuf( old_sb_ );
    }
};

class Tee
{
    teebuf tee_impl;
    Attach_rdbuf attach;
public:
    Tee( std::ostream & stream1, std::ostream & stream2 )
        : tee_impl( stream1.rdbuf(), stream2.rdbuf() )
        , attach( stream1, &tee_impl )
    {
    }
};

