################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FS/FileSystem.c \
../FS/FuncionesDelFileSystem.c \
../FS/VariablesGlobales.c 

OBJS += \
./FS/FileSystem.o \
./FS/FuncionesDelFileSystem.o \
./FS/VariablesGlobales.o 

C_DEPS += \
./FS/FileSystem.d \
./FS/FuncionesDelFileSystem.d \
./FS/VariablesGlobales.d 


# Each subdirectory must supply rules for building sources it contributes
FS/%.o: ../FS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


