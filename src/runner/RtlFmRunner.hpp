/*
 * RtlFmRunner.hpp
 *
 *  Created on: Sep 11, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_RTLFMRUNNER_HPP_
#define WRAPPER_RTLFMRUNNER_HPP_

// System Includes
#include <atomic>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <vector>

class RtlFmRunner
{
public:
    static RtlFmRunner& getInstance();

    void execRtlFmCommand(const char* const rtlFmParams[], const char* const aplayParams[],
            const std::vector<std::string>& userProvidedParamStrings);

    void killAplayAndRtlFm();

    // Intended to be executed by the command parser
    void stopCommandHandler(const std::string& UNUSED, std::string* updatableMessage);
    void getUserProvidedCommandsInUse(const std::string& UNUSED, std::string* updatableMessage);

private:

    /**
     * A private constructor is required for the singleton pattern
     */
    RtlFmRunner() = default;

    /**
     * Delete the default copy constructor
     */
    RtlFmRunner(const RtlFmRunner&) = delete;

    /**
     * Delete the default assignment operator
     */
    RtlFmRunner& operator=(const RtlFmRunner&) = delete;

    void forkAndExecAplay(const char* const aplayParams[]);
    void aplayProcessPreExecOperations(const char* const aplayParams[]);

    void forkAndExecRtlFm(const char* const rtlFmParams[]);
    void rtlFmProcessPreExecOperations(const char* const rtlFmParams[]);

    void createRtlFmAplayCommsPipe();
    void closeRunnerPipeEnds();

    void killAplay(bool allowThrow = true);
    void killRtlFm(bool allowThrow = true);

    void waitPid(pid_t pidToWaitFor, bool allowThrow=true);

    void printCommand(const std::string& prefixString, const char* const params[]);

    // This vector will contain all of the user-provided parameters which
    // are currently being used to run rtl_fm and aplay. These commands
    // are in the form "FUNCTION=PARAMETER", and are assumed valid.
    std::vector<std::string> userProvidedParamStringsInUse;

    // Pids for the aplay and rtl_fm processes. 0 if the program isn't running
    std::atomic<pid_t> rtlFmPid{0};
    std::atomic<pid_t> aplayPid{0};

    // Used for preventing multiple clients from executing or killing (or both)
    // aplay and rtl_fm simultaneously
    std::mutex runnerMutex;

    int rtlFmAplayCommsPipe[2];
    const int& rtlFmAplayCommsPipeReadEndFd = rtlFmAplayCommsPipe[0];
    const int& rtlFmAplayCommsPipeWriteEndFd = rtlFmAplayCommsPipe[1];

};

#endif /* WRAPPER_RTLFMRUNNER_HPP_ */
