################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FunesMemory9/src/FunesMemory9.c \
../FunesMemory9/src/cliente_dummy.c 

OBJS += \
./FunesMemory9/src/FunesMemory9.o \
./FunesMemory9/src/cliente_dummy.o 

C_DEPS += \
./FunesMemory9/src/FunesMemory9.d \
./FunesMemory9/src/cliente_dummy.d 


# Each subdirectory must supply rules for building sources it contributes
FunesMemory9/src/%.o: ../FunesMemory9/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


