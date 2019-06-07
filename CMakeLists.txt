cmake_minimum_required(VERSION 3.14)
project(myfirst C)

set(CMAKE_C_STANDARD 99)

add_executable(myfirst main.c lcd.c bmplist.c)
