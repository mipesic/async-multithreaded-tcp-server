cmake_minimum_required(VERSION 3.14)

include("cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
        SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
        LOCAL
)

project(tbd)

set(CMAKE_CXX_STANDARD 14)

hunter_add_package(Boost COMPONENTS system)
find_package(Boost CONFIG REQUIRED system)

add_executable(tbd main.cpp)
target_link_libraries(tbd INTERFACE Boost::headers Boost::system)

#TODO(milos): check why find_package doesn't do what's done on the following line
include_directories(${BOOST_ROOT}/include/)