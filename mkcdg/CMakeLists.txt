cmake_minimum_required(VERSION 3.6)
project(mkcdg CXX)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(FindFreetype)
include(FindFontconfig)

find_package(fmt REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(Freetype REQUIRED)
find_package(Harfbuzz REQUIRED)

include_directories("include/")
add_executable(mkcdg "src/main.cpp"
        "src/background.cpp"
        "src/cdggenerator.cpp"
        "src/image.cpp"
        "src/mytextrenderer.cpp"
        "src/xmllyrics.cpp"
        "src/tinyxml2.cpp"
        src/line.cpp src/word.cpp src/track.cpp include/track.h src/commonattributes.cpp include/commonattributes.h src/syllable.cpp include/syllable.h src/block.cpp include/block.h)

target_link_libraries(mkcdg ${FONTCONFIG_LIBRARIES} ${FREETYPE_LIBRARIES} ${FMT_LIBRARIES} ${HARFBUZZ_LIBRARIES} harfbuzz::harfbuzz fmt::fmt)
target_include_directories(mkcdg PUBLIC ${FONTCONFIG_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS})
