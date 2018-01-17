################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/daemon/arg_parser/ArgParser.cpp \
../src/daemon/arg_parser/ArgParserFunction.cpp 

OBJS += \
./src/daemon/arg_parser/ArgParser.o \
./src/daemon/arg_parser/ArgParserFunction.o 

CPP_DEPS += \
./src/daemon/arg_parser/ArgParser.d \
./src/daemon/arg_parser/ArgParserFunction.d 


# Each subdirectory must supply rules for building sources it contributes
src/daemon/arg_parser/%.o: ../src/daemon/arg_parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -I../src/param_builder/specific_params -I../src/system -I/usr/include/boost -I../src/runner -I../src/network -I../src/param_builder/base_params -I../src/param_builder -I../src/command -I../src/daemon -I../src/daemon/arg_parser -O2 -g -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


