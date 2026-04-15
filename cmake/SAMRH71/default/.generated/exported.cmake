set(DEPENDENT_MP_BIN2HEXSAMRH71_default_Ws3vy3Sj "c:/Program Files/Microchip/xc32/v5.00/bin/xc32-bin2hex.exe")
set(DEPENDENT_DEPENDENT_TARGET_ELFSAMRH71_default_Ws3vy3Sj ${CMAKE_CURRENT_LIST_DIR}/../../../../out/SAMRH71/default.elf)
set(DEPENDENT_TARGET_DIRSAMRH71_default_Ws3vy3Sj ${CMAKE_CURRENT_LIST_DIR}/../../../../out/SAMRH71)
set(DEPENDENT_BYPRODUCTSSAMRH71_default_Ws3vy3Sj ${DEPENDENT_TARGET_DIRSAMRH71_default_Ws3vy3Sj}/${sourceFileNameSAMRH71_default_Ws3vy3Sj}.c)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRSAMRH71_default_Ws3vy3Sj}/${sourceFileNameSAMRH71_default_Ws3vy3Sj}.c
    COMMAND ${DEPENDENT_MP_BIN2HEXSAMRH71_default_Ws3vy3Sj} --image ${DEPENDENT_DEPENDENT_TARGET_ELFSAMRH71_default_Ws3vy3Sj} --image-generated-c ${sourceFileNameSAMRH71_default_Ws3vy3Sj}.c --image-generated-h ${sourceFileNameSAMRH71_default_Ws3vy3Sj}.h --image-copy-mode ${modeSAMRH71_default_Ws3vy3Sj} --image-offset ${addressSAMRH71_default_Ws3vy3Sj} 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRSAMRH71_default_Ws3vy3Sj}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFSAMRH71_default_Ws3vy3Sj})
add_custom_target(
    dependent_produced_source_artifactSAMRH71_default_Ws3vy3Sj 
    DEPENDS ${DEPENDENT_TARGET_DIRSAMRH71_default_Ws3vy3Sj}/${sourceFileNameSAMRH71_default_Ws3vy3Sj}.c
    )
