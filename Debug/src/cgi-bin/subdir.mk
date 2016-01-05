################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cgi-bin/add.c 

OBJS += \
./src/cgi-bin/add.o 

C_DEPS += \
./src/cgi-bin/add.d 


# Each subdirectory must supply rules for building sources it contributes
src/cgi-bin/%.o: ../src/cgi-bin/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


