################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library/src/commons/bitarray.c \
../so-commons-library/src/commons/config.c \
../so-commons-library/src/commons/error.c \
../so-commons-library/src/commons/log.c \
../so-commons-library/src/commons/process.c \
../so-commons-library/src/commons/string.c \
../so-commons-library/src/commons/temporal.c \
../so-commons-library/src/commons/txt.c 

OBJS += \
./so-commons-library/src/commons/bitarray.o \
./so-commons-library/src/commons/config.o \
./so-commons-library/src/commons/error.o \
./so-commons-library/src/commons/log.o \
./so-commons-library/src/commons/process.o \
./so-commons-library/src/commons/string.o \
./so-commons-library/src/commons/temporal.o \
./so-commons-library/src/commons/txt.o 

C_DEPS += \
./so-commons-library/src/commons/bitarray.d \
./so-commons-library/src/commons/config.d \
./so-commons-library/src/commons/error.d \
./so-commons-library/src/commons/log.d \
./so-commons-library/src/commons/process.d \
./so-commons-library/src/commons/string.d \
./so-commons-library/src/commons/temporal.d \
./so-commons-library/src/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library/src/commons/%.o: ../so-commons-library/src/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


