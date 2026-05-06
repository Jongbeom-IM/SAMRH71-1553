# The following variables contains the files used by the different stages of the build process.
set(SAMRH71_default_default_XC32_FILE_TYPE_assemble)
set_source_files_properties(${SAMRH71_default_default_XC32_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${SAMRH71_default_default_XC32_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(SAMRH71_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${SAMRH71_default_default_XC32_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${SAMRH71_default_default_XC32_FILE_TYPE_assembleWithPreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(SAMRH71_default_default_XC32_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/exceptions.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/initialization.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/interrupts.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/libc_syscalls.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/clk/plib_clk.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom0_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom1_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom2_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom3_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom4_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/flexcom/usart/plib_flexcom5_usart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/matrix/plib_matrix.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/nvic/plib_nvic.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/peripheral/pio/plib_pio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/startup_xc32.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/stdio/xc32_monitor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/main.c")
set_source_files_properties(${SAMRH71_default_default_XC32_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(SAMRH71_default_default_XC32_FILE_TYPE_compile_cpp)
set_source_files_properties(${SAMRH71_default_default_XC32_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(SAMRH71_default_default_XC32_FILE_TYPE_link)
set(SAMRH71_default_default_XC32_FILE_TYPE_bin2hex)

# The linker script used for the build.
set(SAMRH71_default_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default/ATSAMRH71F20C.ld")
set(SAMRH71_default_image_name "default.elf")
set(SAMRH71_default_image_base_name "default")

# The output directory of the final image.
set(SAMRH71_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/SAMRH71")

# The full path to the final image.
set(SAMRH71_default_full_path_to_image ${SAMRH71_default_output_dir}/${SAMRH71_default_image_name})
