add_executable(doxygen2docset_unittests
  "test_main.cc"
  "doxygen2docset_unittests.cc"
)

target_link_libraries(doxygen2docset_unittests
  PRIVATE
    gtest
    doxygen2docset_lib
)
