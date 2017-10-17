/*
 * SystemUtils.hpp
 *
 *  Created on: Sep 30, 2017
 *      Author: bensherman
 */

#ifndef SYSTEM_SYSTEMUTILS_HPP_
#define SYSTEM_SYSTEMUTILS_HPP_

// System Includes
#include <string>

// Project Includes
// <none>

class SystemUtils
{
public:
    static SystemUtils& getInstance();

    static void setVolume(const uint8_t vol);

    static void setAudioControlName(const char * audioControlName);

    // Meant only to be called by the command parser
    void setVolumeCommandHandler(const std::string& vol, std::string* updatableMessage);

private:

    /**
     * A private constructor is required for the singleton pattern
     */
    SystemUtils() = default;

    /**
     * Delete the default copy constructor
     */
    SystemUtils(const SystemUtils&) = delete;

    /**
     * Delete the default assignment operator
     */
    SystemUtils& operator=(const SystemUtils&) = delete;

    static const char VOLUME_SETTER_FORMAT[];

    // The name of the audio control to be used when changing the volume
    static const char * audioControlName;
};

#endif /* SYSTEM_SYSTEMUTILS_HPP_ */
