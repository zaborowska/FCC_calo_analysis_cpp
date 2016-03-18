include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB sources *.cc)
file(GLOB headers *.h)

# Pay attention to the name of the dictionary...
# you want a unique name, not to pick up another .o file,
# e.g. the one of fcc-edm
ROOT_GENERATE_DICTIONARY(fccanalysiscpp-toolsDict LINKDEF LinkDef.h )
add_library(fccanalysiscpp-tools SHARED ${sources} ${headers} ${}fccanalysiscpp-toolsDict.cxx)

# add_library(fccanalysiscpp-tools SHARED ${sources} ${headers} )

target_link_libraries(fccanalysiscpp-tools utilities datamodel podio ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} )


install(TARGETS fccanalysiscpp-tools DESTINATION lib)
