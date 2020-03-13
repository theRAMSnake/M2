file(GLOB ProtoFiles "*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

add_library(m2commonMessages STATIC ${ProtoSources} ${ProtoHeaders})
target_link_libraries(m2commonMessages ${Boost_LIBRARIES} ${PROTOBUF_LIBRARY})

