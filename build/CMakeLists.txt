cmake_minimum_required(VERSION 3.10)

project(eko VERSION 0.0.1)

target_link_options(mu-runtime PUBLIC
target_compile_options(mu-runtime 
add_executable(mu-runtime ../src/mu-runtime/repl.cc)
install(TARGETS mu-runtime)
