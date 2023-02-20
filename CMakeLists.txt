cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)

project(scpp-imgui CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME scpp-imgui)

################################################################################
# Source groups
################################################################################
set(Archivos_de_encabezado
    "AppState.h"
    "resource.h"
    "Router.h"
    "src/Config.h"
    "src/login/Login.h"
)
source_group("Archivos de encabezado" FILES ${Archivos_de_encabezado})

set(Archivos_de_encabezado__curl
    "include/curl/curl.h"
    "include/curl/curlver.h"
    "include/curl/easy.h"
    "include/curl/header.h"
    "include/curl/mprintf.h"
    "include/curl/multi.h"
    "include/curl/options.h"
    "include/curl/stdcheaders.h"
    "include/curl/system.h"
    "include/curl/typecheck-gcc.h"
    "include/curl/urlapi.h"
    "include/curl/websockets.h"
)
source_group("Archivos de encabezado\\curl" FILES ${Archivos_de_encabezado__curl})

set(Archivos_de_encabezado__curlcpp
    "include/curlcpp/curl_config.h"
    "include/curlcpp/curl_easy.h"
    "include/curlcpp/curl_easy_info.h"
    "include/curlcpp/curl_exception.h"
    "include/curlcpp/curl_form.h"
    "include/curlcpp/curl_interface.h"
    "include/curlcpp/curl_ios.h"
    "include/curlcpp/curl_pair.h"
)
source_group("Archivos de encabezado\\curlcpp" FILES ${Archivos_de_encabezado__curlcpp})

set(Archivos_de_encabezado__documentos
    "src/documentos/CategoriaPicker.h"
    "src/documentos/Documentos.h"
    "src/documentos/SingleDoc.h"
    "src/documentos/TipoDocPicker.h"
)
source_group("Archivos de encabezado\\documentos" FILES ${Archivos_de_encabezado__documentos})

set(Archivos_de_encabezado__graph
    "src/graph/CategoryGraph.h"
    "src/graph/TimeGraph.h"
)
source_group("Archivos de encabezado\\graph" FILES ${Archivos_de_encabezado__graph})

set(Archivos_de_encabezado__helpers
    "src/helpers/ApiHelper.h"
    "src/helpers/ApiHelperC.h"
    "src/helpers/FileLogger.h"
    "src/helpers/FormatNumber.h"
    "src/helpers/IconsMaterialDesign.h"
    "src/helpers/imguidatechooser.h"
    "src/helpers/TimerC.h"
    "src/helpers/Utilities.h"
)
source_group("Archivos de encabezado\\helpers" FILES ${Archivos_de_encabezado__helpers})

set(Archivos_de_encabezado__imgui
    "include/GLFW/glfw3.h"
    "include/imgui/imconfig.h"
    "include/imgui/imgui.h"
    "include/imgui/imgui_impl_glfw.h"
    "include/imgui/imgui_impl_vulkan.h"
    "include/imgui/imgui_impl_win32.h"
    "include/imgui/imgui_internal.h"
    "include/imgui/imgui_stdlib.h"
    "include/imgui/imstb_rectpack.h"
    "include/imgui/imstb_textedit.h"
    "include/imgui/imstb_truetype.h"
)
source_group("Archivos de encabezado\\imgui" FILES ${Archivos_de_encabezado__imgui})

set(Archivos_de_encabezado__implot
    "include/implot/implot.h"
    "include/implot/implot_internal.h"
)
source_group("Archivos de encabezado\\implot" FILES ${Archivos_de_encabezado__implot})

set(Archivos_de_encabezado__jsoncpp
    "include/json/allocator.h"
    "include/json/assertions.h"
    "include/json/config.h"
    "include/json/forwards.h"
    "include/json/json-forwards.h"
    "include/json/json.h"
    "include/json/json_features.h"
    "include/json/reader.h"
    "include/json/value.h"
    "include/json/version.h"
    "include/json/writer.h"
)
source_group("Archivos de encabezado\\jsoncpp" FILES ${Archivos_de_encabezado__jsoncpp})

