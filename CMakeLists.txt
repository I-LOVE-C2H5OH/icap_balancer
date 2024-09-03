cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
add_executable(MyProject src/main.cpp src/TcpClient/TcpClient.cpp src/BufferManager/BufferManager.cpp)
add_subdirectory(external/trantor)
add_subdirectory(external/yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE trantor
                                      PRIVATE yaml-cpp)

# include_directories(${DROGON_SOURCE_DIR}
#                     ${DROGON_SOURCE_DIR}/drogon
#                     ${DROGON_SOURCE_DIR}/trantor/lib/inc)