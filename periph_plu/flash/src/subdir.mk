################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCUXpresso_cr_startup.c \
../src/MCUXpresso_crp.c \
../src/MCUXpresso_mtb.c \
../src/plutest.c \
../src/system.c 

OBJS += \
./src/MCUXpresso_cr_startup.o \
./src/MCUXpresso_crp.o \
./src/MCUXpresso_mtb.o \
./src/plutest.o \
./src/system.o 

C_DEPS += \
./src/MCUXpresso_cr_startup.d \
./src/MCUXpresso_crp.d \
./src/MCUXpresso_mtb.d \
./src/plutest.d \
./src/system.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DCR_INTEGER_PRINTF -D__USE_CMSIS -D__CODE_RED -DCORE_M0PLUS -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=256 -D__REDLIB__ -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\periph_plu\inc" -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\peripherals_lib\inc" -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\utilities_lib\inc" -I"D:\ENS\Formation\2_An\M1_E3A\S1\441_Info_indus\Projet\mcux\common\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


