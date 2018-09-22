################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../COM/lqvg/com.c \
../COM/lqvg/protocolo.c 

OBJS += \
./COM/lqvg/com.o \
./COM/lqvg/protocolo.o 

C_DEPS += \
./COM/lqvg/com.d \
./COM/lqvg/protocolo.d 


# Each subdirectory must supply rules for building sources it contributes
COM/lqvg/%.o: ../COM/lqvg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


