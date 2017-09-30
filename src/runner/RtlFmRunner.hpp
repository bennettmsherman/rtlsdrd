/*
 * RtlFmRunner.hpp
 *
 *  Created on: Sep 11, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_RTLFMRUNNER_HPP_
#define WRAPPER_RTLFMRUNNER_HPP_

// System Includes
#include <sys/types.h>

class RtlFmRunner
{
public:
    static RtlFmRunner& getInstance();

    void execRtlFmCommand(const char* const rtlFmParams[], const char* const aplayParams[]);
    static void killAplayAndRtlFm();

    // Intended to be executed by the command parser
    void stopCommandHandler(const std::string& UNUSED);

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

    static void killAplay(bool allowThrow = true);
    static void killRtlFm(bool allowThrow = true);

    static void printCommand(const std::string& prefixString, const char* const params[]);

    static pid_t rtlFmPid;
    static pid_t aplayPid;

    int rtlFmAplayCommsPipe[2];
    const int& rtlFmAplayCommsPipeReadEndFd = rtlFmAplayCommsPipe[0];
    const int& rtlFmAplayCommsPipeWriteEndFd = rtlFmAplayCommsPipe[1];

};

#endif /* WRAPPER_RTLFMRUNNER_HPP_ */
