# Add library cpp files

if (NOT DEFINED MONGOOSE_PATH)
    set(MONGOOSE_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/mongoose")
endif()
if (NOT DEFINED MONGOOSE_PORT)
    set(MONGOOSE_PORT "${CMAKE_CURRENT_LIST_DIR}/port/mongoose")
endif()

message("Using MONGOOSE from ${MONGOOSE_PATH} with Port folder ${MONGOOSE_PORT}")

add_library(mongoose STATIC)

target_sources(mongoose PUBLIC
	${MONGOOSE_PATH}/mongoose.c
)


# Add include directory
target_include_directories(mongoose PUBLIC 
    ${MONGOOSE_PATH}
    ${MONGOOSE_PORT}
)

target_link_libraries(mongoose PUBLIC 
	pico_stdlib
	hardware_pio 
	hardware_dma 
	pico_rand 
	cyw43_driver_picow
	pico_cyw43_arch_lwip_sys_freertos
	FreeRTOS-Kernel
	LWIP_PORT
	)

