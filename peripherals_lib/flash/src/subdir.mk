################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gpio.c \
../src/i2c.c \
../src/plu.c \
../src/spi.c \
../src/swm.c \
../src/syscon.c 

OBJS += \
./src/gpio.o \
./src/i2c.o \
./src/plu.o \
./src/spi.o \
./src/swm.o \
./src/syscon.o 

C_DEPS += \
./src/gpio.d \
./src/i2c.d \
./src/plu.d \
./src/spi.d \
./src/swm.d \
./src/syscon.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__USE_CMSIS -DDEBUG -D__CODE_RED -DCORE_M0PLUS -DCR_INTEGER_PRINTF -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\peripherals_lib\inc" -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\utilities_lib\inc" -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\common\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


