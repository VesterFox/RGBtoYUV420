cmake_minimum_required (VERSION 3.12)

project (RGBtoYUV420)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 11)           
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
set(CMAKE_CXX_EXTENSIONS OFF)        

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SOURCES
	main.cpp
	${SRC_DIR}/bmp.cpp
	${SRC_DIR}/utils.cpp
	${SRC_DIR}/yuv.cpp
)

set(LIB_SOURCES
		${SRC_DIR}/bmp.cpp
		${SRC_DIR}/utils.cpp
		${SRC_DIR}/yuv.cpp
)

add_library(RGBtoYUV420_lib ${LIB_SOURCES})
target_include_directories(RGBtoYUV420_lib PUBLIC ${SRC_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})

add_subdirectory(tests)