# cmake_minimum_required(VERSION 3.0)
cmake_minimum_required(VERSION 3.21.1)

# project() ДОЛЖЕН быть вызван ДО enable_language()
project(eko VERSION 7.1.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Находим Qt6 (только Core и Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

if (Qt6_FOUND)
    message("+ Qt6 found")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(PROJECT "eko")
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-DQ_OS_LINUX)
   add_definitions(-DQ_OS_UNIX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   add_definitions(-DQ_OS_WIN32)
endif()

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

qt_add_resources(QT_RESOURCES eko.qrc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QtWidgets_EXECUTABLE_COMPILE_FLAGS}")

file(GLOB eko_SRCS "*.c" "*.cpp")
file(GLOB eko_HEADERS "*.h")

add_executable(eko ${eko_SRCS} ${QT_RESOURCES})

# =============================================
# ПОИСК ЗАВИСИМОСТЕЙ
# =============================================

# 1. libsndfile
find_package(SndFile CONFIG QUIET)
if (SndFile_FOUND)
    target_link_libraries(eko PRIVATE SndFile::sndfile)
    target_include_directories(eko PRIVATE ${SndFile_INCLUDE_DIRS})
    message("+ sndfile (find_package CONFIG)")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBSNDFILE REQUIRED sndfile)
    if (LIBSNDFILE_FOUND)
        target_link_libraries(eko PRIVATE ${LIBSNDFILE_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBSNDFILE_INCLUDE_DIRS})
        message("+ sndfile (pkg-config fallback)")
    else()
        message(FATAL_ERROR "libsndfile not found!")
    endif()
endif()

# 2. libsamplerate
find_package(Samplerate CONFIG QUIET)
if (Samplerate_FOUND)
    target_link_libraries(eko PRIVATE Samplerate::samplerate)
    target_include_directories(eko PRIVATE ${Samplerate_INCLUDE_DIRS})
    message("+ samplerate (find_package CONFIG)")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBSAMPLERATE REQUIRED samplerate)
    if (LIBSAMPLERATE_FOUND)
        target_link_libraries(eko PRIVATE ${LIBSAMPLERATE_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBSAMPLERATE_INCLUDE_DIRS})
        message("+ samplerate (pkg-config fallback)")
    else()
        message(FATAL_ERROR "libsamplerate not found!")
    endif()
endif()

# 3. PortAudio
find_package(PortAudio CONFIG QUIET)
if (PortAudio_FOUND)
    target_link_libraries(eko PRIVATE PortAudio::portaudio)
    target_include_directories(eko PRIVATE ${PortAudio_INCLUDE_DIRS})
    message("+ portaudio (find_package CONFIG)")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBPORTAUDIO REQUIRED portaudio-2.0)
    if (LIBPORTAUDIO_FOUND)
        target_link_libraries(eko PRIVATE ${LIBPORTAUDIO_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBPORTAUDIO_INCLUDE_DIRS})
        message("+ portaudio (pkg-config fallback)")
    else()
        message(FATAL_ERROR "portaudio not found!")
    endif()
endif()

# =============================================
# ДОПОЛНИТЕЛЬНЫЕ НАСТРОЙКИ
# =============================================

set(eko_ICONPNG64
    ./icons/eko.png
)

set(eko_DESKTOP
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

# =============================================
# ЛИНКОВКА QT
# =============================================

target_link_libraries(eko PRIVATE Qt::Widgets Qt::Core)

# =============================================
# УСТАНОВКА
# =============================================

install (TARGETS eko DESTINATION bin)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install (FILES ${eko_ICONPNG64} DESTINATION share/icons/hicolor/64x64/apps)
    install (FILES ${eko_DESKTOP} DESTINATION share/applications)
endif()
