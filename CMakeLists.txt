#cmake_minimum_required(VERSION 3.0)
cmake_minimum_required(VERSION 3.21.1)

set (QT_MIN_VERSION "5.4.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

enable_language(CXX)
enable_language(C)

find_package(Qt6 COMPONENTS Core Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets)
endif()


if (Qt6_FOUND)
message("+ Qt6 found")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
endif()


qt_standard_project_setup()


set(PROJECT "eko")
project ($PROJECT VERSION 7.0.1 LANGUAGES CXX C)
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-DQ_OS_LINUX)
   add_definitions(-DQ_OS_UNIX)
endif()


message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")


qt_add_resources(QT_RESOURCES eko.qrc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QtWidgets_EXECUTABLE_COMPILE_FLAGS}")

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


if (Qt6_FOUND)
target_link_libraries(eko Qt::Core5Compat)
endif()

target_link_libraries(eko Qt::Widgets Qt::Core)


install (TARGETS eko DESTINATION bin)
install (FILES ${eko_ICONPNG64} DESTINATION share/icons/hicolor/64x64/apps)
install (FILES ${eko_DESKTOP} DESTINATION share/applications)

