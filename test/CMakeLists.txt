find_package(GTest REQUIRED)

enable_testing()


add_executable(gtest_outerjoin gtest_outerjoin.cpp ../simpledb.cpp ../pcommand.cpp)

set_target_properties(gtest_outerjoin PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(gtest_outerjoin
  PRIVATE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/src"
)


target_link_libraries(gtest_outerjoin
  PRIVATE
  GTest::gtest_main
)

gtest_discover_tests(gtest_outerjoin)
