cmake_minimum_required (VERSION 3.19)
project ("winut")
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("./lib/cpput/")
include_directories("./lib/cpput/include/")
link_libraries("cpput")

add_library(${PROJECT_NAME} STATIC
    "./src/errors.cpp"
    "./src/winapi_exception.cpp"
)
target_include_directories(${PROJECT_NAME} PRIVATE "./include/")

if (${WINUT_DEV_BUILD})
    add_executable("test" "main.cpp")
    target_include_directories("test" PRIVATE "./include/")
    target_link_libraries("test" ${PROJECT_NAME})
endif()
