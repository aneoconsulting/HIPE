include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

list(REMOVE_ITEM BOOST_COMPONENTS program_options)
list(APPEND BOOST_COMPONENTS unit_test_framework)

# FIXME
# This seems to be unused. In any case, The FindOPenSSL modules should be used instead.
if(APPLE)
  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif(APPLE)

if(${Boost_FOUND})
  link_directories(${Boost_LIBRARY_DIRS})
endif(${Boost_FOUND})
