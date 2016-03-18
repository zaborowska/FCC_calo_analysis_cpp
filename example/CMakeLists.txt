
add_executable(fccanalysiscpp-read read.cc)

target_link_libraries(fccanalysiscpp-read utilities datamodel datamodelDict podio)

install(TARGETS fccanalysiscpp-read DESTINATION bin)


add_executable(read-calo read-calo.cc)

target_link_libraries(read-calo utilities datamodel datamodelDict podio)

install(TARGETS read-calo DESTINATION bin)


