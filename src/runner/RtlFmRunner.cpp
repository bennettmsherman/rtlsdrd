/*
 * RtlFmRunner.cpp
 *
 *  Created on: Sep 11, 2017
 *      Author: bensherman
 */

// System Includes
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

// Project Includes
#include "RtlFmRunner.hpp"
#include "SystemUtils.hpp"

/**
 * Executes rtl_fm and aplay with the command strings specified by rtlFmParams
 * and aplayParas, respectively. If aplay or rtl_fm are being executed when
 * this function is called, the previous executions are terminated.
 * The userProvidedParamStrings vector will replace the current version of
 * userProvidedParamsInUse.
 * An system_error exception is thrown if any internal error occurs.
 * This execution is MT protected using the runnerMutex.
 */
void RtlFmRunner::execRtlFmCommand(const char* const rtlFmParams[], const char* const aplayParams[],
        const std::vector<std::string>& userProvidedParamStrings)
{
    std::lock_guard<std::mutex> executeCommandGuard{runnerMutex};

    // Kill previous execution of aplay and rtl_fm
    killAplay();
    killRtlFm();

    // Create the pipe for rtl_fm and aplay to communicate
    createRtlFmAplayCommsPipe();

    // Execute aplay first so that no output from rtl_fm is missed
    printCommand("Attempting to execute aplay with cmd: ", aplayParams);
    forkAndExecAplay(aplayParams);

    printCommand("Attempting to execute rtl_fm with cmd: ", rtlFmParams);
    forkAndExecRtlFm(rtlFmParams);

    // The proc starting aplay and rtl_fm has no need to access the rtl_fm<->aplay pipe
    closeRunnerPipeEnds();

    // Update the list of user provided parameter strings used to execute
    // rtl_fm and aplay
    userProvidedParamStringsInUse = userProvidedParamStrings;
}

/**
 * Attempts to close the ends of the rtl_fm <-> aplay comms pipe which are open
 * in the runner. Throws a system_error exception when either end fails to
 * close. Note that if the first call to close() fails, an exception is thrown
 * and the other end will not be closed.
 */
void RtlFmRunner::closeRunnerPipeEnds()
{
    if (close(rtlFmAplayCommsPipeReadEndFd) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "Error closing rtl_fm <-> aplay comms pipe READ end inside the runner process");
    }

    if (close(rtlFmAplayCommsPipeWriteEndFd) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "Error closing rtl_fm <-> aplay comms pipe WRITE end inside the runner process");
    }
}

/**
 * Attempts to kill aplay if aplayPid is nonzero. Then, waits for the process
 * to terminate.
 * std::system_error is thrown if there was an error killing aplay or waiting
 * for it to terminate AND allowThrow is true.
 * If allowThrow is false and an error occurs, the exception message is printed
 * but no exception is thrown.
 */
void RtlFmRunner::killAplay(bool allowThrow)
{
    if (aplayPid != 0)
    {
        if (kill(aplayPid, SIGKILL) < 0)
        {
            std::system_error exception(errno, std::system_category(), "Error killing aplay");
            if (allowThrow)
            {
                throw exception;
            }
            else
            {
                std::cerr << exception.what() << "; Code: " << exception.code()  << std::endl;
            }
        }
        std::cout << "Killed aplay with PID: " << aplayPid << std::endl;
        waitPid(aplayPid, allowThrow);
        aplayPid = 0;
    }
}

/**
 * Waits for the PID specified by pidToWaitFor to change state (terminate).
 * std::system_error is thrown if there was an error calling waitpid() AND
 * allowThrow is true.
 * If allowThrow is false and an error occurs, the exception message is printed
 * but no exception is thrown.
 */
void RtlFmRunner::waitPid(pid_t pidToWaitFor, bool allowThrow)
{
    std::cout << "Waiting for PID: " << std::to_string(pidToWaitFor) << std::endl;
    if (waitpid(pidToWaitFor, NULL, 0) < 0)
    {
        std::string errMsg = "Error waiting on PID";
        errMsg += std::to_string(pidToWaitFor);

        std::system_error exception(errno, std::system_category(), errMsg.c_str());
        if (allowThrow)
        {
            throw exception;
        }
        else
        {
            std::cerr << exception.what() << "; Code: " << exception.code()  << std::endl;
        }
    }
}

/**
 * Attempts to kill rtl_fm if rtlFmPid is nonzero. Then, waits for the process
 * to terminate.
 * std::system_error is thrown if there was an error killing rtl_fm or waiting
 * for it to terminate AND allowThrow is true.
 * If allowThrow is false and an error occurs, the exception message is printed
 * but no exception is thrown.
 */
