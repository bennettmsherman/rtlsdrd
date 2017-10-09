/*
 * SystemUtils.cpp
 *
 *  Created on: Sep 30, 2017
 *      Author: bensherman
 */

// System Includes
#include <cstring>
#include <exception>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <system_error>

// Project Includes
#include "SystemUtils.hpp"

// Static Initialization

// %s refers to the audio control (audioControlName), and %u is the percentage
// to set
const char SystemUtils::VOLUME_SETTER_FORMAT[] = "amixer set %s %u%%";

// Use "Master" as the default control
const char * SystemUtils::audioControlName = "Master";

/**
 * Sets the system volume to the percentage specified by the volume parameter.
 * std::system_error is thrown if an error occurred.
 */
void SystemUtils::setVolume(const uint8_t vol)
{
    if (vol > 100)
    {
        throw std::out_of_range("System volume cannot be greater than 100%");
    }

    // The 3 accounts for the int value to be substituted
    static char volumeSetCommandBuff[sizeof(VOLUME_SETTER_FORMAT) + sizeof(audioControlName) + 3] = {};

    // Substitute the new volume into the volume set command
    (void) std::sprintf(volumeSetCommandBuff, VOLUME_SETTER_FORMAT, audioControlName, vol);

    std::cout << "Executing volume set command: " << volumeSetCommandBuff << std::endl;

    int systemRetVal = system(volumeSetCommandBuff);

    if (systemRetVal != 0)
    {
        throw std::system_error(systemRetVal, std::generic_category(),
                "Error when setting system volume using system()");
    }
}

/**
 * Wrapper for setVolume(). Intended to be called by the command interpreter.
 * The vol param should be a percentage representing the new system volume.
 */
void SystemUtils::setVolumeCommandHandler(const std::string& vol)
{
    setVolume(static_cast<uint8_t>(std::stoul(vol)));
}

/**
 * Sets the audio control name to be used when changing the output volume
 * through the daemon. Throws std::invalid_argument and doesn't change the
 * class's pointer value if the parameter is null.
 */
void SystemUtils::setAudioControlName(const char * audioControlName)
{
    if (audioControlName == nullptr)
    {
        throw std::invalid_argument("ERROR! A null pointer has been provided"
                "as the audio control name!");
    }

    SystemUtils::audioControlName = audioControlName;
}

/**
 * Returns a reference to the singleton SystemUtils instance
 */
SystemUtils& SystemUtils::getInstance()
{
    static SystemUtils instance;
    return instance;
}

