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
all: UART0_Terminal.axf

# Tool invocations
UART0_Terminal.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\peripherals_lib\flash" -L"D:\ENS\Formation\2_An\M1_E3A\441_Info_indus\Projet\mcux\utilities_lib\flash" -Xlinker -Map="UART0_Terminal.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "UART0_Terminal_flash.ld" -o "UART0_Terminal.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) UART0_Terminal.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "UART0_Terminal.axf"; # arm-none-eabi-objcopy -v -O binary "UART0_Terminal.axf" "UART0_Terminal.bin" ; # checksum -p LPC804 -d "UART0_Terminal.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
