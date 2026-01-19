cmake_minimum_required(VERSION 3.8)
project("r3d")

# -----------------------------
# MSVC用の「Edit and Continue」設定
# -----------------------------
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# プロジェクトのルートディレクトリ
set(R3D_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(R3D_IS_MAIN ON)
else()
    set(R3D_IS_MAIN OFF)
endif()

# -----------------------------
# プロジェクト設定オプション
# -----------------------------
option(R3D_BUILD_EXAMPLES "Build the examples" ${R3D_IS_MAIN})
option(R3D_RAYLIB_VENDORED "Use vendored raylib from submodule" OFF)
option(R3D_ASSIMP_VENDORED "Use vendored assimp from submodule" OFF)

# CMake モジュールパス
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# スクリプト読み込み
include(CheckLibraryExists)
include(EmbedShaders)
include(EmbedAssets)

# -----------------------------
# Assimp 設定
# -----------------------------
if(R3D_ASSIMP_VENDORED)
    set(R3D_ASSIMP_SUBMODULE_PATH "${R3D_ROOT_PATH}/external/assimp")
    set(R3D_ASSIMP_CMAKELISTS "${R3D_ASSIMP_SUBMODULE_PATH}/CMakeLists.txt")

    if(EXISTS "${R3D_ASSIMP_CMAKELISTS}")
        message(STATUS "Using vendored assimp from: ${R3D_ASSIMP_SUBMODULE_PATH}")

        set(ASSIMP_INJECT_DEBUG_POSTFIX     OFF CACHE BOOL "")
        set(ASSIMP_WARNINGS_AS_ERRORS       OFF CACHE BOOL "")
        set(ASSIMP_IGNORE_GIT_HASH          ON CACHE BOOL "")
        set(ASSIMP_BUILD_TESTS              OFF CACHE BOOL "")
        set(ASSIMP_NO_EXPORT                ON CACHE BOOL "")
        set(ASSIMP_INSTALL                  OFF CACHE BOOL "")

        # 必要なインポーターのみ有効化
        set(ASSIMP_BUILD_COLLADA_IMPORTER   ON CACHE BOOL "")
        set(ASSIMP_BUILD_GLTF_IMPORTER      ON CACHE BOOL "")
        set(ASSIMP_BUILD_OBJ_IMPORTER       ON CACHE BOOL "")
        set(ASSIMP_BUILD_M3D_IMPORTER       ON CACHE BOOL "")
        set(ASSIMP_BUILD_FBX_IMPORTER       ON CACHE BOOL "")
        set(ASSIMP_BUILD_IQM_IMPORTER       ON CACHE BOOL "")
        # その他のフォーマットは無効
        foreach(fmt AMF 3DS AC ASE ASSBIN B3D BVH DXF CSM HMP IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF OGRE OPENGEX PLY MS3D COB BLEND IFC XGL Q3D Q3BSP RAW SIB SMD STL TERRAGEN 3D USD X X3D 3MF MMD)
            set(ASSIMP_BUILD_${fmt}_IMPORTER OFF CACHE BOOL "")
        endforeach()

        add_subdirectory("${R3D_ASSIMP_SUBMODULE_PATH}")

        set(R3D_ASSIMP_INC_PATH "${R3D_ASSIMP_SUBMODULE_PATH}/src" CACHE STRING "Path to assimp includes" FORCE)

        # DLL名を変更
        set_target_properties(assimp PROPERTIES
            OUTPUT_NAME "assimp"
            PREFIX ""
            SUFFIX ".dll"
        )
    else()
        message(FATAL_ERROR "Vendored assimp not found! Missing: ${R3D_ASSIMP_CMAKELISTS}")
    endif()
else()
    find_package(assimp QUIET)
    if(TARGET assimp)
        message(STATUS "Using system-installed assimp")
        get_target_property(RAYLIB_INCLUDE_DIR assimp INTERFACE_INCLUDE_DIRECTORIES)
        set(R3D_ASSIMP_INC_PATH "${RAYLIB_INCLUDE_DIR}" CACHE STRING "Path to assimp includes" FORCE)
    else()
        message(FATAL_ERROR "System assimp not found.")
    endif()
endif()

# -----------------------------
# Raylib 設定
# -----------------------------
if(R3D_RAYLIB_VENDORED)
    set(R3D_RAYLIB_SUBMODULE_PATH "${R3D_ROOT_PATH}/external/raylib")
    set(R3D_RAYLIB_CMAKELISTS "${R3D_RAYLIB_SUBMODULE_PATH}/CMakeLists.txt")

    if(EXISTS "${R3D_RAYLIB_CMAKELISTS}")
        message(STATUS "Using vendored raylib from: ${R3D_RAYLIB_SUBMODULE_PATH}")

        # 不要なモジュールをOFF
        set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_OBJ OFF CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_MTL OFF CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_IQM OFF CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_GLTF OFF CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_VOX OFF CACHE BOOL "" FORCE)
        set(SUPPORT_FILEFORMAT_M3D OFF CACHE BOOL "" FORCE)
        set(SUPPORT_MESH_GENERATION OFF CACHE BOOL "" FORCE)

        add_subdirectory("${R3D_RAYLIB_SUBMODULE_PATH}")
        set(R3D_RAYLIB_INC_PATH "${R3D_RAYLIB_SUBMODULE_PATH}/src" CACHE STRING "Path to raylib includes" FORCE)

        # DLL名を変更
        set_target_properties(raylib PROPERTIES
            OUTPUT_NAME "raylib"
            PREFIX ""
            SUFFIX ".dll"
        )
    else()
        message(FATAL_ERROR "Vendored raylib not found! Missing: ${R3D_RAYLIB_CMAKELISTS}")
    endif()
