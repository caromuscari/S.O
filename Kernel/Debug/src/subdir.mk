################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/configuracion.c \
../src/consolaKernel.c \
../src/consolaManager.c \
../src/cpuManager.c \
../src/log.c \
../src/manejo_errores.c \
../src/memoria.c \
../src/mensaje.c \
../src/metadata.c \
../src/planificador.c \
../src/semaforos_vglobales.c \
../src/socket.c 

OBJS += \
./src/Kernel.o \
./src/configuracion.o \
./src/consolaKernel.o \
./src/consolaManager.o \
./src/cpuManager.o \
./src/log.o \
./src/manejo_errores.o \
./src/memoria.o \
./src/mensaje.o \
./src/metadata.o \
./src/planificador.o \
./src/semaforos_vglobales.o \
./src/socket.o 

C_DEPS += \
./src/Kernel.d \
./src/configuracion.d \
./src/consolaKernel.d \
./src/consolaManager.d \
./src/cpuManager.d \
./src/log.d \
./src/manejo_errores.d \
./src/memoria.d \
./src/mensaje.d \
./src/metadata.d \
./src/planificador.d \
./src/semaforos_vglobales.d \
./src/socket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


