COMPONENT_PATH := $(abspath $(lastword $(dir $(MAKEFILE_LIST))))


DRIVERS_SRCDIRS := $(COMPONENT_PATH)/GPIO
DRIVERS_INCDIRS := $(COMPONENT_PATH)/GPIO

ifdef CONFIG_SERIAL_PORTS_USE
DRIVERS_SRCDIRS += $(COMPONENT_PATH)/Serial
DRIVERS_INCDIRS += $(COMPONENT_PATH)/Serial
endif