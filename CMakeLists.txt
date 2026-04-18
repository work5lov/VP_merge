cmake_minimum_required(VERSION 3.16)
project(VP_merge VERSION 1.0 LANGUAGES CXX)

# Настройка Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS PrintSupport Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport Widgets Quick)

# Отключаем CPack для QXlsx
set(QXLSX_SKIP_CPACK TRUE CACHE BOOL "Skip CPack configuration for QXlsx")
add_subdirectory(QXlsx/QXlsx)

# Добавление QXlsx
# include_directories(${CMAKE_SOURCE_DIR}/header)
# file(GLOB QXLSX_SOURCES "${CMAKE_SOURCE_DIR}/source/*.cpp")
# add_library(qxlsx STATIC ${QXLSX_SOURCES})

# Источники приложения
set(PROJECT_SOURCES
  main.cpp
  docpainter.cpp
  painterprinter.cpp
  docpainter.h
  painterprinter.h
  Structures.h
  appcontroller.h
  appcontroller.cpp
)

# Исполняемый файл
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME}
  MANUAL_FINALIZATION
  ${PROJECT_SOURCES}
  )
# Define target properties for Android with Qt 6 as:
# set_property(TARGET CmakeWidgetsTest APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
  add_library(${PROJECT_NAME} SHARED
  ${PROJECT_SOURCES}
  )
# Define properties for Android with Qt 5 after find_package() calls as:
# set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
  add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCES}
  )
  endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  # Qt${QT_VERSION_MAJOR}::Core
  # Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::PrintSupport
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Quick
  QXlsx::QXlsx
  # qxlsx
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
  ${BUNDLE_ID_OPTION}
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(CmakeWidgetsTest)
endif()