cmake_minimum_required(VERSION 3.5.0)
project(ZiplineAirSimClient)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fPIC")
# find and include eigen
include_directories("AirLib/deps/eigen3")

# rpc stuff
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
set(RPCLIB_VERSION_FOLDER rpclib-2.3.0)
set(RPC_LIB_INCLUDES "external/rpclib/${RPCLIB_VERSION_FOLDER}/include")
set(RPC_LIB rpc)

# build RPC lib
add_subdirectory("external/rpclib/rpclib-2.3.0/" "${CMAKE_CURRENT_BINARY_DIR}/rpclib")

# create AirLib library
include_directories(
  AirLib/include
  AirLib
  ${OpenCV_INCLUDE_DIRS}
  ${RPC_LIB_INCLUDES}
)

file(GLOB_RECURSE AirLib_sources
  AirLib/src/api/RpcLibClientBase.cpp
  AirLib/src/common/common_utils/FileSystem.cpp
  AirLib/src/vehicles/multirotor/api/MultirotorApiBase.cpp
  AirLib/src/vehicles/multirotor/api/MultirotorRpcLibClient.cpp
  AirLib/src/safety/SafetyEval.cpp
  AirLib/src/safety/ObstacleMap.cpp
)

add_library(AirLib SHARED ${AirLib_sources})
target_link_libraries(AirLib
  ${RPC_LIB}
)

add_executable(${PROJECT_NAME} "HelloDrone/main.cpp")
target_link_libraries(${PROJECT_NAME}
  AirLib
  ${RPC_LIB}
  ${OpenCV_LIBRARIES}
)

install(TARGETS AirLib
        CONFIGURATIONS Release
        LIBRARY DESTINATION /usr/lib
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/AirLib/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h*"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/AirLib/deps/eigen3/
  DESTINATION include
)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin)
