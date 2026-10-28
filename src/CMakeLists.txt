# This is the main binary. List your C files here.
add_executable(SemaphoreInversion
    SemaphoreInversion.c
)

pico_set_program_name(SemaphoreInversion "test")
pico_set_program_version(SemaphoreInversion "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(SemaphoreInversion 1)
pico_enable_stdio_usb(SemaphoreInversion 0)

# Add the libraries to the build
target_link_libraries(SemaphoreInversion PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(SemaphoreInversion PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(SemaphoreInversion)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_0 ${PICOTOOL} load -f $<TARGET_FILE:SemaphoreInversion> DEPENDS SemaphoreInversion)
add_custom_target(openocd_flash_0 ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:SemaphoreInversion>" verify reset exit' DEPENDS SemaphoreInversion)


# This is the main binary. List your C files here.
add_executable(MutexInversion
    MutexInversion.c
)

pico_set_program_name(MutexInversion "test")
pico_set_program_version(MutexInversion "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(MutexInversion 1)
pico_enable_stdio_usb(MutexInversion 0)

# Add the libraries to the build
target_link_libraries(MutexInversion PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(MutexInversion PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(MutexInversion)

find_program(OPENOCD openocd)
find_program(PICOTOOL picotool)

add_custom_target(flash_1 ${PICOTOOL} load -f $<TARGET_FILE:MutexInversion> DEPENDS MutexInversion)
add_custom_target(openocd_flash_1 ${OPENOCD} -s ${OPENOCD_PATH} -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000' -c program "$<TARGET_FILE:MutexInversion>" verify reset exit' DEPENDS MutexInversion)
