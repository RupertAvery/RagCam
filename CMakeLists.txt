cmake_minimum_required(VERSION 3.15)
project(RagCam)

set(SOURCES
    3DMath.cpp
    Camera.cpp
    Frustum.cpp
    GND.cpp
    grf.c
    ro-common.cpp
    Rsm-common.cpp
    Rsm.cpp
    RSM_Mesh.cpp
    rsmview.cpp
    RSW.cpp
    tgaload.cpp
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/inffast.c
    zlib/inflate.c
    zlib/inftrees.c
    zlib/trees.c
    zlib/zutil.c
)

add_executable(RagCam WIN32 ${SOURCES})

target_compile_definitions(RagCam PRIVATE
    WIN32
    _WINDOWS
    _MBCS
    GRF_STATIC
    $<$<CONFIG:Release>:NDEBUG;_USEGRF>
    $<$<CONFIG:Debug>:_DEBUG;>
)

target_compile_options(RagCam PRIVATE
    /W3
    /EHsc
    /nologo
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Debug>:/Gm /Od /ZI /GZ>
)

target_link_libraries(RagCam PRIVATE
    opengl32
    glu32
kernel32
    user32
    gdi32
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    $<$<CONFIG:Debug>:uuid>
)

set_target_properties(RagCam PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
    OUTPUT_NAME_RELEASE "ragcam"
    OUTPUT_NAME_DEBUG "RagCamDB"
)
