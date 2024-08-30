cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
add_executable(MyProject src/main.cpp)
#add_subdirectory(external/drogon)
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)

# include_directories(${DROGON_SOURCE_DIR}
#                     ${DROGON_SOURCE_DIR}/drogon
#                     ${DROGON_SOURCE_DIR}/trantor/lib/inc)