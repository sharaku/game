# ----------------------------------------------------------------------------
#
#  MIT License
#  
#  Copyright (c) 2016 Abe Takafumi
#  
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#  
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#  
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE. *
#
#
#  Modules CMake file for libsharaku
# ----------------------------------------------------------------------------

# ---------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)

# ---------------------------------------------------------------
# windowsの場合は、ここで設定を取り込む
if(NOT CMAKE_TOOLCHAIN_FILE)
	if(WIN32)
		include(cmake/win10-x86.cmake)
	endif()
endif(NOT CMAKE_TOOLCHAIN_FILE)

include_directories(
	include
	../../container/include
	../../pool/include
	)

# ---------------------------------------------------------------
# コンパイラ引数
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CROSS_FLAGS_C}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CROSS_FLAGS_CXX}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CROSS_FLAGS_C} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CROSS_FLAGS_CXX} -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${CROSS_FLAGS_C}-O2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CROSS_FLAGS_CXX} -O2")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CROSS_FLAGS_C} -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CROSS_FLAGS_CXX} -O2")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${CROSS_FLAGS_C} -O3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${CROSS_FLAGS_CXX} -O3")

# ---------------------------------------------------------------
# libs
set(MODULE_SYSTEM
	"src/chess_field.cpp"
	"src/wslg_field.cpp"
	)
add_library(game.wslg.${TARGET_SUFFIX} STATIC
	${MODULE_SYSTEM}
	)

# ---------------------------------------------------------------
# testing

# ---------------------------------------------------------------
# exsample
add_executable(chess_sample 
	example/chess_sample.cpp
	)
target_link_libraries(chess_sample
	game.wslg.${TARGET_SUFFIX}
	)

# ---------------------------------------------------------------
# tools

