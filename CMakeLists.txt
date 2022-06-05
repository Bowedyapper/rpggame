cmake_minimum_required(VERSION 3.8)

project(RPGGame)

set(SDL2_PATH "G:/cpp_libs/SDL2-2.0.22" CACHE BOOL "" FORCE)
set(SDL2_TTF_PATH "G:/cpp_libs/SDL2_ttf-2.0.18" CACHE BOOL "" FORCE)
set(SDL2_IMAGE_PATH "G:/cpp_libs/SDL2_image-2.0.5" CACHE BOOL "" FORCE)
set(sioclient_DIR "G:/cpp_libs/sioclient" CACHE BOOL "" FORCE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
add_executable(RPGGame main.cpp)

# Add SDL2 library
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Main)

# Add SDL2_ttf library
find_package(SDL2_ttf REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::TTF)

# Add SDL2_image library
find_package(SDL2_image REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Image)

# Add socketio Library
find_package(sioclient REQUIRED)
target_link_libraries(${PROJECT_NAME} ${SIOCLIENT_LIBRARIES})


target_include_directories(${PROJECT_NAME} PRIVATE include)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets_to_be_copied ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(RPGGame copy_assets)