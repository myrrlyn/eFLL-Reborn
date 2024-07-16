cmake_minimum_required (VERSION 3.27)

project (
    eFLL
    VERSION 2.0.0
    LANGUAGES C CXX
)

include (${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)

if (PROJECT_IS_TOP_LEVEL)
    # Make IDEs prettier
    set_property (GLOBAL PROPERTY USE_FOLDERS ON)

    # Use ccache if installed for fasting rebuilding
    # https://ccache.dev/ for more info on ccache
    find_program (CCACHE_FOUND ccache)

    if (CCACHE_FOUND)
        set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif (CCACHE_FOUND)

    # Add helper cmake scripts
    list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    include (AddCMakeTools)
    include (clang/ClangFormat)
    include (clang/ClangTidy)

    # List user/build options
    option (${PROJECT_NAME}_BUILD_DOCS "Build documentation" OFF)
    option (${PROJECT_NAME}_BUILD_EXAMPLES "Build example code" OFF)
    option (${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
    option (${PROJECT_NAME}_ENABLE_CLANG_TIDY "Enable clang-tidy on project targets"
            $ENV{${PROJECT_NAME}_ENABLE_CLANG_TIDY}
    )

    if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
        message (
            FATAL_ERROR
                "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
        )
    endif ()

    if (DEFINED ENV{CPM_SOURCE_CACHE})
        # If user has an environment defined location, use that
        set (
            CPM_SOURCE_CACHE
            $ENV{CPM_SOURCE_CACHE}
            CACHE PATH "CPM Cache Location" FORCE
        )
    elseif (DEFINED CPM_SOURCE_CACHE)
        # If user provided a custom defined location, use that
        set (
            CPM_SOURCE_CACHE
            ${CPM_SOURCE_CACHE}
            CACHE PATH "CPM Cache Location" FORCE
        )
    else ()
        # Use custom location in build folder if no user input
        set (
            CPM_SOURCE_CACHE
            ${CMAKE_SOURCE_DIR}/build/cpm_cache
            CACHE PATH "CPM Cache Location" FORCE
        )
    endif ()
endif (PROJECT_IS_TOP_LEVEL)

add_library (${PROJECT_NAME})
target_include_directories (${PROJECT_NAME} PUBLIC include)
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)

# Add third party/vendor code
add_subdirectory (deps)

# add source code
add_subdirectory (src)

if (PROJECT_IS_TOP_LEVEL)
    # Add clang-format and clang-tidy to project
    target_clangformat_setup (eFLL)

    if (eFLL_ENABLE_CLANG_TIDY)
        target_setup_clang_tidy (eFLL)
    endif ()
endif (PROJECT_IS_TOP_LEVEL)

if (${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory (examples)
endif (${PROJECT_NAME}_BUILD_EXAMPLES)

if (${PROJECT_NAME}_BUILD_TESTS)
    enable_testing ()
    add_subdirectory (tests)
endif (${PROJECT_NAME}_BUILD_TESTS)

if (${PROJECT_NAME}_BUILD_DOCS)
    include (${CMAKE_CURRENT_LIST_DIR}/cmake/AddCMakeTools.cmake)
    set (BUILD_DOCUMENTATION ON)
    # include(${cmake-scripts_SOURCE_DIR}/doxygen.cmake)

    build_docs (DOXYFILE_PATH ${CMAKE_CURRENT_LIST_DIR}/docs/Doxyfile OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/docs/build)
endif (${PROJECT_NAME}_BUILD_DOCS)
