cmake_minimum_required(VERSION 3.0.0)

PROJECT(fib C)

INCLUDE_DIRECTORIES(                    # 相于 make 中的 -I参数 
"../include"
$ENV{NEKO_INSTPATH}/include
)

#################################
FILE(GLOB FIB_SRC "../src/fib.c")       # 和 SET 很像, 但是 FILE 能以通配符的形式指定文件

SET(LIBRARY_OUTPUT_PATH                 # 更改最终输目录和 CMakeLists 同层的 release/debug 下 
	${CMAKE_SOURCE_DIR}
)      
ADD_LIBRARY(fib MODULE ${FIB_SRC})      # MODULE 用于 neko, 反正 neko 的源码是指定为这个

SET_TARGET_PROPERTIES(fib               # 不能在 ADD_LIBRARY/ADD_EXECUTABLE 语句之前
	PROPERTIES
	PREFIX ""           # 前缀
##	OUTPUT_NAME fib     # 文件名
	SUFFIX .ndll        # 文件尾
	VERSION 0.1.0
	SOVERSION 0
)

#################################
ADD_LIBRARY(share SHARED ${FIB_SRC})    # 由于 main.c 链接时需要指定 SHARED/STATIC
                                        # 但好像在 windows 下和 fib出来的文件并没有什么分别
#################################
## FILE(GLOB SOURCE "../src/*.c")
FILE(GLOB SOURCE "../src/main.c")
ADD_EXECUTABLE(main ${SOURCE})
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(main              # 只要指定目标名好了, so easy!
	share
)

# MSVC - > msbuild XXX.sln /p:Configuration=Release|Debug [/t:Clean]
##
# LINK_DIRECTORIES(DIR_1 DIR_2 ....)	# 参考 INCLUDE_DIRECTORIES, 相当于 -L
# ADD_CUSTOM_COMMAND(OUTPUT|TARGET ....)
# ADD_CUSTOM_TARGET(NAME[ALL]...)       # 添加一个没有输出的目标, 它总是被构建

# -D   例: ADD_DEFINITIONS(-DENABLE_DEBUG -DABC)
# 依赖 例: ADD_DEPENDENCIES(target_name depend_target_1 depend_target_2)