void RtlFmRunner::killRtlFm(bool allowThrow)
{
    if (rtlFmPid != 0)
    {
        if (kill(rtlFmPid, SIGKILL) < 0)
        {
            std::system_error exception(errno, std::system_category(), "Error killing rtl_fm");
            if (allowThrow)
            {
                throw exception;
            }
            else
            {
                std::cerr << exception.what() << "; Code: " << exception.code()  << std::endl;
            }
        }
        std::cout << "Killed rtl_fm with PID: " << rtlFmPid << std::endl;
        waitPid(rtlFmPid, allowThrow);
        rtlFmPid = 0;
    }
}

/**
 * Forks a new process which will execute aplay.
 * In the parent, if an error occurs during forking, a std::system_error is
 * thrown.
 * In the child process, if an error occurs, the process will exit.
 */
void RtlFmRunner::forkAndExecAplay(const char* const aplayParams[])
{
    pid_t forkRet = fork();
    if (forkRet < 0) // Error
    {
        throw std::system_error(errno, std::system_category(), "Error forking for aplay");
    }
    else if (forkRet == 0) // Child
    {
        try
        {
            aplayProcessPreExecOperations(aplayParams);
        }
        catch (const std::system_error& except)
        {
            std::cerr << "Aplay failed to execute; exception: " << except.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else // Parent
    {
        aplayPid = forkRet;
        std::cout << "aplay exec()'d with PID: " << aplayPid << std::endl;
    }
}

/**
 * Executed by the process which is to execute aplay!
 * Closes unnecessary pipe ends, changes stdin to rtl_fm's stdout,
 * and attempts to execute aplay. If any of these operations fail,
 * a std::system_error is thrown.
 */
void RtlFmRunner::aplayProcessPreExecOperations(const char* const aplayParams[])
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

/**
 * Forks a new process which will execute rtl_fm.
 * In the parent, if an error occurs during forking, a std::system_error is
 * thrown.
 * In the child process, if an error occurs, the process will exit.
 */
void RtlFmRunner::forkAndExecRtlFm(const char* const rtlFmParams[])
{
    pid_t forkRet = fork();
    if (forkRet < 0) // Error
    {
        throw std::system_error(errno, std::system_category(), "Error forking for rtl_fm");
    }
    else if (forkRet == 0) // Child
    {
        try
        {
            rtlFmProcessPreExecOperations(rtlFmParams);
        }
        catch (const std::system_error& except)
        {
            std::cerr << "rtl_fm failed to execute; exception: " << except.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else // Parent
    {
        rtlFmPid = forkRet;
        std::cout << "rtl_fm exec()'d with PID: " << rtlFmPid << std::endl;
    }
}

/**
 * Executed by the process which is to execute rtl_fm.
 * Closes unnecessary pipe ends, changes stdout fd,
 * and attempts to execute rtl_fm. If any of these operations fail,
 * a std::system_error is thrown.
 */
void RtlFmRunner::rtlFmProcessPreExecOperations(const char* const rtlFmParams[])
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

/**
 * The function called by the "STOP" command. Terminates aplay
 * and rtl_fm if they're currently running.
 */
void RtlFmRunner::stopCommandHandler(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;
    (void) updatableMessage;
    killAplayAndRtlFm();
}

/**
 * Kills aplay and rtl_fm. This is intended to be used as a public function
 * to be called when the main daemon is terminated. If an error occurs,
 * no exceptions are thrown, but the exception messages are displayed to stderr.
 * This execution is MT protected using runnerMutex.
 */
void RtlFmRunner::killAplayAndRtlFm()
{
    std::lock_guard<std::mutex> killProcessGuard{runnerMutex};
    killAplay(false);
    killRtlFm(false);
}

/**
 * Prints the params array, which is intended to be of the form passed to
 * execv(). params MUST have a last element of NULL.
 * prefixString is printed before the command list with no trailing newline
 */
void RtlFmRunner::printCommand(const std::string& prefixString, const char* const params[])
{
    std::cout << prefixString;

    uint32_t index = 0;
    while (params[index] != (char *) NULL)
    {
        std::cout << params[index] << " ";
        ++index;
    }
    std::cout << std::endl;
}

/**
 * Returns a reference to the singleton RtlFmRunner instance
 */
RtlFmRunner& RtlFmRunner::getInstance()
{
    static RtlFmRunner instance;
    return instance;
}

/**
 * Appends to updatableMessage the list of valid user supplied commands which
 * are in use for the current rtl_fm/aplay execution.
 * This also includes the current system volume, although it's not normally
 * saved in the user entered params list (this is somewhat hacky).
 */
void RtlFmRunner::getUserProvidedCommandsInUse(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;
    for (std::string userCmd : userProvidedParamStringsInUse)
    {
        updatableMessage->append("\n");
        updatableMessage->append(userCmd);
    }

    // Get the current system volume and append it to the list
    updatableMessage->append("\n");
    updatableMessage->append(SystemUtils::VOLUME_SETTER_COMMAND + "=");
    updatableMessage->append(std::to_string(SystemUtils::getInstance().getVolume()));

}

