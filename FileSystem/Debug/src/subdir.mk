################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FileSystem.c \
../src/archivos.c \
../src/fsecundarias.c \
../src/funciones.c \
../src/log.c \
../src/manejo_errores.c \
../src/mensaje.c \
../src/socket.c 

OBJS += \
./src/FileSystem.o \
./src/archivos.o \
./src/fsecundarias.o \
./src/funciones.o \
./src/log.o \
./src/manejo_errores.o \
./src/mensaje.o \
./src/socket.o 

C_DEPS += \
./src/FileSystem.d \
./src/archivos.d \
./src/fsecundarias.d \
./src/funciones.d \
./src/log.d \
./src/manejo_errores.d \
./src/mensaje.d \
./src/socket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


