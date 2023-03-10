set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(WARNING_FLAGS
                    "-Wall -Werror" \
                    "-Wno-error=unused-function"    \
                    "-Wno-error=unused-variable"    \
                    "-Wno-error=unused-but-set-variable")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_C "elf")
set(CMAKE_EXECUTABLE_SUFFIX_ASM "elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)