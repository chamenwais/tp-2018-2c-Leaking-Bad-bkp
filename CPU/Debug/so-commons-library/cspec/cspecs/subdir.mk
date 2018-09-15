################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library/cspec/cspecs/cspec.c 

OBJS += \
./so-commons-library/cspec/cspecs/cspec.o 

C_DEPS += \
./so-commons-library/cspec/cspecs/cspec.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library/cspec/cspecs/%.o: ../so-commons-library/cspec/cspecs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


