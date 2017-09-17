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

// Project Includes

class RtlFmRunner
{
public:
    void execRtlFmCommand(const char* const rtlFmParams[], const char* const aplayParams[]);

private:
    void forkAndExecAplay(const char* const aplayParams[]);
    void forkAndExecRtlFm(const char* const rtlFmParams[]);
    void createRtlFmAplayCommsPipe();
    void cleanupPreviousExecution();

    pid_t rtlFmPid = 0;
    pid_t aplayPid = 0;

    int rtlFmAplayCommsPipe[2];
    const int & rtlFmAplayCommsPipeReadEndFd = rtlFmAplayCommsPipe[0];
    const int & rtlFmAplayCommsPipeWriteEndFd = rtlFmAplayCommsPipe[1];

};

#endif /* WRAPPER_RTLFMRUNNER_HPP_ */
