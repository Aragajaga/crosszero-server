cmake_minimum_required(VERSION 3.1)
project(crosszero-server)

find_package(Boost 1.72.0 REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

set(EXECUTABLE_NAME czsrv)

add_executable(${EXECUTABLE_NAME} src/main.cc)
target_link_libraries(${EXECUTABLE_NAME}
  wsock32
  ws2_32
  SQLite::SQLite3
  OpenSSL::Crypto
  Boost::system
  nlohmann_json::nlohmann_json)
