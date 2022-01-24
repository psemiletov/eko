cmake_minimum_required(VERSION 3.0)
set (QT_MIN_VERSION "5.4.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

enable_language(CXX)
enable_language(C)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)


if(Qt${QT_VERSION_MAJOR} STREQUAL "Qt6")
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Core5Compat REQUIRED)
endif()


if(Qt${QT_VERSION_MAJOR} STREQUAL "Qt5")
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
endif()


message(Qt${QT_VERSION_MAJOR} " found")


include_directories(${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS})
add_definitions(${Qt${QT_VERSION_MAJOR}Widgets_DEFINITIONS})
add_definitions(${Qt${QT_VERSION_MAJOR}Widgets_COMPILE_DEFINITIONS})


set(PROJECT "eko")
project ($PROJECT VERSION 7.0.0 LANGUAGES CXX C)
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-DQ_OS_LINUX)
   add_definitions(-DQ_OS_UNIX)
endif()


message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED Core Widgets)

if(BUILD_TESTING)
  find_package(Qt${Test}${QT_VERSION_MAJOR} CONFIG REQUIRED)
endif()


if(Qt${QT_VERSION_MAJOR} STREQUAL "Qt6")
qt6_add_resources(QT_RESOURCES eko.qrc)
endif()

if (Qt${QT_VERSION_MAJOR} MATCHES "Qt5")
qt5_add_resources(QT_RESOURCES eko.qrc)
endif()



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt${QT_VERSION_MAJOR}Widgets_EXECUTABLE_COMPILE_FLAGS}")

file(GLOB eko_SRCS "*.c" "*.cpp")
file(GLOB eko_HEADERS "*.h")

add_executable(eko ${eko_SRCS} ${QT_RESOURCES})


find_package(PkgConfig)

pkg_check_modules(sndfile REQUIRED sndfile)
if(sndfile_FOUND)
  target_link_libraries(eko ${sndfile_LIBRARIES})
#  target_link_libraries(eko lsndfile)
  include_directories(${sndfile_INCLUDE_DIRS})
  message("+ sndfile")
endif()


pkg_check_modules(samplerate REQUIRED samplerate)
if(samplerate_FOUND)
  target_link_libraries(eko ${samplerate_LIBRARIES})
  include_directories(${samplerate_INCLUDE_DIRS})
  message("+ samplerate")
endif()


pkg_check_modules(portaudio2 REQUIRED portaudio-2.0)
if(portaudio2_FOUND)
  target_link_libraries(eko ${portaudio2_LIBRARIES})
  include_directories(${portaudio2_INCLUDE_DIRS})
  message("+ portaudio")
endif()


set(eko_ICONPNG64
    ./icons/eko.png
)


set(tea_DESKTOP
    ./desktop/eko.desktop
)


add_custom_target(dist
COMMAND git archive --format=tar --prefix=${PROJECT}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}/ HEAD | gzip >${PROJECT}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.tar.gz
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)



if (EXISTS "/usr/include/linux/joystick.h")
   message("+JOYSTICK_SUPPORTED")
   add_definitions(-DJOYSTICK_SUPPORTED)
endif()

#endif()
#endif()


if(Qt${QT_VERSION_MAJOR} STREQUAL "Qt6")
   target_link_libraries(eko Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt6::Core5Compat)
endif()


if(Qt${QT_VERSION_MAJOR} STREQUAL "Qt5")
target_link_libraries(eko Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core)
endif()


install (TARGETS eko DESTINATION bin)
install (FILES ${eko_ICONPNG64} DESTINATION share/icons/hicolor/64x64/apps)
install (FILES ${eko_DESKTOP} DESTINATION share/applications)

