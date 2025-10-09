#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <ostream>
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <chrono>

class Logger
{
    public:
        Logger();
        virtual ~Logger() = default;

        enum Severity {
            Normal,
            Warning,
            Error,
            Fatal,
            Debug
        };
        static const std::string severityToString(Severity s, bool colorize = false);

        // Main log method. All others forms call this.
        virtual void log(Severity s, const std::string & msg);

        // simple string logging
        void normal(const std::string & msg)  { log(Normal, msg);  }
        void warning(const std::string & msg) { log(Warning, msg); }
        void error(const std::string & msg)   { log(Error, msg);   }
        void fatal(const std::string & msg)   { log(Fatal, msg);   }
        void debug(const std::string & msg)   { log(Debug, msg);   }

        // printf()-style logging
        void vlogf(Severity, const char * fmt, va_list args);
        void logf(Severity s, const char * fmt, ...);
        void normalf(const char * fmt, ...);
        void warningf(const char * fmt, ...);
        void errorf(const char * fmt, ...);
        void fatalf(const char * fmt, ...);
        void debugf(const char * fmt, ...);

        // Helper for ostream flavor of log methods (eg: logger.normal() << "message").
        // Log methods return an instance of this class, which accumulates a string
        // from chained operator<<()s. Finally, when the OStreamBuffer temporary goes out
        // of scope, it flushes the resulting message to the Logger that created it.
        struct OStreamBuffer {
            OStreamBuffer(Logger & l, Severity s) : logger(l), severity(s) {}
            OStreamBuffer(OStreamBuffer &) = delete;
            OStreamBuffer(OStreamBuffer &&) = default;
            ~OStreamBuffer() { logger.log(severity, ss.str()); }

            template<typename T> OStreamBuffer & operator<<(T v) { ss << v; return *this; }

            Logger & logger;
            Severity severity;
            std::stringstream ss;
        };

        // ostream-style logging
        OStreamBuffer normal()  { return OStreamBuffer(*this, Normal);  }
        OStreamBuffer warning() { return OStreamBuffer(*this, Warning); }
        OStreamBuffer error()   { return OStreamBuffer(*this, Error);   }
        OStreamBuffer fatal()   { return OStreamBuffer(*this, Fatal);   }
        OStreamBuffer debug()   { return OStreamBuffer(*this, Debug);   }

	    void logArgv(Severity s, const char * msg,
                     int argc, char ** argv);

        // Omit the newline from the next log message so as to append
        // back-to-back messages
        void join();

        double relativeTimeSinceStart() const;

        static std::string yesno(bool p) { return p ? "yes" : "no"; }

    protected:
        void logToStdout(Severity s, const std::string & msg);

        std::mutex mutex;

        bool joinNext = false;
        bool messageOnlyForJoin = false;

        std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
};

class FileLogger : public Logger
{
    public:
        FileLogger() = delete;
        FileLogger(const std::string & filename);
        FileLogger(const FileLogger &) = delete;
        FileLogger(const FileLogger &&) = delete;
        virtual ~FileLogger() = default;

        virtual void log(Severity s, const std::string & msg);

        bool mirrorToStdout = false;
        bool mirrorWarningsAndErrorsToStdout = true;

    protected:
        std::ofstream outFile;
};

Logger & getLogger();
void setLogger(std::shared_ptr<Logger> logger);

#endif
