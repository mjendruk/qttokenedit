
# 
# External dependencies
# 

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

find_package(Qt6Core 6.9)
find_package(Qt6Gui 6.9)
find_package(Qt6Widgets 6.9)
find_package(Qt6Test 6.9)

# Enable automoc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)

# 
# Executable name and options
# 

# Target name
set(target qttokenedit-test)
message(STATUS "Test ${target}")


# 
# Sources
# 

set(sources
    main.cpp
    TokenEditItemModelTests.cpp
    TokenEditShowLineEditTests.cpp
    TokenEditTest.cpp
    TokenEditTest.h
)


# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

add_test(${target} ${target})

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}::qttokenedit
    gmock-dev
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
