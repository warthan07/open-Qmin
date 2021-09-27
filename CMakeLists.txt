cmake_minimum_required(VERSION 3.9.0)
project(openQmin LANGUAGES CUDA CXX)

find_package(MPI REQUIRED)

add_definitions(-DDIMENSION=5)

SET(CUDA_ARCH "80")

set(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11")
set(CMAKE_CUDA_FLAGS "${CUDA_NVCC_FLAGS} --expt-relaxed-constexpr -arch=sm_${CUDA_ARCH} -gencode=arch=compute_${CUDA_ARCH},code=sm_${CUDA_ARCH}")
set(CMAKE_CUDA_ARCHITECTURES ${CUDA_ARCH})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-DDEBUGFLAGUP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -g -lineinfo -Xptxas --generate-line-info")
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

message("CXX flags = " "${CMAKE_CXX_FLAGS}")
message("CUDA flags = " "${CMAKE_CUDA_FLAGS}")
message("CUDA include dirs = " "${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")

set(FORCESLIB_DIR src/forces)
file(GLOB FORCESLIB_CPP ${FORCESLIB_DIR}/*.cpp)
file(GLOB FORCESLIB_CU ${FORCESLIB_DIR}/*.cu)
add_library(Forces ${FORCESLIB_CPP} ${FORCESLIB_CU})
set_target_properties(Forces PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(MODELLIB_DIR src/model)
file(GLOB MODELLIB_CPP ${MODELLIB_DIR}/*.cpp)
file(GLOB MODELLIB_CU ${MODELLIB_DIR}/*.cu)
add_library(Model ${MODELLIB_CPP} ${MODELLIB_CU})
set_target_properties(Model PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(SIMULATIONLIB_DIR src/simulation)
file(GLOB SIMULATIONLIB_CPP ${SIMULATIONLIB_DIR}/*.cpp)
file(GLOB SIMULATIONLIB_CU ${SIMULATIONLIB_DIR}/*.cu)
add_library(Simulation ${SIMULATIONLIB_CPP} ${SIMULATIONLIB_CU})
set_target_properties(Simulation PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(UPDATERSLIB_DIR src/updaters)
file(GLOB UPDATERSLIB_CPP ${UPDATERSLIB_DIR}/*.cpp)
file(GLOB UPDATERSLIB_CU ${UPDATERSLIB_DIR}/*.cu)
add_library(Updaters ${UPDATERSLIB_CPP} ${UPDATERSLIB_CU})
set_target_properties(Updaters PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(UTILITIESLIB_DIR src/utilities)
file(GLOB UTILITIESLIB_CPP ${UTILITIESLIB_DIR}/*.cpp)
file(GLOB UTILITIESLIB_CU ${UTILITIESLIB_DIR}/*.cu)
add_library(Utilities ${UTILITIESLIB_CPP} ${UTILITIESLIB_CU})
set_target_properties(Utilities PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(myLibs Forces Model Simulation Updaters Utilities)
include_directories(
	inc
	${FORCESLIB_DIR}
	${MODELLIB_DIR}
	${SIMULATIONLIB_DIR}
	${UPDATERSLIB_DIR}
	${UTILITIESLIB_DIR}
	${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
	${OPENGL_INCLUDE_DIRS}
	${MPI_INCLUDE_PATH})
link_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}/../lib)


# list the names of cpp files corresponding to linked executables you'd like...
foreach(ARG openQmin)
add_executable("${ARG}.out" "${ARG}.cpp" ${SOURCES} ${HEADERS} ${UI_HEADERS})
target_link_libraries("${ARG}.out" ${myLibs} ${MPI_LIBRARIES} -lnvToolsExt)
if(MPI_COMPILE_FLAGS)
    set_target_properties("${ARG}.out" PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()
if(MPI_LINK_FLAGS)
    set_target_properties("${ARG}.out" PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
endforeach()
