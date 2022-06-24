cmake_minimum_required(VERSION 3.8)

project(RPGGame)


add_executable(${PROJECT_NAME} "main.cpp")

IF (WIN32)
    set(SDL2_PATH "G:/cpp_libs/SDL2-2.0.22" CACHE BOOL "" FORCE)
    set(SDL2_TTF_PATH "G:/cpp_libs/SDL2_ttf-2.0.18" CACHE BOOL "" FORCE)
    set(SDL2_IMAGE_PATH "G:/cpp_libs/SDL2_image-2.0.5" CACHE BOOL "" FORCE)
    set(sioclient_DIR "G:/cpp_libs/sioclient_debug" CACHE BOOL "" FORCE)
    
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/sdl2-cmake-modules)

    # Add socketio Library
    find_package(sioclient REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${SIOCLIENT_LIBRARIES})
ELSE()
    # Add socketio Library
    target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include/)
    find_library(sioclient NAMES libsioclient.a PATHS /usr/local/lib/)
    target_link_libraries(${PROJECT_NAME} sioclient)
ENDIF()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
# Find and link SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Find and link SDL2_image
find_package(SDL2_image REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})

# Find and link SDL2_ttf
find_package(SDL2_ttf REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARIES})




MESSAGE(${SDL2_INCLUDE_DIRS})
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets_to_be_copied ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(RPGGame copy_assets)
