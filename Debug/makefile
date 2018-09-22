################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SAFA/subdir.mk
-include FunesMemory9/src/subdir.mk
-include FunesMemory9/subdir.mk
-include FS/subdir.mk
-include DAM/src/subdir.mk
-include CPU/subdir.mk
-include COM/lqvg/subdir.mk
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
all: Leaking\ Bad

# Tool invocations
Leaking\ Bad: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "Leaking Bad" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) "Leaking Bad"
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
