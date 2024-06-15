cmake_minimum_required(VERSION 3.15)    # JUCE requirenment

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(UI_NAME="Driver UI")
add_compile_definitions(JUCE_MODAL_LOOPS_PERMITTED=1)
add_compile_definitions(JUCE_DISPLAY_SPLASH_SCREEN=0)
add_compile_definitions(JUCE_USE_DIRECTWRITE=1)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)

set(TARGET DriverUI)

set(TARGET_VER 1.0.0)

project(${TARGET} VERSION ${TARGET_VER} LANGUAGES C CXX)

find_path(NDI_ADVANCED_INCLUDE_DIRS "Processing.NDI.Advanced.h"
    PATHS "C:/Program Files/NDI/NDI 5 Advanced SDK/Include"
    REQUIRED
)

find_library(NDI_ADVANCED_LIB "Processing.NDI.Lib.Advanced.x64.lib"
	PATHS "C:/Program Files/NDI/NDI 5 Advanced SDK/Lib/x64"
	REQUIRED
)

find_package(JUCE CONFIG REQUIRED)

juce_add_gui_app(${PROJECT_NAME}
	# VERSION ...                       # Set this if the app version is different to the project version
	# ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
	# ICON_SMALL ...
	# DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
	# COMPANY_NAME ...                  # Specify the name of the app's author
	PRODUCT_NAME "${PROJECT_NAME}")             # The name of the final executable, which can differ from the target name

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        JUCE_PLUGINHOST_LADSPA=1
        JUCE_PLUGINHOST_LV2=1
        JUCE_PLUGINHOST_VST3=1
        JUCE_PLUGINHOST_AU=1
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=1     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        #RestCall.hpp
        #RestCall.cpp
        #CtrlPanel.hpp
        #CtrlPanel.cpp
		AboutTab.hpp
		main.cpp
        AboutTab.cpp
        AppLookAndFeel.cpp
        AppLookAndFeel.hpp
        AppSysTrayComponent.cpp
        AppSysTrayComponent.hpp
        HomeTab.cpp
        HomeTab.hpp
        LicenseCheckComponent.cpp
        LicenseCheckComponent.hpp
        LicensePanel.cpp
        LicensePanel.hpp
        MainWindow.cpp
        MainWindow.hpp
		NDISourceFinder.cpp
		NDISourceFinder.hpp
        SearchBox.cpp
        SearchBox.hpp
        Settings.cpp
        Settings.hpp
        Utilities.cpp
        Utilities.hpp
		LicenseConfig.hpp
)


target_include_directories(${TARGET} PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${NDI_ADVANCED_INCLUDE_DIRS}
)

file(GLOB AssetFiles CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/Assets/*")
juce_add_binary_data(ZAssets HEADER_NAME Assets.hpp NAMESPACE Assets SOURCES ${AssetFiles})

juce_generate_juce_header(${TARGET})

target_link_libraries(${TARGET}
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_cryptography
        juce::juce_dsp
        ${NDI_ADVANCED_LIB}

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        ZAssets
)

