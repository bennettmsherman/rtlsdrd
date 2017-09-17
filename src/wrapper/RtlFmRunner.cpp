/*
 * RtlFmRunner.cpp
 *
 *  Created on: Sep 11, 2017
 *      Author: bensherman
 */

// System Includes
#include <errno.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <system_error>
#include <unistd.h>

// Project Includes
#include "RtlFmRunner.hpp"

void RtlFmRunner::execRtlFmCommand(const char* const rtlFmParams[], const char* const aplayParams[])
{
    // Kills the currently running rtl_fm and aplay processes, if applicable
    cleanupPreviousExecution();

    // Create the pipe for rtl_fm and aplay to communicate
    createRtlFmAplayCommsPipe();

    // Execute aplay first so that no output from rtl_fm is missed
    forkAndExecAplay(aplayParams);

    forkAndExecRtlFm(rtlFmParams);

    // The proc starting aplay and rtl_fm has no need to access the rtl_fm<->aplay pipe
    close(rtlFmAplayCommsPipeReadEndFd);
    close(rtlFmAplayCommsPipeWriteEndFd);

}

void RtlFmRunner::cleanupPreviousExecution()
{
    if (aplayPid != 0)
    {
        if (kill(aplayPid, SIGKILL) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error killing aplay");
        }
        std::cout << "Killed aplay with PID: " << aplayPid << std::endl;
        aplayPid = 0;
    }
    if (rtlFmPid != 0)
    {
        if (kill(rtlFmPid, SIGKILL) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error killing rtl_fm");
        }
        std::cout << "Killed rtl_fm with PID: " << rtlFmPid << std::endl;
        rtlFmPid = 0;
    }
}

void RtlFmRunner::forkAndExecAplay(const char* const aplayParams[])
{
    pid_t forkRet = fork();
    if (forkRet < 0) // Error
    {
        throw std::system_error(errno, std::system_category(), "Error forking for aplay");
    }
    else if (forkRet == 0) // Child
    {
        // Close the unneeded write end of the pipe
        if (close(rtlFmAplayCommsPipeWriteEndFd) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error closing write end of rtl_fm<->aplay pipe before exec()ing aplay");
        }

        // Connect aplay's stdin to the read end of the pipe
        if (dup2(rtlFmAplayCommsPipeReadEndFd, STDIN_FILENO) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error using dup2() to set aplay's stdin to the rtl_fm<->aplay pipe read end");
        }

        // Let's boogie
        if(execv(aplayParams[0], const_cast<char* const*>(aplayParams)) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error exec()ing aplay");
        }
    }
    else // Parent
    {
        aplayPid = forkRet;
        std::cout << "aplay exec()'d with PID: " << aplayPid << std::endl;
    }
}

void RtlFmRunner::forkAndExecRtlFm(const char* const rtlFmParams[])
{
    pid_t forkRet = fork();
    if (forkRet < 0) // Error
    {
        throw std::system_error(errno, std::system_category(), "Error forking for rtl_fm");
    }
    else if (forkRet == 0) // Child
    {
        // Close the unneeded read end of the pipe
        if (close(rtlFmAplayCommsPipeReadEndFd) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error closing read end of rtl_fm<->aplay pipe before exec()ing rtl_fm process");
        }

        // Connect rtl_fm's stdout to the write end of the pipe
        if (dup2(rtlFmAplayCommsPipeWriteEndFd, STDOUT_FILENO) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error using dup2() to set rtl_fm's stdout to the rtl_fm<->aplay pipe write end");
        }

        // Let's boogie
        if(execv(rtlFmParams[0], const_cast<char* const*>(rtlFmParams)) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Error exec()ing rtl_fm");
        }
    }
    else // Parent
    {
        rtlFmPid = forkRet;
        std::cout << "rtl_fm exec()'d with PID: " << rtlFmPid << std::endl;
    }
}

/**
 * Creates a pipe for rtl_fm and aplay to communicate through.
 * If the pipe is created successfully, rtlFmAplayCommsPipe is
 * updated. If an error occurred, a system_error is thrown.
 */
void RtlFmRunner::createRtlFmAplayCommsPipe()
{
    int pipeRet = pipe(rtlFmAplayCommsPipe);

    if (pipeRet < 0)
    {
        throw std::system_error(errno, std::system_category(), "Error creating rtl_fm -> aplay pipe");
    }

    std::cout << "Pipe created successfully with file descriptors: " << rtlFmAplayCommsPipe[0] << "," << rtlFmAplayCommsPipe[1] << std::endl;
}


