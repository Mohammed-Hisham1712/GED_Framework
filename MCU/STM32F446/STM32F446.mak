#Toolchain commands
TOOLCHAIN_PREFIX = $(CMDECHO)arm-none-eabi-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)gcc
SIZE = $(TOOLCHAIN_PREFIX)size
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
SIZE = $(TOOLCHAIN_PREFIX)size
MAKE_HEX = $(OBJCOPY) -O ihex
MAKE_BIN = $(OBJCOPY) -O binary
ST_FLASH = st-flash
FLASH = $(ST_FLASH) write
ERASE = $(ST_FLASH) erase

#MCU specif flags
CPU = -mcpu=cortex-m4
FPU = -mfpu=auto
FLOAT-ABI = -mfloat-abi=soft
THUMB = -mthumb

MCU = $(CPU) $(THUMB) $(FPU) $(FLOAT-ABI)

#Compiler flags
WFLAGS := -Wall -Werror \
		  -Wno-error=unused-function \
		  -Wno-error=unused-variable \
		  -Wno-error=unused-but-set-variable

C_FLAGS := $(MCU) -std=gnu99 -c

#DEBUG or RELEASE options
ifeq ("$(DEBUG)", "1")
C_FLAGS += -O0 -g3 -D_DEBUG=1
else
C_FLAGS += -O2
endif

#-nostdlib
#Do not use the standard system startup files or libraries when linking. 
#No startup files and only the libraries you specify will be passed to the linker. 
#The compiler may generate calls to memcmp, memset, memcpy and memmove.
#These entries are usually resolved by entries in libc. 
#These entry points should be supplied through some other mechanism when this option is 
#specified.

#One of the standard libraries bypassed by -nostdlib and -nodefaultlibs is libgcc.a, a
#library of internal subroutines that GCC uses to overcome shortcomings of particular 
#machines, or special needs for some languages. 
#(See Interfacing to GCC Output, for more discussion of libgcc.a.) In most cases, 
#you need libgcc.a even when you want to avoid other standard libraries.
#In other words, when you specify -nostdlib or -nodefaultlibs you should usually 
#specify -lgcc as well. This ensures that you have no unresolved references to internal
#GCC library subroutines.

LIB_C_PATH := "/opt/gcc-arm/arm-none-eabi/lib/thumb/v7-m/nofp"
LIB_GCC_PATH := "/opt/gcc-arm/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp"

LD_LIBS := -nostdlib -specs=nano.specs
LD_LIBS += -L$(LIB_C_PATH) -lc
LD_LIBS += -L$(LIB_GCC_PATH) -lgcc
LD_SCRIPT := $(PATH_MCU)/$(TARGET_MCU).ld

LDFLAGS := -T$(LD_SCRIPT) $(LD_LIBS) -Wl,-Map=$(PATH_BUILD)/$(PROJECT_NAME).map
