cmake_minimum_required(VERSION 3.30...4.1)

project(BoostFilesystem LANGUAGES CXX)

enable_testing()

find_package(Boost REQUIRED COMPONENTS filesystem)

add_executable(windows_root boost_root.cpp)
target_link_libraries(windows_root PRIVATE Boost::filesystem)
add_test(NAME WindowsRootBoost COMMAND windows_root)

file(GENERATE OUTPUT .gitignore CONTENT "*")
