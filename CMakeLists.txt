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
    LINKER:-T,LdScript/linkerScript.ld
)

add_executable(ATVU)

target_sources(ATVU PRIVATE
    Src/main.c
    Startup/startup.c
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

target_compile_definitions(ATVU PRIVATE STM32F411xE)

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

# для сборки F411
add_custom_command(TARGET ATVU POST_BUILD

    COMMAND arm-none-eabi-objcopy -O ihex
        $<TARGET_FILE:ATVU>
        $<TARGET_FILE_DIR:ATVU>/ATVU.hex

    COMMAND arm-none-eabi-objcopy -O binary
        $<TARGET_FILE:ATVU>
        $<TARGET_FILE_DIR:ATVU>/ATVU.bin   
        
    COMMENT "Generating HEX and BIN for ATVU"
)

add_custom_command(TARGET ATVU POST_BUILD

    COMMAND arm-none-eabi-size $<TARGET_FILE:ATVU>

    COMMENT "Calculating size of ATVU.elf"
)