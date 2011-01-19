cmake_minimum_required(VERSION 2.6)

project(HammerJS)

set(CMAKE_BUILD_TYPE "Release")

if(CMAKE_COMPILER_IS_GNUCXX )
   add_definitions(-Wall)
endif(CMAKE_COMPILER_IS_GNUCXX )

set(HammerJS_HEADERS
    parser/JSParser.h
    parser/Lexer.h
    parser/Lookup.h
    parser/ParserArena.h
    parser/Parser.h
    parser/SyntaxTree.h
    parser/SourceCode.h
    parser/SourceProvider.h
    parser/TreeDumper.h
    runtime/Identifier.h
    runtime/JSGlobalData.h
    runtime/JSGlobalObjectFunctions.h
    runtime/UString.h
    wtf/ASCIICType.h
    wtf/AlwaysInline.h
    wtf/Assertions.h
    wtf/Noncopyable.h
    wtf/Platform.h
    wtf/SegmentedVector.h
    wtf/TypeTraits.h
    wtf/Vector.h
    wtf/VectorTraits.h
    wtf/dtoa.h
    config.h
)

set(HammerJS_SOURCES
    hammerjs.cpp
    parser/JSParser.cpp
    parser/Lexer.cpp
    parser/ParserArena.cpp
    parser/Parser.cpp
    parser/TreeDumper.cpp
    runtime/JSGlobalObjectFunctions.cpp
    wtf/dtoa.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/parser
    ${PROJECT_SOURCE_DIR}/runtime
    ${PROJECT_SOURCE_DIR}/wtf
    ${PROJECT_SOURCE_DIR}/lib/include
)

add_executable(hammerjs ${HammerJS_SOURCES})

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
)

add_library(v8 STATIC IMPORTED)
set_property(TARGET v8 PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/libv8.a)

target_link_libraries(hammerjs v8 pthread)

