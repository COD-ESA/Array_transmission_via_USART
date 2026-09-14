cmake_minimum_required(VERSION 3.21)

project(Array_transmission_via_USART C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- РАЗДЕЛ НАСТРОЙКИ НАБОРА ФЛАГОВ ---
# флаги сообщений 
set(MY_DEVELOPER_WARNINGS
    -Wall
    -Wextra # неиспользуемые параметры функций
    -Werror # Трактует все предупреждения как ошибки
    -Wshadow # предупреждать если има локальной переменной затеняет глобальную
    -Wconversion # предупреждает о неявных приведениях типов, которые могут привести к потере данных. 
    -Wstrict-prototypes # кричать матом если объявляю или определяю функцию без явного указания типов её аргументов.
)

# флаги оптимизации
set(CPU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

# Флаги оптимизации и секций (общие для оптимизации размера)
set(SECTION_FLAGS
    -ffunction-sections # раскладывает по отдельным секциям код функции.
    -fdata-sections # раскладывает по отдельным секциям данные глобальные и статические переменные.
)

set(OPTIMIZ_FLAGS_DEBUG
    -fanalyzer
    -Og
    -g3
    -gdwarf-4 # для отладочной информации
    ${SECTION_FLAGS}
    -save-temps=obj # для сохранения промежуточных файлов компиляции
)

set(OPTIMIZ_FLAGS_RELEASE
    ${SECTION_FLAGS}
    -O3 # Максимальная оптимизация по скорости
) 


set(LINK_FLAGS
    ${CPU_FLAGS}
    LINKER:--gc-sections #удалить неиспользуемые функции
    LINKER:-T,${CMAKE_SOURCE_DIR}/LdScript/linkerScript.ld
)


add_executable(ATVU)
add_executable(ATVU_SLAVE)

target_sources(ATVU PRIVATE
    Src/main.c
    Src/ManualSystemInit.c
    Src/InitFlash.c
    Src/ledLight.c
    Src/mco1.c
    Src/initDMA1.c
    Startup/startup.c
    Src/writeFlash.c
    Src/activateReceptionUsart.c
    Src/IRQn.c
    Src/initUsart.c
    Src/branch_initUsart.S
    Src/numberCounter.c
    Src/readValueRegistr.c
    Src/initTimer.c
)

target_sources( ATVU_SLAVE PRIVATE
    Src/slave/mainSlave.c
    Src/ManualSystemInit.c
    Src/InitFlash.c
    Src/ledLight.c
    Src/mco1.c
    Src/initDMA1.c
    Startup/startup.c
    Src/writeFlash.c
    # Src/activateReceptionUsart.c
    Src/IRQn.c
    Src/slave/initUsartSlave.c
    Src/branch_initUsart.S
    Src/numberCounter.c
    Src/readValueRegistr.c
    # Src/slave/getDataUsartSaveInFlash.c
    Src/slave/specialSettingUsart.c
    Src/slave/activateReceptionUsartSlave.c
)

# создание переменной COMMON_INCLUDES_BLACK и запись в нее общих исходников для F411
# Проверяем, задан ли путь через пресеты. Если нет — можно задать дефолтный.
if(NOT DEFINED STM32_FW)
    set(STM32_FW "G:/Repositoris STM/STM32Cube_FW_F4_V1.27.1") # Запасной путь
endif()

message(STATUS "STM32_FW='${STM32_FW}'")

set(COMMON_INCLUDES_F411
    "${STM32_FW}/Drivers/CMSIS/Include"
    "${STM32_FW}/Drivers/CMSIS/Core/Include"
    "${STM32_FW}/Drivers/CMSIS/Device/ST/STM32F4xx/Include"
)

message(STATUS "COMMON_INCLUDES_F411:")
foreach(dir IN LISTS COMMON_INCLUDES_F411)
    message(STATUS "  ${dir}")
endforeach()

# Добавление исходников для платы black_pill
target_include_directories(ATVU PRIVATE
    Inc
    ${COMMON_INCLUDES_F411}
)

# Добавление исходников для платы black_pill ведомой
target_include_directories(ATVU_SLAVE PRIVATE
    Inc
    ${COMMON_INCLUDES_F411}
)

target_compile_definitions(ATVU PRIVATE STM32F411xE)
set_target_properties(ATVU PROPERTIES SUFFIX ".elf")

target_compile_definitions(ATVU_SLAVE PRIVATE STM32F411xE)
set_target_properties(ATVU_SLAVE PROPERTIES SUFFIX ".elf")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ATVU PRIVATE ${CPU_FLAGS} ${MY_DEVELOPER_WARNINGS} ${OPTIMIZ_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(ATVU PRIVATE ${CPU_FLAGS} ${MY_DEVELOPER_WARNINGS} ${OPTIMIZ_FLAGS_RELEASE})
endif()    


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(ATVU PRIVATE
        ${LINK_FLAGS}
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(ATVU PRIVATE
        ${LINK_FLAGS}
        -s
        -flto
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ATVU_SLAVE PRIVATE ${CPU_FLAGS} ${MY_DEVELOPER_WARNINGS} ${OPTIMIZ_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(ATVU_SLAVE PRIVATE ${CPU_FLAGS} ${MY_DEVELOPER_WARNINGS} ${OPTIMIZ_FLAGS_RELEASE})
endif()    


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(ATVU_SLAVE PRIVATE
        ${LINK_FLAGS}
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(ATVU_SLAVE PRIVATE
        ${LINK_FLAGS}
        -s
        -flto
    )
endif()

# Поиск программы arm-none-eabi-size
find_program(ARM_SIZE
    NAMES arm-none-eabi-size
    REQUIRED
)
# Проверка, что программа найдена
message( "ARM_SIZE" =  ${ARM_SIZE})
# для сборки F411
add_custom_command(TARGET ATVU POST_BUILD

    COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:ATVU>
        $<TARGET_FILE_DIR:ATVU>/ATVU.hex

    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:ATVU>
        $<TARGET_FILE_DIR:ATVU>/ATVU.bin  
        
    COMMAND ${ARM_SIZE} $<TARGET_FILE:ATVU>
        
    COMMENT "Generating HEX and BIN for ATVU. Calculating size of ATVU.elf"
)

add_custom_command(TARGET ATVU_SLAVE POST_BUILD

    COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:ATVU_SLAVE>
        $<TARGET_FILE_DIR:ATVU_SLAVE>/ATVU_SLAVE.hex

    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:ATVU_SLAVE>
        $<TARGET_FILE_DIR:ATVU_SLAVE>/ATVU_SLAVE.bin  
        
    COMMAND ${ARM_SIZE} $<TARGET_FILE:ATVU_SLAVE>
        
    COMMENT "Generating HEX and BIN for ATVU_SLAVE. Calculating size of ATVU_SLAVE.elf"
)