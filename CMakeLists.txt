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
    GIT_TAG v4.2.4)
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

if(APPLE)
    set(APP_ASSETS_PATH "Resources/${ASSETS_DIR}")
endif()

configure_file(src/config.h.in config/config.h)

file(GLOB src_files CONFIGURE_DEPENDS "src/*.cpp")
include_directories(${CMAKE_BINARY_DIR}/config)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources)

set(ICON_RESOURCE ${PROJECT_SOURCE_DIR}/desktop/chordcat.icns)
add_executable(chordcat MACOSX_BUNDLE
    ${ICON_RESOURCE}
    ${src_files}
)

target_link_libraries(${PROJECT_NAME} PRIVATE libfluidsynth.a sfml-graphics sfml-audio sfml-system ImGui-SFML::ImGui-SFML Boost::boost)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:libremidi>)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (WIN32 AND BUILD_SHARED_LIBS)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD COMMAND 
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Frameworks/"
    $<TARGET_FILE:${PROJECT_NAME}>)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "chordcat")
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.chordcat.chordcat")
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE chordcat.icns)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/desktop/MacOSXBundleInfo.plist.in)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/desktop/chordcat.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

install(TARGETS ${PROJECT_NAME}
BUNDLE DESTINATION . COMPONENT Runtime
RUNTIME DESTINATION bin COMPONENT Runtime
)

set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
set(DIRS ${CMAKE_BINARY_DIR})

# install(CODE "include(BundleUtilities)
#     fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "chordcat")
set(CPACK_PACKAGE_CONTACT "shriram <shriram@ters.dev>")
set(HOMEPAGE "https://github.com/shriramters/chordcat")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Clean cpp project. This is simple cpp example project build with CMake.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/out")

#--------------------------------------------------------------------------
# Apple specific
set(CPACK_GENERATOR "DragNDrop")
# The folder called ALL_IN_ONE
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(MACOSX_BUNDLE_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
set(MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.chordcat.chordcat")
set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_SOURCE_DIR}/desktop/chordcat.icns)
set(MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/desktop/MacOSXBundleInfo.plist.in)
set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set_source_files_properties(${PROJECT_SOURCE_DIR}/desktop/chordcat.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

set(CPACK_DMG_VOLUME_NAME "chordcat-dnd")
set(CPACK_OSX_PACKAGE_VERSION "10.6") #min package version

include(CPack)