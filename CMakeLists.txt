cmake_minimum_required(VERSION 3.20)

project(RenderingEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------
# Source files
# -----------------------------------------

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/*.h
)

add_executable(RenderingEngine ${SOURCES} ${HEADERS})

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RenderingEngine)

# -----------------------------------------
# Include directories
# -----------------------------------------

target_include_directories(RenderingEngine PRIVATE

${CMAKE_SOURCE_DIR}

assimp/include
dxtex/include
)

# -----------------------------------------
# Library directories
# -----------------------------------------

target_link_directories(RenderingEngine PRIVATE

assimp/bin/x64
dxtex/bin
)

# -----------------------------------------
# Link libraries
# -----------------------------------------

target_link_libraries(RenderingEngine PRIVATE

assimp-vc140-mt
DirectXTex

kernel32
user32
gdi32
winspool
comdlg32
advapi32
shell32
ole32
oleaut32
uuid
odbc32
odbccp32
)

# -----------------------------------------
# Windows subsystem
# -----------------------------------------

set_target_properties(RenderingEngine PROPERTIES
WIN32_EXECUTABLE TRUE
)

# -----------------------------------------
# Runtime library (/MT /MTd)
# -----------------------------------------

set_property(TARGET RenderingEngine PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# -----------------------------------------
# Preprocessor definitions
# -----------------------------------------

target_compile_definitions(RenderingEngine PRIVATE

$<$<CONFIG:Debug>:_DEBUG>
$<$<CONFIG:Debug>:IS_DEBUG=true>

$<$<CONFIG:Release>:NDEBUG>
$<$<CONFIG:Release>:IS_DEBUG=false>

_CONSOLE
)

# -----------------------------------------
# Copy DLLs after build
# -----------------------------------------

add_custom_command(TARGET RenderingEngine POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
${CMAKE_SOURCE_DIR}/assimp-vc140-mt.dll
${CMAKE_SOURCE_DIR}/zlib1.dll
$<TARGET_FILE_DIR:RenderingEngine>
)