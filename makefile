CC = gcc
CFLAGS =-Wall -Wextra

all:tweets snake

tweets:tweets_generator
tweets_generator:tweets_generator.o markov_chain.o linked_list.o
	$(CC) $(CFLAGS) -o tweets_generator tweets_generator.o markov_chain.o linked_list.o
tweets_generator.o: tweets_generator.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c tweets_generator.c
markov_chain.o: markov_chain.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c markov_chain.c
linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c linked_list.c

snake:snakes_and_ladders
snakes_and_ladders: snakes_and_ladders.o markov_chain.o linked_list.o
	$(CC) $(CFLAGS) -o snakes_and_ladders snakes_and_ladders.o markov_chain.o linked_list.o
snakes_and_ladders.o: snakes_and_ladders.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c snakes_and_ladders.c
clean:
	rm -f snakes_and_ladders snakes_and_ladders.o tweets_generator tweets_generator.o markov_chain.o linked_list.o