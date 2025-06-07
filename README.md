# Markov Chain Text & Game Generator (C)
This project demonstrates the use of generic **Markov Chains** in C to model random text generation and board game simulations.
It was implemented as part of a programming course (C & C++) in The Hebrew University and includes two main programs.

# Project Structure
- markov_chain.c / markov_chain.h: Generic implementation of Markov Chains using function pointers for any data type.
- linked_list.c / linked_list.h: Simple singly linked list implementation used by the chain.
- tweets_generator.c: Loads a text file (e.g., tweets) and generates random "tweets" based on learned word transitions.
- snakes_and_ladders.c: Uses the same Markov chain logic to generate random game paths on a 100-cell snakes and ladders board. 

# How to Compile
Use the provided `Makefile` (or compile manually if needed).
To build both programs:
