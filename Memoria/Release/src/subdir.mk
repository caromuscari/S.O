################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Memoria.c \
../src/log.c \
../src/manejo_errores.c \
../src/socket.c 

OBJS += \
./src/Memoria.o \
./src/log.o \
./src/manejo_errores.o \
./src/socket.o 

C_DEPS += \
./src/Memoria.d \
./src/log.d \
./src/manejo_errores.d \
./src/socket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


