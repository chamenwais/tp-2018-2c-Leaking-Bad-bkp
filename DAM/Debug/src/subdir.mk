################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ConexionesEntrantes.c \
../src/Contexto.c \
../src/ElDiego.c \
../src/Utilidades.c 

OBJS += \
./src/ConexionesEntrantes.o \
./src/Contexto.o \
./src/ElDiego.o \
./src/Utilidades.o 

C_DEPS += \
./src/ConexionesEntrantes.d \
./src/Contexto.d \
./src/ElDiego.d \
./src/Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


