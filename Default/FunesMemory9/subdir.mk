################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FunesMemory9/FunesMemory9.c \
../FunesMemory9/cliente_dummy.c 

OBJS += \
./FunesMemory9/FunesMemory9.o \
./FunesMemory9/cliente_dummy.o 

C_DEPS += \
./FunesMemory9/FunesMemory9.d \
./FunesMemory9/cliente_dummy.d 


# Each subdirectory must supply rules for building sources it contributes
FunesMemory9/%.o: ../FunesMemory9/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


