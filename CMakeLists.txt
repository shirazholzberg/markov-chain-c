cmake_minimum_required(VERSION 3.22)
project(ex3b_shirazholzberg C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(tweet
        linked_list.c
        linked_list.h
        markov_chain.h
        #snakes_and_ladders.c
        tweets_generator.c
        markov_chain.c)

add_executable(snake
        linked_list.c
        linked_list.h
        markov_chain.h
        snakes_and_ladders.c
        #tweets_generator.c
        markov_chain.c)
