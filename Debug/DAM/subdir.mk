################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DAM/ConexionesEntrantes.c \
../DAM/ConexionesSalientes.c \
../DAM/Contexto.c \
../DAM/ElDiego.c \
../DAM/Utilidades.c 

OBJS += \
./DAM/ConexionesEntrantes.o \
./DAM/ConexionesSalientes.o \
./DAM/Contexto.o \
./DAM/ElDiego.o \
./DAM/Utilidades.o 

C_DEPS += \
./DAM/ConexionesEntrantes.d \
./DAM/ConexionesSalientes.d \
./DAM/Contexto.d \
./DAM/ElDiego.d \
./DAM/Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
DAM/%.o: ../DAM/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


