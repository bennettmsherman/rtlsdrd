/*
 * SystemUtils.hpp
 *
 *  Created on: Sep 30, 2017
 *      Author: bensherman
 */

#ifndef SYSTEM_SYSTEMUTILS_HPP_
#define SYSTEM_SYSTEMUTILS_HPP_

// System Includes
#include <atomic>
#include <mutex>
#include <string>
#include <stdint.h>

// Project Includes
// <none>

class SystemUtils
{
public:
    static SystemUtils& getInstance(const char * audioControlName = DEFAULT_AUDIO_CONTROL_NAME);

    void setVolume(const uint8_t vol);

    uint8_t getVolume();

    // Meant only to be called by the command parser
    void setVolumeCommandHandler(const std::string& vol, std::string* updatableMessage);

    // The user specifies "VOLUME=newvol" to set the volume
    static const std::string VOLUME_SETTER_COMMAND;

private:

    /**
     * A private constructor is required for the singleton pattern
     */
    SystemUtils(const char * audioControlName = DEFAULT_AUDIO_CONTROL_NAME);

    /**
     * Delete the default copy constructor
     */
    SystemUtils(const SystemUtils&) = delete;

    /**
     * Delete the default assignment operator
     */
    SystemUtils& operator=(const SystemUtils&) = delete;

    static const char VOLUME_SETTER_FORMAT[];

    static const char* DEFAULT_AUDIO_CONTROL_NAME;

    // The name of the audio control to be used when changing the volume
    const std::string audioControlName;

    // The system volume. Uninitialized to a valid value until the first call
    // to setVolume()
    std::atomic<uint8_t> systemVolume;

    // Used to prevent clients from trying to change the volume simultaneously
    std::mutex volumeSetterMutex;

};

#endif /* SYSTEM_SYSTEMUTILS_HPP_ */
