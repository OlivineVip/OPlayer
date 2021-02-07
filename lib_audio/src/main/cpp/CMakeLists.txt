cmake_minimum_required(VERSION 3.10.2)

project("audio")


#设置输出的 so 目录
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/jniLibs/${ANDROID_ABI})


#三方开源项目 path
set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/../../../../thirdparty)
#ffmpeg
set(FFMPEG_PATH ${THIRD_PARTY_ROOT}/ffmpeg)

#FFmpeg
include_directories(${FFMPEG_PATH}/include)
#LINK_DIRECTORIES(${FFMPEG_PATH}/libs/${ANDROID_ABI})
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so)
add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavdevice.so)
add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.so)
add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavformat.so)
add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavutil.so)
add_library(h264 SHARED IMPORTED)
set_target_properties(h264 PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libh264.so)
add_library(postproc SHARED IMPORTED)
set_target_properties(postproc PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libpostproc.so)
add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libswresample.so)
add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${FFMPEG_PATH}/libs/${CMAKE_ANDROID_ARCH_ABI}/libswscale.so)




# 本地代码 source path
#jni
set(NATIVE_JNI_PATH ${CMAKE_SOURCE_DIR}/jni)
set(NATIVE_SIMPLE_PATH ${CMAKE_SOURCE_DIR}/simple)
set(NATIVE_DECODE_PATH ${CMAKE_SOURCE_DIR}/decode)
set(NATIVE_PLAYER_PATH ${CMAKE_SOURCE_DIR}/player)
set(NATIVE_UTIL_PATH ${CMAKE_SOURCE_DIR}/utils)


#本地代码 头文件
#jni 头文件
include_directories(${NATIVE_JNI_PATH})
include_directories(${NATIVE_SIMPLE_PATH})
include_directories(${NATIVE_DECODE_PATH})
include_directories(${NATIVE_PLAYER_PATH})
include_directories(${NATIVE_UTIL_PATH})

#编码 CPP
#JNI 接口
FILE(GLOB JNI_CPP ${NATIVE_JNI_PATH}/*.cpp)
FILE(GLOB NATIVE_SIMPLE_CPP ${NATIVE_SIMPLE_PATH}/*.cpp)
FILE(GLOB NATIVE_DECODE_CPP ${NATIVE_DECODE_PATH}/*.cpp)
FILE(GLOB NATIVE_PLAYER_CPP ${NATIVE_PLAYER_PATH}/*.cpp)
FILE(GLOB NATIVE_UTIL_CPP ${NATIVE_UTIL_PATH}/*.cpp)


add_library( # Sets the name of the library.
        audio-lib

        SHARED

        ${NATIVE_SIMPLE_CPP}
        ${NATIVE_DECODE_CPP}
        ${NATIVE_PLAYER_CPP}
        ${NATIVE_UTIL_CPP}
        ${JNI_CPP})


find_library(
        log-lib
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        audio-lib
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        h264
        postproc
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})