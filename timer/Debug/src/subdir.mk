################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/cr_startup_lpc80x.c \
../src/crp.c \
../src/timer.c 

OBJS += \
./src/aeabi_romdiv_patch.o \
./src/cr_startup_lpc80x.o \
./src/crp.o \
./src/timer.o 

C_DEPS += \
./src/cr_startup_lpc80x.d \
./src/crp.d \
./src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\utilities_lib" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\peripherals_lib" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\timer\inc" -g3 -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\utilities_lib" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\peripherals_lib" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\timer\inc" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\common\inc" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\peripherals_lib\inc" -I"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\utilities_lib\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


