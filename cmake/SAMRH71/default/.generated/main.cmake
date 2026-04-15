include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(SAMRH71_default_library_list )

# Handle files with suffix s, for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_assemble)
add_library(SAMRH71_default_default_XC32_assemble OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_assemble})
    SAMRH71_default_default_XC32_assemble_rule(SAMRH71_default_default_XC32_assemble)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_assemble>")

endif()

# Handle files with suffix S, for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
add_library(SAMRH71_default_default_XC32_assembleWithPreprocess OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_assembleWithPreprocess})
    SAMRH71_default_default_XC32_assembleWithPreprocess_rule(SAMRH71_default_default_XC32_assembleWithPreprocess)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_assembleWithPreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_compile)
add_library(SAMRH71_default_default_XC32_compile OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_compile})
    SAMRH71_default_default_XC32_compile_rule(SAMRH71_default_default_XC32_compile)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_compile>")

endif()

# Handle files with suffix cpp, for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_compile_cpp)
add_library(SAMRH71_default_default_XC32_compile_cpp OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_compile_cpp})
    SAMRH71_default_default_XC32_compile_cpp_rule(SAMRH71_default_default_XC32_compile_cpp)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_compile_cpp>")

endif()

# Handle files with suffix [cC], for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_dependentObject)
add_library(SAMRH71_default_default_XC32_dependentObject OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_dependentObject})
    SAMRH71_default_default_XC32_dependentObject_rule(SAMRH71_default_default_XC32_dependentObject)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_dependentObject>")

endif()

# Handle files with suffix elf, for group default-XC32
if(SAMRH71_default_default_XC32_FILE_TYPE_bin2hex)
add_library(SAMRH71_default_default_XC32_bin2hex OBJECT ${SAMRH71_default_default_XC32_FILE_TYPE_bin2hex})
    SAMRH71_default_default_XC32_bin2hex_rule(SAMRH71_default_default_XC32_bin2hex)
    list(APPEND SAMRH71_default_library_list "$<TARGET_OBJECTS:SAMRH71_default_default_XC32_bin2hex>")

endif()


# Main target for this project
add_executable(SAMRH71_default_image_Ws3vy3Sj ${SAMRH71_default_library_list})

set_target_properties(SAMRH71_default_image_Ws3vy3Sj PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${SAMRH71_default_output_dir}")
target_link_libraries(SAMRH71_default_image_Ws3vy3Sj PRIVATE ${SAMRH71_default_default_XC32_FILE_TYPE_link})

# Add the link options from the rule file.
SAMRH71_default_link_rule( SAMRH71_default_image_Ws3vy3Sj)

# Call bin2hex function from the rule file
SAMRH71_default_bin2hex_rule(SAMRH71_default_image_Ws3vy3Sj)
add_custom_target(
    merge_loadable_files ALL
    COMMAND hexmate  c:/Users/User/Documents/research/SAMRH71-1553/SAMRH71.X/dist/default/production/SAMRH71.X.production.hex ${CMAKE_CURRENT_SOURCE_DIR}/../../../out/SAMRH71/default.hex  -O${CMAKE_CURRENT_SOURCE_DIR}/../../../out/SAMRH71/default-unified.hex
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../out/SAMRH71/default-unified.hex
    COMMENT "Merging loadable hex files into unified image")
add_dependencies(merge_loadable_files SAMRH71_default_Bin2Hex)

