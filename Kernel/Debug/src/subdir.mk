################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/configuracion.c \
../src/cpuManager.c \
../src/log.c \
../src/manejo_errores.c \
../src/socket.c 

OBJS += \
./src/Kernel.o \
./src/configuracion.o \
./src/cpuManager.o \
./src/log.o \
./src/manejo_errores.o \
./src/socket.o 

C_DEPS += \
./src/Kernel.d \
./src/configuracion.d \
./src/cpuManager.d \
./src/log.d \
./src/manejo_errores.d \
./src/socket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


