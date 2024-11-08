cmake_minimum_required(VERSION 3.29)
project(OS_Codes C)

set(CMAKE_C_STANDARD 11)

add_executable(OS_Codes
        "chap5/简单型Clock置换算法 仿真.c"
#                "chap5/最近最少使用置换算法 仿真.c"
        #        main.c
#        chap3/banker_algorithm.c
#        "chap5/最佳置换算法 仿真.c"
#        "chap5/先进先出置换算法 仿真.c"
)
