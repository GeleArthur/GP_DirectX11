# Source files
set(SOURCES
        "src/ColorRGB.h"
        "src/ColorRGB.cpp"
        "src/main.cpp"
        "src/Matrix.h"
        "src/pch.cpp"
        "src/pch.h"
        "src/RendererCombined.cpp"
        "src/RendererCombined.h"
        "src/Timer.h"
        "src/Timer.cpp"
        "src/Utils.h"
        "src/Utils.cpp"
        "src/Vector.h"
        "src/SoftwareRendererHelpers.cpp"
        "src/SoftwareRendererHelpers.h"
        "src/PhongMesh.h"
        "src/PhongMesh.cpp"
        "src/FireFX.h"
        "src/FireFX.cpp"
        "src/Camera.h"
        "src/Camera.cpp"
        "src/Texture.h"
        "src/Texture.cpp"
        "src/TextureManager.h"
        "src/TextureManager.cpp"
        "src/BaseMeshEffect.h"
        "src/UnlitMesh.h"
        "src/UnlitMesh.cpp"
        "src/BaseMeshEffect.cpp"
        "src/Scene.cpp"
        "src/Scene.h"
        "src/DirectXUtils.h"
        "src/CullMode.h"

        "src/magic_enum.hpp"
        "src/tiny_obj_loader.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib dxgi.lib)

file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
add_custom_target(Resources ALL SOURCES ${RESOURCE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

# SDL IMPORT

set(SDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.30.7")
add_library(SDL STATIC IMPORTED)
set_target_properties(SDL PROPERTIES
        IMPORTED_LOCATION "${SDL_DIR}/lib/x64/SDL2.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL)

file(GLOB_RECURSE DLL_FILES
        "${SDL_DIR}/lib/x64/*.dll"
        "${SDL_DIR}/lib/x64/*.manifest"
)

foreach (DLL ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach (DLL)

set(SDL_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image-2.8.2")
add_library(SDL_IMAGE STATIC IMPORTED)
set_target_properties(SDL_IMAGE PROPERTIES
        IMPORTED_LOCATION "${SDL_IMAGE_DIR}/lib/x64/SDL2_image.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL_IMAGE_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL_IMAGE)

file(GLOB_RECURSE DLL_FILES
        "${SDL_IMAGE_DIR}/lib/x64/*.dll"
        "${SDL_IMAGE_DIR}/lib/x64/*.manifest"
)

foreach (DLL ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach (DLL)




# DirectX Effects
set(FX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/dx11effects")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FX_LIBRARY "${FX_DIR}/lib/x64/dx11effects_d.lib")
else ()
    set(FX_LIBRARY "${FX_DIR}/lib/x64/dx11effects.lib")
endif ()
add_library(FX STATIC IMPORTED)
set_target_properties(FX PROPERTIES
        IMPORTED_LOCATION "${FX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FX_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE FX)

# Visual Leak Detector
if (WIN32 AND CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(ENABLE_VLD=1)
    set(VLD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/vld")
    add_library(vld STATIC IMPORTED)
    set_target_properties(vld PROPERTIES
            IMPORTED_LOCATION "${VLD_DIR}/lib/x64/vld.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${VLD_DIR}/include"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE vld)

    set(DLL_SOURCE_DIR "${VLD_DIR}/lib")

    file(GLOB_RECURSE DLL_FILES
            "${DLL_SOURCE_DIR}/*.dll"
            "${DLL_SOURCE_DIR}/*.manifest"
    )

    foreach (DLL ${DLL_FILES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach (DLL)
endif ()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
