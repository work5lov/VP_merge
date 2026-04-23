cmake_minimum_required(VERSION 3.16)
project(VP_merge VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS PrintSupport Widgets Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport Widgets Quick)

set(QXLSX_SKIP_CPACK TRUE CACHE BOOL "Skip CPack configuration for QXlsx")
add_subdirectory(QXlsx/QXlsx)

set(PROJECT_SOURCES
  main.cpp
  appcontroller.h
  appcontroller.cpp
  backend/projecttypes.h
  backend/documentrenderdata.h
  backend/documentexportresult.h
  backend/headlessdocumentexportservice.h
  backend/headlessdocumentexportservice.cpp
  backend/xmlprojectparser.h
  backend/xmlprojectparser.cpp
  backend/pegeneratorservice.h
  backend/pegeneratorservice.cpp
  backend/spgeneratorservice.h
  backend/spgeneratorservice.cpp
  backend/vpgeneratorservice.h
  backend/vpgeneratorservice.cpp
  backend/groupedvpgeneratorservice.h
  backend/groupedvpgeneratorservice.cpp
  backend/projectfacade.h
  backend/projectfacade.cpp
  Structures.h
  resources.qrc
  docpainter.cpp
  docpainter.h
  painterprinter.cpp
  painterprinter.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
else()
  if(ANDROID)
    add_library(${PROJECT_NAME} SHARED
      ${PROJECT_SOURCES}
    )
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
  Qt${QT_VERSION_MAJOR}::PrintSupport
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Quick
  QXlsx::QXlsx
)

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
  qt_finalize_executable(${PROJECT_NAME})
endif()
