cmake_minimum_required(VERSION 3.10)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

project(game)

FIND_PACKAGE(Bullet REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
link_directories(${Boost_LIBRARY_DIR})

# specify the C++ standard
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_FLAGS "-w -g -lGL -lGLEW -lassimp -lglfw -ldl -lpthread -lm -lboost_serialization")

FILE(GLOB MAIN src/core/main.cpp)
FILE(GLOB CORE src/core/*.cpp src/render/*.cpp src/util/*.cpp)
FILE(GLOB GAME srcgame/*.cpp)
FILE(GLOB LOGURU vendor/loguru/loguru.cpp)
FILE(GLOB STB vendor/stb_image/stb_image.cpp)
FILE(GLOB IMGUI vendor/imgui/*.cpp)
FILE(GLOB IMGUIFILE vendor/ImFileBrowser/*.cpp)
FILE(GLOB CSG src/core/csg/*.cpp)
FILE(GLOB IMGUIZMO vendor/ImGuizmo/*.cpp)
FILE(GLOB GLAD deps/glad/*.c)
FILE(GLOB UTIL src/core/util/*.cpp)

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("src/*")
INCLUDE_DIRECTORIES("src/render")
INCLUDE_DIRECTORIES("src/util")
INCLUDE_DIRECTORIES("src/editor")
INCLUDE_DIRECTORIES("src/csg")

INCLUDE_DIRECTORIES("src/core/")
INCLUDE_DIRECTORIES("deps")
INCLUDE_DIRECTORIES("renderer")
INCLUDE_DIRECTORIES("vendor/")
INCLUDE_DIRECTORIES("vendor/imgui")
INCLUDE_DIRECTORIES("src")
INCLUDE_DIRECTORIES("vendor/stb_image")
INCLUDE_DIRECTORIES("vendor/loguru")
INCLUDE_DIRECTORIES("vendor/ImFileBrowser")
INCLUDE_DIRECTORIES("vendor/ImGuizmo")
INCLUDE_DIRECTORIES("assets/*/*")
INCLUDE_DIRECTORIES("shaders/")
INCLUDE_DIRECTORIES("vendor/FastNoiseLite")
INCLUDE_DIRECTORIES("/usr/include/bullet")
INCLUDE_DIRECTORIES("/usr/include")
INCLUDE_DIRECTORIES("/usr/lib")

add_executable(game src/core/main.cpp)

add_library(GameEngine
		${COMP}
		${CORE}
		${RENDER}
		${GAMEH}
		${GAME}
		${LOGURU}
		${STB}
    ${IMGUI}
    ${IMGUIZMO}
    ${IMGUIFILE}
		${UTIL}
		${CSG}
 		)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)
target_link_libraries(game GameEngine)
target_link_libraries(game LINK_PUBLIC ${BOOST_LIBRARIES})

INCLUDE_DIRECTORIES(${BULLET_INCLUDE_DIRECTORIES})
target_link_libraries(game ${BULLET_LIBRARIES})
