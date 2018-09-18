################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lqvg/com.c \
../lqvg/protocolo.c 

OBJS += \
./lqvg/com.o \
./lqvg/protocolo.o 

C_DEPS += \
./lqvg/com.d \
./lqvg/protocolo.d 


# Each subdirectory must supply rules for building sources it contributes
lqvg/%.o: ../lqvg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