else()
    find_package(raylib QUIET)
    if(TARGET raylib)
        message(STATUS "Using system-installed raylib")
        get_target_property(RAYLIB_INCLUDE_DIR raylib INTERFACE_INCLUDE_DIRECTORIES)
        set(R3D_RAYLIB_INC_PATH "${RAYLIB_INCLUDE_DIR}" CACHE STRING "Path to raylib includes" FORCE)
    else()
        message(FATAL_ERROR "System raylib not found.")
    endif()
endif()

# -----------------------------
# r3d ライブラリ作成
# -----------------------------
if(BUILD_SHARED_LIBS)
    add_definitions(-DR3D_BUILD_SHARED)
    add_definitions(-DGLAD_API_CALL_EXPORT)
endif()

add_library(${PROJECT_NAME}
    "${R3D_ROOT_PATH}/src/details/r3d_primitives.c"
    "${R3D_ROOT_PATH}/src/details/r3d_outline.c"
    "${R3D_ROOT_PATH}/src/details/r3d_billboard.c"

    "${R3D_ROOT_PATH}/src/details/r3d_drawcall.c"
    "${R3D_ROOT_PATH}/src/details/r3d_frustum.c"
    "${R3D_ROOT_PATH}/src/details/r3d_light.c"
    "${R3D_ROOT_PATH}/src/r3d_environment.c"
    "${R3D_ROOT_PATH}/src/r3d_particles.c"
    "${R3D_ROOT_PATH}/src/r3d_lighting.c"
    "${R3D_ROOT_PATH}/src/r3d_culling.c"
    "${R3D_ROOT_PATH}/src/r3d_skybox.c"
    "${R3D_ROOT_PATH}/src/r3d_curves.c"
    "${R3D_ROOT_PATH}/src/r3d_sprite.c"
    "${R3D_ROOT_PATH}/src/r3d_model.c"
    "${R3D_ROOT_PATH}/src/r3d_utils.c"
    "${R3D_ROOT_PATH}/src/r3d_state.c"
    "${R3D_ROOT_PATH}/src/r3d_core.c"
)

# DLL名を変更
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "r3d"
    PREFIX ""
    SUFFIX ".dll"
)