set(Archivos_de_origen
    "App.cpp"
    "AppState.cpp"
    "Router.cpp"
    "src/Config.cpp"
    "src/jsoncpp/jsoncpp.cpp"
    "src/login/Login.cpp"
)
source_group("Archivos de origen" FILES ${Archivos_de_origen})

set(Archivos_de_origen__curlcpp
    "include/curlcpp/curl_easy.cpp"
    "include/curlcpp/curl_exception.cpp"
    "include/curlcpp/curl_form.cpp"
)
source_group("Archivos de origen\\curlcpp" FILES ${Archivos_de_origen__curlcpp})

set(Archivos_de_origen__documentos
    "src/documentos/CategoriaPicker.cpp"
    "src/documentos/Documentos.cpp"
    "src/documentos/SingleDoc.cpp"
    "src/documentos/TipoDocPicker.cpp"
)
source_group("Archivos de origen\\documentos" FILES ${Archivos_de_origen__documentos})

set(Archivos_de_origen__graph
    "src/graph/CategoryGraph.cpp"
    "src/graph/TimeGraph.cpp"
)
source_group("Archivos de origen\\graph" FILES ${Archivos_de_origen__graph})

set(Archivos_de_origen__helpers
    "src/helpers/ApiHelper.cpp"
    "src/helpers/ApiHelperC.cpp"
    "src/helpers/FileLogger.cpp"
    "src/helpers/FormatNumber.cpp"
    "src/helpers/imguidatechooser.cpp"
    "src/helpers/TimerC.cpp"
    "src/helpers/Utilities.cpp"
)
source_group("Archivos de origen\\helpers" FILES ${Archivos_de_origen__helpers})

set(Archivos_de_origen__imgui
    "include/imgui/imgui.cpp"
    "include/imgui/imgui_demo.cpp"
    "include/imgui/imgui_draw.cpp"
    "include/imgui/imgui_impl_glfw.cpp"
    "include/imgui/imgui_impl_vulkan.cpp"
    "include/imgui/imgui_impl_win32.cpp"
    "include/imgui/imgui_stdlib.cpp"
    "include/imgui/imgui_tables.cpp"
    "include/imgui/imgui_widgets.cpp"
)
source_group("Archivos de origen\\imgui" FILES ${Archivos_de_origen__imgui})

set(Archivos_de_origen__implot
    "include/implot/implot.cpp"
    "include/implot/implot_demo.cpp"
    "include/implot/implot_items.cpp"
)
source_group("Archivos de origen\\implot" FILES ${Archivos_de_origen__implot})

set(ALL_FILES
    ${Archivos_de_encabezado}
    ${Archivos_de_encabezado__curl}
    ${Archivos_de_encabezado__curlcpp}
    ${Archivos_de_encabezado__documentos}
    ${Archivos_de_encabezado__graph}
    ${Archivos_de_encabezado__helpers}
    ${Archivos_de_encabezado__imgui}
    ${Archivos_de_encabezado__implot}
    ${Archivos_de_encabezado__jsoncpp}
    ${Archivos_de_origen}
    ${Archivos_de_origen__curlcpp}
    ${Archivos_de_origen__documentos}
    ${Archivos_de_origen__graph}
    ${Archivos_de_origen__helpers}
    ${Archivos_de_origen__imgui}
    ${Archivos_de_origen__implot}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE scppimgui)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        ">"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CRT_SECURE_NO_WARNINGS;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /SUBSYSTEM:CONSOLE
            >
            $<$<CONFIG:Release>:
                /SUBSYSTEM:WINDOWS;
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$(SolutionDir)/lib/libcurl;"
        "$(SolutionDir)/lib/glfw/glfw3;"
        "$(SolutionDir)lib/vulkan/vulkan-1"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}//lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "C:/Users/Chemy/Documents/GitHub/SCPP-ImGui/lib"
        ">"
    )
endif()

