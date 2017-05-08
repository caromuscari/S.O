################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Consola.c \
../src/escuchar_mensaje.c \
../src/hilo_programa.c \
../src/hilo_usuario.c \
../src/log.c \
../src/mensaje.c \
../src/parametros.c \
../src/socket_cliente.c 

OBJS += \
./src/Consola.o \
./src/escuchar_mensaje.o \
./src/hilo_programa.o \
./src/hilo_usuario.o \
./src/log.o \
./src/mensaje.o \
./src/parametros.o \
./src/socket_cliente.o 

C_DEPS += \
./src/Consola.d \
./src/escuchar_mensaje.d \
./src/hilo_programa.d \
./src/hilo_usuario.d \
./src/log.d \
./src/mensaje.d \
./src/parametros.d \
./src/socket_cliente.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


