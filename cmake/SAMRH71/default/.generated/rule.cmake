# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "C:/Users/User/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(SAMRH71_default_default_XC32_assemble_rule target)
    set(options
        "-g"
        "${ASSEMBLER_PRE}"
        "-mprocessor=ATSAMRH71F20C"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST},--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--gdwarf-2,-I${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target} PRIVATE "__DEBUG=1")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X")
endfunction()
function(SAMRH71_default_default_XC32_assembleWithPreprocess_rule target)
    set(options
        "-x"
        "assembler-with-cpp"
        "-g"
        "${MP_EXTRA_AS_PRE}"
        "${DEBUGGER_NAME_AS_MACRO}"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c"
        "-mprocessor=ATSAMRH71F20C"
        "-mdtcm=0x40000"
        "-mitcm=0x20000"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST},--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1,-I${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X")
endfunction()
function(SAMRH71_default_default_XC32_compile_rule target)
    set(options
        "-g"
        "${CC_PRE}"
        "-x"
        "c"
        "-c"
        "-mprocessor=ATSAMRH71F20C"
        "-ffunction-sections"
        "-fdata-sections"
        "-O1"
        "-fno-common"
        "-mdtcm=0x40000"
        "-mitcm=0x20000"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/ATSAMRH71F20C_DFP"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/CMSIS"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/CMSIS/CMSIS/Core/Include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X"
        PRIVATE "${PACK_REPO_PATH}/ARM/CMSIS/5.4.0/CMSIS/Core/Include")
endfunction()
function(SAMRH71_default_default_XC32_compile_cpp_rule target)
    set(options
        "-g"
        "${CC_PRE}"
        "${DEBUGGER_NAME_AS_MACRO}"
        "-mprocessor=ATSAMRH71F20C"
        "-frtti"
        "-fexceptions"
        "-fno-check-new"
        "-fenforce-eh-specs"
        "-ffunction-sections"
        "-O1"
        "-fno-common"
        "-mdtcm=0x40000"
        "-mitcm=0x20000"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config/default"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/ATSAMRH71F20C_DFP"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/CMSIS"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/packs/CMSIS/CMSIS/Core/Include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X"
        PRIVATE "${PACK_REPO_PATH}/ARM/CMSIS/5.4.0/CMSIS/Core/Include")
endfunction()
function(SAMRH71_default_dependentObject_rule target)
    set(options
        "-mprocessor=ATSAMRH71F20C"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
endfunction()
function(SAMRH71_default_link_rule target)
    set(options
        "-g"
        "${MP_EXTRA_LD_PRE}"
        "${DEBUGGER_OPTION_TO_LINKER}"
        "${DEBUGGER_NAME_AS_MACRO}"
        "-mprocessor=ATSAMRH71F20C"
        "-mno-device-startup-code"
        "-mdtcm=0x40000"
        "-mitcm=0x20000"
        "-Wl,--defsym=__MPLAB_BUILD=1${MP_EXTRA_LD_POST},--script=${SAMRH71_default_LINKER_SCRIPT},--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=_min_heap_size=512,--gc-sections,-L${CMAKE_CURRENT_SOURCE_DIR}/../../../SAMRH71.X,-Map=mem.map,--memorysummary,memoryfile.xml"
        "-mdfp=${PACK_REPO_PATH}/Microchip/SAMRH71_DFP/2.6.253/samrh71c")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target} PRIVATE "XPRJ_default=default")
endfunction()
function(SAMRH71_default_bin2hex_rule target)
    add_custom_target(
        SAMRH71_default_Bin2Hex ALL
        COMMAND ${MP_BIN2HEX} ${SAMRH71_default_image_name}
        WORKING_DIRECTORY ${SAMRH71_default_output_dir}
        BYPRODUCTS "${SAMRH71_default_output_dir}/${SAMRH71_default_image_base_name}.hex"
        COMMENT "Convert build file to .hex")
    add_dependencies(SAMRH71_default_Bin2Hex ${target})
endfunction()
