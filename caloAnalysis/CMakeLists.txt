include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB sources *.cc)
file(GLOB headers *.h)

# Pay attention to the name of the dictionary...
# you want a unique name, not to pick up another .o file,
# e.g. the one of fcc-edm
ROOT_GENERATE_DICTIONARY(caloanalysis-CaloAnalysisDict CaloAnalysis.h LINKDEF LinkDef.h )

add_library(caloanalysis-myanalysis SHARED ${sources} ${headers} ${}caloanalysis-CaloAnalysisDict.cxx)

target_link_libraries(caloanalysis-myanalysis utilities datamodel datamodelDict podio ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

install(TARGETS caloanalysis-myanalysis DESTINATION lib)

# Install the dictionary
if (${ROOT_VERSION} GREATER 6)
  install(FILES
      "${PROJECT_BINARY_DIR}/caloAnalysis/caloanalysis-CaloAnalysisDict_rdict.pcm"
      DESTINATION lib COMPONENT dev)
endif()

