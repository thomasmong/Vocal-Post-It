################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: test.axf

dependents:
	-cd D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/peripherals_lib/flash && $(MAKE) all
	-cd D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/utilities_lib/flash && $(MAKE) all

# Tool invocations
test.axf: $(OBJS) $(USER_OBJS) D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/peripherals_lib/flash/libperipherals_lib.a D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/utilities_lib/flash/libutilities_lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\utilities_lib\flash" -L"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\peripherals_lib\flash" -Xlinker -Map="test.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "post_it_Debug.ld" -o "test.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) test.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "test.axf"; # arm-none-eabi-objcopy -v -O binary "test.axf" "test.bin" ; # checksum -p LPC804 -d "test.bin";
	-@echo ' '

.PHONY: all clean dependents post-build
D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/peripherals_lib/flash/libperipherals_lib.a:
D:/ENS/Formation/2_An/M1_E3A/441_Info_indus/Projet/mcux/utilities_lib/flash/libutilities_lib.a:

-include ../makefile.targets
