################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../dispatcher/dispatcher.cpp 

OBJS += \
./dispatcher/dispatcher.o 

CPP_DEPS += \
./dispatcher/dispatcher.d 


# Each subdirectory must supply rules for building sources it contributes
dispatcher/%.o: ../dispatcher/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


