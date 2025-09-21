cmake_minimum_required(VERSION 3.21)

project(
  chordcat
  LANGUAGES CXX
  VERSION 1.0.0)

message(
  STATUS "Configuring chordcat ${CMAKE_PROJECT_VERSION} - ${CMAKE_BUILD_TYPE}")

set(APP_TARGET_NAME appchordcat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ASSETS_DIR assets)

include(FetchContent)
find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia)

# REMIDI
FetchContent_Declare(
  REMIDI
  GIT_REPOSITORY https://github.com/jcelerier/libremidi.git
  GIT_TAG v4.5.0)
FetchContent_MakeAvailable(REMIDI)

qt_standard_project_setup(REQUIRES 6.9)

include(GNUInstallDirs)

file(GLOB APP_SOURCES "src/*.cpp")
file(GLOB APP_HEADERS "src/*.hpp")

if(WIN32)
  set(desktop_files desktop/chordcat.rc)
endif()

# Use a different name for the executable target to avoid conflicts
qt_add_executable(${APP_TARGET_NAME}
    ${APP_SOURCES}
    ${APP_HEADERS}
    resources.qrc
    ${desktop_files}
)

if(ANDROID)
    message(STATUS "Configuring for Android: Using pre-built FluidSynth binaries.")
    set(FLUIDSYNTH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/android_deps/fluidsynth-2.4.7-android24)
    set(FLUIDSYNTH_INCLUDE_DIR ${FLUIDSYNTH_ROOT}/include)
    # ANDROID_ABI is set by the Android toolchain (e.g; arm64-v8a)
    set(FLUIDSYNTH_LIB_DIR ${FLUIDSYNTH_ROOT}/lib/${ANDROID_ABI})

    # Add FluidSynth include directories
    target_include_directories(${APP_TARGET_NAME} PRIVATE
        ${FLUIDSYNTH_INCLUDE_DIR}
    )

    # Link FluidSynth libraries and other common Android NDK libraries
    target_link_libraries(${APP_TARGET_NAME} PRIVATE
        ${FLUIDSYNTH_LIB_DIR}/libfluidsynth.so
        android
        log
        OpenSLES
    )
else()
    # Original FluidSynth find_package for non-Android builds
    find_package(FluidSynth REQUIRED)
endif()

