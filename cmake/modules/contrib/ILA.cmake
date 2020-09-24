if(USE_ILA_CODEGEN STREQUAL "ON") 
  add_definitions(-DUSE_ILA_RUNTIME=1)
  file(GLOB ILA_RELAY_CONTRIB_SRC src/relay/backend/contrib/ila/*.cc)
  list(APPEND COMPILER_SRCS ${ILA_RELAY_CONTRIB_SRC})
  list(APPEND COMPILER_SRCS ${JSON_RELAY_CONTRIB_SRC})

  file(GLOB ILA_CONTRIB_SRC src/runtime/contrib/ila/ila_runtime.cc)
  list(APPEND RUNTIME_SRCS ${ILA_CONTRIB_SRC})
endif()

