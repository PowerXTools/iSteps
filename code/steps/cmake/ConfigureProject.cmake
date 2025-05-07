# 检查和设置编译器标志
macro(configure_compiler_flags)
    # 检查是否支持C++17
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
    if(NOT COMPILER_SUPPORTS_CXX17)
        message(FATAL_ERROR "编译器 ${CMAKE_CXX_COMPILER} 不支持 C++17 标准")
    endif()
    
    # 检查是否支持C17
    include(CheckCCompilerFlag)
    check_c_compiler_flag("-std=c17" COMPILER_SUPPORTS_C17)
    if(NOT COMPILER_SUPPORTS_C17)
        message(FATAL_ERROR "编译器 ${CMAKE_C_COMPILER} 不支持 C17 标准")
    endif()
    
    # 检查OpenMP支持
    find_package(OpenMP)
    if(NOT OpenMP_FOUND)
        message(WARNING "未找到OpenMP。某些性能功能将被禁用。")
    endif()
endmacro()

# 配置构建类型
macro(configure_build_type)
    # 设置默认构建类型
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "构建类型" FORCE)
        message(STATUS "未指定构建类型，将使用默认值: ${CMAKE_BUILD_TYPE}")
    endif()
    
    # 可用的构建类型
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
    
    # 设置构建特定的输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
endmacro()

# 配置项目版本信息
macro(configure_version)
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)
    set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    
    # 创建版本头文件
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/templates/version.h.in
        ${CMAKE_SOURCE_DIR}/header/basic/version.h
    )
endmacro()