set_source_files_properties(AppSettings.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

# The QML module can share the project name, as the executable target is now different
qt_add_qml_module(ChordcatQmlModule
    URI Chordcat
    VERSION 1.0
    QML_FILES
        Main.qml
        PianoKeyboard.qml
        Preferences.qml
        InstrumentTable.qml
        AboutDialog.qml
        AppSettings.qml
)

target_include_directories(${APP_TARGET_NAME} PUBLIC src)

# Set properties on the new executable target
set_target_properties(${APP_TARGET_NAME} PROPERTIES
    # Ensure the final executable is named "chordcat"
    OUTPUT_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    ANDROID_PACKAGE_NAME "dev.ters.chordcat"
)

# Link libraries to the new executable target
target_link_libraries(${APP_TARGET_NAME}
    PRIVATE Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Multimedia
)
target_link_libraries(${APP_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:libremidi>)

# Install the new executable target
install(TARGETS ${APP_TARGET_NAME} COMPONENT Application
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install soundfonts to the user's local data directory
install(
  DIRECTORY assets/soundfonts
  DESTINATION ${CMAKE_INSTALL_DATADIR}/Ters/Chordcat
  COMPONENT Application
)

if(WIN32)
  # Install windows ico
  install(
    FILES desktop/chordcat.ico
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  COMPONENT Application)

  # Install DLLs for the new target
  install(
    TARGETS ${APP_TARGET_NAME}
    COMPONENT Application
    RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES
    "api-ms-"
    "ext-ms-"
    POST_EXCLUDE_REGEXES
    ".*system32/.*\\.dll"
    DIRECTORIES
    $<TARGET_FILE_DIR:${APP_TARGET_NAME}>)
endif()

# Install Desktop Files for Linux (Unix but not Apple)
if(UNIX AND NOT APPLE)
  install(
    FILES desktop/dev.ters.Chordcat.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    COMPONENT Application)
  install(
    FILES desktop/16x16/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps
    COMPONENT Application)
  install(
    FILES desktop/22x22/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/22x22/apps
    COMPONENT Application)
  install(
    FILES desktop/24x24/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps
    COMPONENT Application)
  install(
    FILES desktop/32x32/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps
    COMPONENT Application)
  install(
    FILES desktop/36x36/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/36x36/apps
    COMPONENT Application)
  install(
    FILES desktop/44x44/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/44x44/apps
    COMPONENT Application)
  install(
    FILES desktop/48x48/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps
    COMPONENT Application)
  install(
    FILES desktop/64x64/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps
    COMPONENT Application)
  install(
    FILES desktop/72x72/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/72x72/apps
    COMPONENT Application)
  install(
    FILES desktop/96x96/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/96x96/apps
    COMPONENT Application)
  install(
    FILES desktop/128x128/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
    COMPONENT Application)
  install(
    FILES desktop/150x150/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/150x150/apps
    COMPONENT Application)
  install(
    FILES desktop/192x192/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/192x192/apps
    COMPONENT Application)
  install(
    FILES desktop/256x256/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
    COMPONENT Application)
  install(
    FILES desktop/310x310/chordcat.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/310x310/apps
    COMPONENT Application)
  install(
    FILES desktop/dev.ters.Chordcat.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
    COMPONENT Application)
endif()

# Define components (Development component is a placeholder for future)
set(CPACK_COMPONENTS_ALL Application Development)
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Chordcat Application")
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development Files")

# Set the default component (pre-selected in the install wizard)
set(CPACK_COMPONENT_APPLICATION_HIDDEN FALSE)
set(CPACK_COMPONENT_DEVELOPMENT_HIDDEN TRUE)

# Component-specific descriptions
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "The main application")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Development files (headers, etc.)")

# CPack config
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chord naming application")
set(CPACK_PACKAGE_VENDOR "Ters")
set(CPACK_PACKAGE_CONTACT "s20n@ters.dev")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_IGNORE_FILES "build" ".git" ".github" ".gitignore" ".clangd"
                              ".clang-format")

if(WIN32)
  # NSIS specific configuration
  include(InstallRequiredSystemLibraries)
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
  set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
  # Installer icon
  set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/desktop\chordcat.ico")
  # Icon in the add/remove programs list in control panel. can be .exe or ico.
  set(CPACK_NSIS_INSTALLED_ICON_NAME
      "share\${PROJECT_NAME}\${PROJECT_NAME}.ico")
  set(CPACK_NSIS_HELP_LINK "https://shriramters.github.io/chordcat")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://shriramters.github.io/chordcat")
  set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PROJECT_NAME})
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME
      "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

  # Custom installation directory without version number
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "chordcat")

  # Define Start Menu shortcut
  set(CPACK_NSIS_CREATE_ICONS_EXTRA
      "CreateShortCut '$SMPROGRAMS\$STARTMENU_FOLDER\Chordcat.lnk' '$INSTDIR\bin\${PROJECT_NAME}.exe'")

  # Define uninstall command for start menu shortcut
  set(CPACK_NSIS_DELETE_ICONS_EXTRA
      "Delete '$SMPROGRAMS\$STARTMENU_FOLDER\Chordcat.lnk'")

  set(CPACK_GENERATOR "NSIS")
endif()

if(UNIX AND NOT APPLE)
  # Temporarily setting to DEB. Will support RPMs in the future.
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Shriram Ravindranathan <s20n@ters.dev>")
  set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_SET_DESTDIR ON)
endif()

include(CPack)
