################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ConexionesEntrantes.c \
../ConexionesSalientes.c \
../Contexto.c \
../ElDiego.c \
../Utilidades.c 

OBJS += \
./ConexionesEntrantes.o \
./ConexionesSalientes.o \
./Contexto.o \
./ElDiego.o \
./Utilidades.o 

C_DEPS += \
./ConexionesEntrantes.d \
./ConexionesSalientes.d \
./Contexto.d \
./ElDiego.d \
./Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


