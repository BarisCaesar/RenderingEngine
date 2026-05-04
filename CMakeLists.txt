cmake_minimum_required(VERSION 3.20)

project(RenderingEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source Files

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/*.h
)

add_executable(RenderingEngine ${SOURCES} ${HEADERS})

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RenderingEngine)


# Include directories


target_include_directories(RenderingEngine PRIVATE

${CMAKE_SOURCE_DIR}

assimp/include
dxtex/include
include
include/Bindables
include/RenderGraph
include/RenderGraph/Passes
include/Model
include/Macros
include/Geometry
include/Drawable
shaders/include
)

# Library directories


target_link_directories(RenderingEngine PRIVATE

assimp/bin/x64
dxtex/bin
)


# Link libraries


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


# Windows subsystem


set_target_properties(RenderingEngine PROPERTIES
WIN32_EXECUTABLE TRUE
)


# Runtime library 


set_property(TARGET RenderingEngine PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)


# Preprocessor definitions


target_compile_definitions(RenderingEngine PRIVATE

$<$<CONFIG:Debug>:_DEBUG>
$<$<CONFIG:Debug>:IS_DEBUG=true>

$<$<CONFIG:Release>:NDEBUG>
$<$<CONFIG:Release>:IS_DEBUG=false>

_CONSOLE
)

# Shader Compilation

file(GLOB_RECURSE HLSL_SHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex/*.hlsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/pixel/*.hlsl"
)

foreach(FILE ${HLSL_SHADER_FILES})

    get_filename_component(FILE_NAME ${FILE} NAME_WE)

    if(FILE_NAME MATCHES "_VS$")
        set_source_files_properties(${FILE} PROPERTIES ShaderType "vs")
    elseif(FILE_NAME MATCHES "_PS$")
        set_source_files_properties(${FILE} PROPERTIES ShaderType "ps")
    endif()

    set_source_files_properties(${FILE} PROPERTIES ShaderModel "4_0")

endforeach()

foreach(FILE ${HLSL_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  list(APPEND CSO_SHADER_FILES ${CMAKE_BINARY_DIR}/${FILE_WE}.cso)
  get_source_file_property(shadertype ${FILE} ShaderType)
  get_source_file_property(shadermodel ${FILE} ShaderModel)
  add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
                     COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} /I "${CMAKE_CURRENT_SOURCE_DIR}/shaders/include" $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${CMAKE_BINARY_DIR}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
                     COMMENT "HLSL ${FILE}"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     VERBATIM)
endforeach(FILE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy ${CSO_SHADER_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>
                  COMMAND_EXPAND_LISTS)



# Copy dlls after build

add_custom_command(TARGET RenderingEngine POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
${CMAKE_SOURCE_DIR}/assimp-vc140-mt.dll
${CMAKE_SOURCE_DIR}/zlib1.dll
$<TARGET_FILE_DIR:RenderingEngine>
)