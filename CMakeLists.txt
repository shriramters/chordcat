cmake_minimum_required(VERSION 3.21)
project(chordcat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
# SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# REMIDI
FetchContent_Declare(REMIDI
    GIT_REPOSITORY https://github.com/jcelerier/libremidi.git
    GIT_TAG v4.2.4)
FetchContent_MakeAvailable(REMIDI)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB src_files CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${src_files})
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-system)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:libremidi>)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ${PROJECT_NAME})