# -----------------------------
# シェーダー・アセット埋め込み
# -----------------------------
EmbedShaders(${PROJECT_NAME}
    "${R3D_ROOT_PATH}/shaders/common/screen.vert"
    "${R3D_ROOT_PATH}/shaders/common/cubemap.vert"
    "${R3D_ROOT_PATH}/shaders/generate/gaussian_blur_dual_pass.frag"
    "${R3D_ROOT_PATH}/shaders/generate/downsampling.frag"
    "${R3D_ROOT_PATH}/shaders/generate/upsampling.frag"
    "${R3D_ROOT_PATH}/shaders/generate/cubemap_from_equirectangular.frag"
    "${R3D_ROOT_PATH}/shaders/generate/irradiance_convolution.frag"
    "${R3D_ROOT_PATH}/shaders/generate/prefilter.frag"
    "${R3D_ROOT_PATH}/shaders/raster/geometry.vert"
    "${R3D_ROOT_PATH}/shaders/raster/geometry_instanced.vert"
    "${R3D_ROOT_PATH}/shaders/raster/geometry.frag"
    "${R3D_ROOT_PATH}/shaders/raster/forward.vert"
    "${R3D_ROOT_PATH}/shaders/raster/forward_instanced.vert"
    "${R3D_ROOT_PATH}/shaders/raster/forward.frag"
    "${R3D_ROOT_PATH}/shaders/raster/skybox.vert"
    "${R3D_ROOT_PATH}/shaders/raster/skybox.frag"
    "${R3D_ROOT_PATH}/shaders/raster/depth_volume.vert"
    "${R3D_ROOT_PATH}/shaders/raster/depth_volume.frag"
    "${R3D_ROOT_PATH}/shaders/raster/depth.vert"
    "${R3D_ROOT_PATH}/shaders/raster/depth_instanced.vert"
    "${R3D_ROOT_PATH}/shaders/raster/depth.frag"
    "${R3D_ROOT_PATH}/shaders/raster/outline.vert"
    "${R3D_ROOT_PATH}/shaders/raster/outline_instanced.vert"
    "${R3D_ROOT_PATH}/shaders/raster/outline.frag"
    "${R3D_ROOT_PATH}/shaders/raster/depth_cube.vert"
    "${R3D_ROOT_PATH}/shaders/raster/depth_cube_instanced.vert"
    "${R3D_ROOT_PATH}/shaders/raster/depth_cube.frag"
    "${R3D_ROOT_PATH}/shaders/screen/ssao.frag"
    "${R3D_ROOT_PATH}/shaders/screen/ambient.frag"
    "${R3D_ROOT_PATH}/shaders/screen/lighting.frag"
    "${R3D_ROOT_PATH}/shaders/screen/scene.frag"
    "${R3D_ROOT_PATH}/shaders/screen/bloom.frag"
    "${R3D_ROOT_PATH}/shaders/screen/fog.frag"
    "${R3D_ROOT_PATH}/shaders/screen/output.frag"
    "${R3D_ROOT_PATH}/shaders/screen/fxaa.frag"
)

EmbedAssets(${PROJECT_NAME}
    "${R3D_ROOT_PATH}/assets/lut/ibl_brdf_256.dds"
    "${R3D_ROOT_PATH}/assets/noise/blue_noise_128.png"
)

# -----------------------------
# ライブラリリンク
# -----------------------------
target_link_libraries(${PROJECT_NAME} PUBLIC raylib assimp)

check_library_exists(m cos "" HAVE_LIB_M)
if(HAVE_LIB_M)
    target_link_libraries(${PROJECT_NAME} PUBLIC m)
endif()

# -----------------------------
# インクルードディレクトリ
# -----------------------------
target_include_directories(${PROJECT_NAME} PUBLIC
    "${R3D_ROOT_PATH}/external/glad"
    "${R3D_ROOT_PATH}/include"
    "${R3D_RAYLIB_INC_PATH}"
    "${R3D_ASSIMP_INC_PATH}"
)

# -----------------------------
# C標準を指定
# -----------------------------
if(CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET r3d PROPERTY C_STANDARD 99)
endif()

# -----------------------------
# 例（examples）をビルド
# -----------------------------
if(R3D_BUILD_EXAMPLES)
    include("${R3D_ROOT_PATH}/examples/CMakeLists.txt")
endif()
