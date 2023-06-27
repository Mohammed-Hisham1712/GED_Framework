#Enable verbosity
ifneq ("$(VERBOSE)", "1")
CMDECHO = @
endif

#Auxilary commands
DISPLAY = $(CMDECHO)echo
MKDIR = $(CMDECHO)mkdir -p
REMOVE = $(CMDECHO)rm -rf
CP = cp
MENUCONFIG = kconfig-mconf
EXPORT_ENV = export
GENCONFIG_OPT = --config-file $(CONFIG_FILE) --header-file\
									 $(PROJECT_PATH)/$(PROJECT_NAME)_config.h
GENCONFIG = genconfig.py $(GENCONFIG_OPT)

#Check Makefile variables
ifndef COMPONENT_SRCDIRS
$(error please define a variable called COMPONENT_SRC_DIRS. It should be defined in Makefile)
endif

ifndef COMPONENT_ADD_INCLUDEDIRS
$(error please define a variable called COMPONENT_INC_DIRS. It should be defined in Makefile)
endif

ifndef PLATFORM_PATH
$(error please define a variable PLATFORM_PATH holding the path for driver and target specific files)
endif

ifndef TARGET_MCU
$(error please define a variable TARGET_MCU. Example TARGET_MCU = STM32F446)
endif

ifndef PROJECT_NAME
$(error no PROJECT_NAME found)
endif

PROJECT_PATH := $(abspath $(firstword $(dir $(MAKEFILE_LIST))))
PATH_ROOT := $(abspath $(lastword $(dir $(MAKEFILE_LIST))))

PATH_MCU = $(PATH_ROOT)/MCU/$(TARGET_MCU)

PATH_OBJECTS := $(PROJECT_PATH)/obj
PATH_BUILD := $(PROJECT_PATH)/build
PATH_INCLUDES := $(PROJECT_PATH)/includes

KCONFIG_INFILE := $(PATH_ROOT)/project.in

ifdef CONFIG
CONFIG_FILE := $(CONFIG)
else
CONFIG_FILE := SDKCONFIG
endif

TARGET_ELF := $(PATH_BUILD)/$(PROJECT_NAME).elf
TARGET_BIN := $(PATH_BUILD)/$(PROJECT_NAME).bin
TARGET_HEX := $(PATH_BUILD)/$(PROJECT_NAME).hex

#Include *.config or SDKCONFIG file
ifneq (, $(wildcard $(PROJECT_PATH)/$(CONFIG_FILE)))
include $(PROJECT_PATH)/$(CONFIG_FILE)
endif

#Include MCU specific makefile
include $(PATH_MCU)/$(TARGET_MCU).mak

#Include HAL makefile
include $(PATH_ROOT)/Components/HAL/hal.mak

#Include Drivers makefile
include $(PATH_ROOT)/Components/Drivers/drivers.mak

#Include RTOS makefile
include $(PATH_ROOT)/Components/RTOS/rtos.mak

#Inckude Utils makefile
include $(PATH_ROOT)/Components/Utils/utils.mak


PLATFORM_EXTRA_SRCDIRS := $(PATH_MCU)
PLATFORM_EXTRA_INCDIRS :=

PLATFORM_SRCDIRS := $(HAL_SRCDIRS) $(DRIVERS_SRCDIRS) $(RTOS_SRCDIRS)\
 									$(UTILS_SRCDIRS) $(PLATFORM_EXTRA_SRCDIRS)
PLATFORM_INCDIRS := $(HAL_INCDIRS) $(DRIVERS_INCDIRS) $(RTOS_INCDIRS)\
									$(UTILS_INCDIRS) $(PLATFORM_EXTRA_INCDIRS)
PLATFORM_EXCLUDES := $(HAL_EXECLUDES) $(DRIVERS_EXECLUDES) $(RTOS_EXECLUDES)\
									$(UTILS_EXECLUDES) $(PLATFORM_EXTRA_EXECLUDES)
PLATFORM_DEFINES := $(HAL_DEFINES) $(DRIVERS_DEFINES) $(RTOS_DEFINES)\
									$(UTILS_DEFINES) $(PLATFORM_EXTRA_DEFINES)

MAIN_SRCDIR := $(PROJECT_PATH)/main
MAIN_INCDIR := $(PROJECT_PATH)/main

SRC_DIR := $(PLATFORM_SRCDIRS) $(MAIN_SRCDIR) $(COMPONENT_SRCDIRS)
INCLUDES := $(PLATFORM_INCDIRS) $(MAIN_INCDIR) $(COMPONENT_ADD_INCLUDEDIRS)
EXCLUDES := $(PLATFORM_EXCLUDES) $(COMPONENT_EXCLUDE_DIRS)
C_DEFINES := $(PLATFORM_DEFINES) $(COMPONENT_DEFINES)

# Get all .c files in SRC_DIR
SRC_C := $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
SRC_C := $(filter-out $(EXCLUDES), $(SRC_C))

INC_H := $(foreach dir, $(INCLUDES), $(wildcard $(dir)/*.h))

SRC_C_TEMP := $(notdir $(SRC_C))

vpath %.c $(SRC_DIR)

OBJECT_LIST := $(patsubst %.c, $(PATH_OBJECTS)/%.o, $(SRC_C_TEMP))

INCLUDES_C := $(addprefix -I, $(INCLUDES))

DEFINES_C := $(addprefix -D, $(C_DEFINES))

.PHONY: all clean clobber flash erase menuconfig setup

all: $(TARGET_ELF) $(TARGET_HEX) $(TARGET_BIN)

$(PATH_OBJECTS)/%.o: %.c
	$(MKDIR) $(PATH_OBJECTS)
	$(CC) -o $@ $(WFLAGS) $(C_FLAGS) $(INCLUDES_C) $(DEFINES_C) $<

$(TARGET_ELF): $(OBJECT_LIST)
	$(MKDIR) $(PATH_BUILD)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(SIZE) $@

$(TARGET_HEX): $(TARGET_ELF)
	$(MAKE_HEX) $< $@

$(TARGET_BIN): $(TARGET_ELF)
	$(MAKE_BIN) $< $@ 

clean:
	$(REMOVE) $(PATH_BUILD) $(PATH_OBJECTS)

clobber:
	$(REMOVE) $(PATH_BUILD) $(PATH_OBJECTS) $(PATH_INCLUDES)\
										$(PROJECT_PATH)/$(PROJECT_NAME)_config.h

flash:
	$(ERASE)
	$(FLASH) $(TARGET_BIN) 0x08000000

erase:
	$(ERASE)

#PATH_ROOT_ENV is exported to child process running kconfig-mconf
menuconfig:
	$(EXPORT_ENV) PATH_ROOT_ENV=$(PATH_ROOT); \
	$(EXPORT_ENV) PROJECT_PATH_ENV=$(PROJECT_PATH); \
	$(EXPORT_ENV) PROJECT_NAME_ENV=$(PROJECT_NAME); \
	$(EXPORT_ENV) KCONFIG_CONFIG=$(CONFIG_FILE);	\
	$(MENUCONFIG) $(KCONFIG_INFILE)
	$(PATH_ROOT)/$(GENCONFIG)
setup:
	$(MKDIR) $(PATH_INCLUDES)
	$(CP) $(INC_H) $(PATH_INCLUDES)
