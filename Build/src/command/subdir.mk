################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/command/CommandParser.cpp 

OBJS += \
./src/command/CommandParser.o 

CPP_DEPS += \
./src/command/CommandParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/command/%.o: ../src/command/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src/param_builder/specific_params -I../src/runner -I../src/network -I../src/param_builder/base_params -I../src/param_builder -I../src/command -I../src/daemon -O2 -g -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


