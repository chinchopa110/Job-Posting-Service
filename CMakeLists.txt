cmake_minimum_required(VERSION 3.14)
project(JobService VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(DOMAIN_SOURCES
    src/domain/Worker.cpp
    src/domain/Employer.cpp
    src/domain/JobPosting.cpp
    src/domain/Reply.cpp
)

set(REPOSITORY_SOURCES
    src/repository/WorkerRepository.cpp
    src/repository/EmployerRepository.cpp
    src/repository/JobPostingRepository.cpp
    src/repository/ReplyRepository.cpp
)

set(SERVICE_SOURCES
    src/service/WorkerService.cpp
    src/service/EmployerService.cpp
    src/service/JobPostingService.cpp
    src/service/ReplyService.cpp
    src/JobService.cpp
)

set(FILTER_SOURCES
    src/filter/JobFilter.cpp
)

set(ALL_SOURCES
    ${DOMAIN_SOURCES}
    ${REPOSITORY_SOURCES}
    ${SERVICE_SOURCES}
    ${FILTER_SOURCES}
)

add_executable(test_services ${ALL_SOURCES} tests/test_services.cpp)
add_executable(job_service ${ALL_SOURCES} src/main.cpp)

target_link_libraries(test_services PRIVATE GTest::gtest_main)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_services PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(job_service PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(test_services PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
set_target_properties(job_service PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(test_services)