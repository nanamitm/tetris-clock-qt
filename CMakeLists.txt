cmake_minimum_required(VERSION 3.21)
project(TetrisClock VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(TetrisClock
    main.cpp
    TetrisController.h
    TetrisController.cpp
    DigitPatterns.h
)

set_target_properties(TetrisClock PROPERTIES
    OUTPUT_NAME "tetris-clock"
)

qt_add_qml_module(TetrisClock
    URI TetrisClock
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(TetrisClock PRIVATE Qt6::Quick Qt6::Core)

if(ANDROID)
    # Android 15+: 16KB ページアライメント対応
    add_link_options(-Wl,-z,max-page-size=16384)

    set_target_properties(TetrisClock PROPERTIES
        QT_ANDROID_PACKAGE_NAME              "com.nanamitm.tetrisclock"
        QT_ANDROID_APP_NAME                  "Tetris Clock"
        QT_ANDROID_VERSION_NAME              "${PROJECT_VERSION}"
        QT_ANDROID_VERSION_CODE              1
        QT_ANDROID_PACKAGE_SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_ANDROID_MIN_SDK_VERSION           26
        QT_ANDROID_TARGET_SDK_VERSION        35
        QT_ANDROID_COMPILE_SDK_VERSION       36
        QT_ANDROID_SDK_BUILD_TOOLS_REVISION  37.0.0
    )

    # Qt レベルの APK 署名（Release ビルドかつ環境変数が設定済みの場合）
    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND DEFINED ENV{QT_ANDROID_KEYSTORE_PATH})
        set_target_properties(TetrisClock PROPERTIES
            QT_ANDROID_SIGN_APK            TRUE
            QT_ANDROID_KEYSTORE_PATH       $ENV{QT_ANDROID_KEYSTORE_PATH}
            QT_ANDROID_KEYSTORE_ALIAS      $ENV{QT_ANDROID_KEYSTORE_ALIAS}
            QT_ANDROID_KEYSTORE_STORE_PASS $ENV{QT_ANDROID_KEYSTORE_STORE_PASS}
            QT_ANDROID_KEYSTORE_KEY_PASS   $ENV{QT_ANDROID_KEYSTORE_KEY_PASS}
        )
    endif()
endif()

# Install rules for Desktop platforms (Linux / Windows / macOS)
if(NOT ANDROID)
    install(TARGETS TetrisClock
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
    
    if(UNIX AND NOT APPLE)
        install(FILES tetris-clock.desktop
            DESTINATION share/applications
        )
        install(FILES tetris-clock.svg
            DESTINATION share/icons/hicolor/scalable/apps
        )
    endif()
endif()

