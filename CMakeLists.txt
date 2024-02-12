cmake_minimum_required(VERSION 3.21)

project(chordcat LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR assets)

include(FetchContent)
# SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# REMIDI
FetchContent_Declare(REMIDI
    GIT_REPOSITORY https://github.com/jcelerier/libremidi.git
    GIT_TAG v4.4.0)
FetchContent_MakeAvailable(REMIDI)

# FluidSynth
FetchContent_Declare(FLUIDSYNTH
    GIT_REPOSITORY https://github.com/FluidSynth/fluidsynth.git
    GIT_TAG v2.3.4)
FetchContent_MakeAvailable(FLUIDSYNTH)

# ImGui
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.1)
FetchContent_MakeAvailable(ImGui)

# ImGui-SFML
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6)
FetchContent_MakeAvailable(ImGui-SFML)

# Boost Property Tree
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.74.0)
include_directories(${Boost_INCLUDE_DIRS})


option(RELEASE_MODE "Flag that defines if cmake is running in release mode or development mode. Runs in development mode by default" OFF)

include(GNUInstallDirs)

# Copy files to build dir if in development or install to CMAKE_INSTALL_DATADIR
if( ${RELEASE_MODE} )
  set(APP_ASSETS_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${ASSETS_DIR})
else()
  file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_BINARY_DIR}/${APP_ASSETS_PATH})
  set(APP_ASSETS_PATH ${CMAKE_BINARY_DIR}/${ASSETS_DIR})
endif()

configure_file(src/config.h.in config/config.h)


file(GLOB src_files CONFIGURE_DEPENDS "src/*.cpp")
include_directories(${CMAKE_BINARY_DIR}/config)

add_executable(${PROJECT_NAME} ${src_files})
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-system libfluidsynth ImGui-SFML::ImGui-SFML Boost::boost)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:libremidi>)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY ${ASSETS_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
# Install Desktop Files for Linux (Unix but not Apple)
if( UNIX AND NOT APPLE )
  Install(FILES desktop/chordcat.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  Install(FILES desktop/16x16/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps)
  Install(FILES desktop/22x22/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/22x22/apps)
  Install(FILES desktop/24x24/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps)
  Install(FILES desktop/32x32/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
  Install(FILES desktop/36x36/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/36x36/apps)
  Install(FILES desktop/44x44/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/44x44/apps)
  Install(FILES desktop/48x48/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps)
  Install(FILES desktop/64x64/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps)
  Install(FILES desktop/72x72/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/72x72/apps)
  Install(FILES desktop/96x96/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/96x96/apps)
  Install(FILES desktop/128x128/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps)
  Install(FILES desktop/150x150/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/150x150/apps)
  Install(FILES desktop/192x192/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/192x192/apps)
  Install(FILES desktop/256x256/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)
  Install(FILES desktop/310x310/chordcat.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/310x310/apps)
endif()


# CPack config
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chord naming application")
set(CPACK_PACKAGE_VENDOR "Ters")
set(CPACK_PACKAGE_CONTACT "s20n@ters.dev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Shriram Ravindranathan")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_SET_DESTDIR ON)
set(CPACK_SOURCE_IGNORE_FILES "build" ".git" ".github" ".gitignore" ".clangd" ".clang-format")
include(CPack)
