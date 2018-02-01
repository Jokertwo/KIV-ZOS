################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ZOS.c \
../src/bitMap.c \
../src/createBootFile.c \
../src/load.c \
../src/mftHelper.c 

OBJS += \
./src/ZOS.o \
./src/bitMap.o \
./src/createBootFile.o \
./src/load.o \
./src/mftHelper.o 

C_DEPS += \
./src/ZOS.d \
./src/bitMap.d \
./src/createBootFile.d \
./src/load.d \
./src/mftHelper.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


