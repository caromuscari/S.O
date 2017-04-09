################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/configuracion.c \
../src/log.c \
../src/socket_cliente.c \
../src/socket_servidor.c 

OBJS += \
./src/Kernel.o \
./src/configuracion.o \
./src/log.o \
./src/socket_cliente.o \
./src/socket_servidor.o 

C_DEPS += \
./src/Kernel.d \
./src/configuracion.d \
./src/log.d \
./src/socket_cliente.d \
./src/socket_servidor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


