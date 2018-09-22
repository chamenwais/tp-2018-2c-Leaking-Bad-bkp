################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DAM/src/ConexionesEntrantes.c \
../DAM/src/ConexionesSalientes.c \
../DAM/src/Contexto.c \
../DAM/src/ElDiego.c \
../DAM/src/Utilidades.c 

OBJS += \
./DAM/src/ConexionesEntrantes.o \
./DAM/src/ConexionesSalientes.o \
./DAM/src/Contexto.o \
./DAM/src/ElDiego.o \
./DAM/src/Utilidades.o 

C_DEPS += \
./DAM/src/ConexionesEntrantes.d \
./DAM/src/ConexionesSalientes.d \
./DAM/src/Contexto.d \
./DAM/src/ElDiego.d \
./DAM/src/Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
DAM/src/%.o: ../DAM/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


