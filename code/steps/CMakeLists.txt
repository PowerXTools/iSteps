cmake_minimum_required(VERSION 3.10)

# 项目定义
project(libisteps VERSION 1.0.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置C标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 查找OpenMP
find_package(OpenMP REQUIRED)

# 定义编译选项函数
function(set_compiler_options target is_release)
    # 通用编译选项
    target_compile_options(${target} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fPIC>
        $<$<COMPILE_LANGUAGE:CXX>:-march=native>
        $<$<COMPILE_LANGUAGE:CXX>:-funroll-loops>
        $<$<COMPILE_LANGUAGE:CXX>:-malign-double>
        $<$<COMPILE_LANGUAGE:CXX>:-minline-all-stringops>
    )
    
    # OpenMP支持
    target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    
    if(${is_release})
        # Release模式特定的编译选项
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
            $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
            $<$<COMPILE_LANGUAGE:CXX>:-Winit-self>
            $<$<COMPILE_LANGUAGE:CXX>:-Wredundant-decls>
            $<$<COMPILE_LANGUAGE:CXX>:-Wcast-align>
            $<$<COMPILE_LANGUAGE:CXX>:-Wundef>
            $<$<COMPILE_LANGUAGE:CXX>:-Wfloat-equal>
            $<$<COMPILE_LANGUAGE:CXX>:-Winline>
            $<$<COMPILE_LANGUAGE:CXX>:-Wunreachable-code>
            $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-declarations>
            $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-include-dirs>
            $<$<COMPILE_LANGUAGE:CXX>:-Wswitch-enum>
            $<$<COMPILE_LANGUAGE:CXX>:-Wswitch-default>
            $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>
            $<$<COMPILE_LANGUAGE:CXX>:-Wzero-as-null-pointer-constant>
            $<$<COMPILE_LANGUAGE:CXX>:-Wmain>
            $<$<COMPILE_LANGUAGE:CXX>:-pedantic>
            $<$<COMPILE_LANGUAGE:CXX>:-w>
            $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
            $<$<COMPILE_LANGUAGE:CXX>:-pg>
        )
        # Release模式链接选项
        target_link_options(${target} PRIVATE -pg -lgmon -static-libstdc++ -static-libgcc -march=native)
    else()
        # Debug模式特定的编译选项
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-Wall>
        )
        # Debug模式链接选项
        target_link_options(${target} PRIVATE -static-libstdc++ -static-libgcc -march=native)
    endif()
endfunction()

# 设置包含目录
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/UMFPACK/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/CSparse/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/cpptest
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/SuiteSparse_config
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/AMD/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/COLAMD/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/KLU/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/BTF/Include
    ${CMAKE_SOURCE_DIR}/../thirdparty/SuiteSparse/CXSparse/Include
)

# 设置依赖库
set(DEPEND_LIBS
    ${CMAKE_SOURCE_DIR}/../libdepend/libCSparse.a
    ${CMAKE_SOURCE_DIR}/../libdepend/libCpptest.a
    ${CMAKE_SOURCE_DIR}/../libdepend/libCXSparse.a
)

# 查找所有源文件和头文件
file(GLOB_RECURSE SRC_FILES 
    ${CMAKE_SOURCE_DIR}/source/*.cpp
)

file(GLOB_RECURSE HEADER_FILES 
    ${CMAKE_SOURCE_DIR}/header/*.h
)

# 添加demos/main.cpp文件
list(APPEND SRC_FILES ${CMAKE_SOURCE_DIR}/../demos/main.cpp)

# 创建可执行文件
add_executable(isteps ${SRC_FILES} ${HEADER_FILES})

# 设置包含目录
target_include_directories(isteps PRIVATE ${INCLUDE_DIRS})

# 链接依赖库
target_link_libraries(isteps PRIVATE ${DEPEND_LIBS})

# 根据构建类型设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_compiler_options(isteps FALSE)
else()
    set_compiler_options(isteps TRUE)
endif()

# 设置输出目录
set_target_properties(isteps PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin
)

# 安装规则
install(TARGETS isteps
    RUNTIME DESTINATION bin
)
