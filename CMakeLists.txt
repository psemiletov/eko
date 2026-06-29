# cmake_minimum_required(VERSION 3.0)
cmake_minimum_required(VERSION 3.21.1)

# project() ДОЛЖЕН быть вызван ДО enable_language()
project(eko VERSION 7.1.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# =============================================
# ПОИСК QT6
# =============================================

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

   # =============================================
   # ИСПРАВЛЕНИЯ ДЛЯ WINDOWS
   # =============================================
   add_compile_definitions(_HAS_STD_BYTE=0)
   add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
   add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
   add_compile_definitions(_ENABLE_EXTENDED_ALIGNED_STORAGE)
   add_compile_definitions(_HAS_FEATURES_ANALYSIS=0)

   # Флаги компилятора ТОЛЬКО для MSVC
   if (MSVC)
       add_compile_options(/std:c++17)
       add_compile_options(/Zc:__cplusplus)
       add_compile_options(/Zc:preprocessor-)
       add_compile_options(/permissive-)
   endif()
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

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux: используем pkg-config
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LIBSNDFILE REQUIRED sndfile)
    if (LIBSNDFILE_FOUND)
        target_link_libraries(eko PRIVATE ${LIBSNDFILE_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBSNDFILE_INCLUDE_DIRS})
        message("+ sndfile (pkg-config)")
    else()
        message(FATAL_ERROR "libsndfile not found! Install: sudo apt install libsndfile-dev")
    endif()

    pkg_check_modules(LIBSAMPLERATE REQUIRED samplerate)
    if (LIBSAMPLERATE_FOUND)
        target_link_libraries(eko PRIVATE ${LIBSAMPLERATE_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBSAMPLERATE_INCLUDE_DIRS})
        message("+ samplerate (pkg-config)")
    else()
        message(FATAL_ERROR "libsamplerate not found! Install: sudo apt install libsamplerate-dev")
    endif()

    pkg_check_modules(LIBPORTAUDIO REQUIRED portaudio-2.0)
    if (LIBPORTAUDIO_FOUND)
        target_link_libraries(eko PRIVATE ${LIBPORTAUDIO_LIBRARIES})
        target_include_directories(eko PRIVATE ${LIBPORTAUDIO_INCLUDE_DIRS})
        message("+ portaudio (pkg-config)")
    else()
        message(FATAL_ERROR "portaudio not found! Install: sudo apt install portaudio19-dev")
    endif()

else()
    # Windows: используем find_package CONFIG (через Vcpkg)

    # 1. libsndfile
    find_package(SndFile CONFIG REQUIRED)
    if (SndFile_FOUND)
        target_link_libraries(eko PRIVATE SndFile::sndfile)
        target_include_directories(eko PRIVATE ${SndFile_INCLUDE_DIRS})
        message("+ sndfile (find_package CONFIG)")
    else()
        message(FATAL_ERROR "libsndfile not found! Install via vcpkg: vcpkg install libsndfile:x64-windows")
    endif()

    # 2. libsamplerate
    find_package(SampleRate CONFIG REQUIRED)
    if (SampleRate_FOUND)
        target_link_libraries(eko PRIVATE SampleRate::samplerate)
        target_include_directories(eko PRIVATE ${SampleRate_INCLUDE_DIRS})
        message("+ samplerate (find_package CONFIG)")
    else()
        # Пробуем альтернативное имя
        find_package(samplerate CONFIG QUIET)
        if (samplerate_FOUND)
            target_link_libraries(eko PRIVATE samplerate)
            target_include_directories(eko PRIVATE ${samplerate_INCLUDE_DIRS})
            message("+ samplerate (find_package CONFIG - samplerate)")
        else()
            message(FATAL_ERROR "libsamplerate not found! Install via vcpkg: vcpkg install libsamplerate:x64-windows")
        endif()
    endif()

    # 3. PortAudio
    find_package(portaudio CONFIG REQUIRED)
    if (portaudio_FOUND)
        target_link_libraries(eko PRIVATE portaudio)
        target_include_directories(eko PRIVATE ${portaudio_INCLUDE_DIRS})
        message("+ portaudio (find_package CONFIG)")
    else()
        message(FATAL_ERROR "portaudio not found! Install via vcpkg: vcpkg install portaudio:x64-windows")
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
# ЛИНКОВКА QT6
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
