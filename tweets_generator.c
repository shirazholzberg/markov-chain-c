#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "markov_chain.h"

#define MAX_SENTENCE 1001
#define BASE 10
#define MAX_TWEET 20
#define LENGTH_5 5
#define LENGTH_4 4
#define NO_WORDS -1

/**
 * @param words_to_read number of lines to read
 * @param num_words_read number of lines that has already been read
 * @return 1 if the user gave no numbers to read, or there are words to read,
 * but the amount of words to be read is smaller and we need
 * to keep on reading! else 0
 */
static int got_num_words_to_read(int words_to_read, int num_words_read) {
    if (words_to_read == NO_WORDS) {
        return 1;
    } else if (words_to_read > num_words_read) {
        return 1;
    }
    return 0;
}

/**
 * the function fills the database in the given markov chain
 * according to the given file
 * @param fp a file
 * @param words_to_read number of words to read
 * @param markov_chain a pointer to a markov chain
 * @return 0 if the database is filled successfully, else 1
 */
static int fill_database(FILE *fp, int words_to_read,
                         MarkovChain *markov_chain) {
    int num_words_read = 0;
    char line[MAX_SENTENCE];
    char *word;
    while ((fgets(line, MAX_SENTENCE, fp) != NULL) &&
           (got_num_words_to_read(words_to_read, num_words_read))) {
        word = strtok(line, " \n\r\t");
        if (word != NULL) {
            Node *first_node = add_to_database(markov_chain, word);
            if (first_node != NULL) { //word added
                num_words_read++;
                word = strtok(NULL, " \n\r\t");
            } else { //memory problem
                return 1;
            }
            if (words_to_read == num_words_read) {
                break;
            }
            while ((word != NULL) &&
                   (got_num_words_to_read(words_to_read, num_words_read))) {
                Node *second_node = add_to_database(markov_chain, word);
                if (second_node != NULL) { //word added
                    num_words_read++;
                    word = strtok(NULL, " \n\r\t");
                } else { //memory problem
                    return 1;
                }
                //updating frequencies list
                if (!add_node_to_frequencies_list(first_node->data,
                                                  second_node->data,
                                                  markov_chain)) {
                    return 1; //memory problem
                }
                //replacing first node and second node
                first_node = second_node;
            }
        }
    }
    return 0;
}

/**
 * the function checks the file path given from the user and prints matching
 * messages if needed.
 * @param path the path to the file given by the user.
 * @return EXIT_SUCCESS if the path is valid, else EXIT_FAILURE.
 */
static int path_checks(char *path) {
    if (access(path, F_OK) == -1) {
        printf("Error: file doesn't exist\n");
        return EXIT_FAILURE;
    }
    if (access(path, R_OK) == -1) {
        printf("Error: no permission for the file\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * the function checks the arguments given from the user and prints matching
 * messages if needed.
 * @param argc number of arguments
 * @param argv the arguments
 * @return EXIT_SUCCESS if there are 3 or 4 arguments and the path is valid,
 * else EXIT_FAILURE.
 */
static int arguments_check(int argc, char *argv[]) {
    if ((argc != LENGTH_4) && (argc != LENGTH_5)) {
        printf("Usage: the program receives only 3 or 4 arguments\n");
        return EXIT_FAILURE;
    }
    if (path_checks(argv[3])) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 *the function prints a word with space after it.
 * @param data the given word
 */
static void print_word(void *data) {
    char *word = data;
    printf("%s ", word);
}

/**
 *the function checks if the last char is '.'
 * @param data the word to check
 * @return true if the last char is '.', else false
 */
static bool check_if_last(void *data) {
    char *word = data;
    size_t len = strlen(word);
    if (word[len - 1] == '.') { // Check the last character
        return true;
    }
    return false;
}

/**
 *the function comperes to words
 * @param data1 the first word
 * @param data2 the second word
 * @return the function returns 0 if the 2 words are equal,
 * a positive value if the first is bigger
 * a negative value if the second is bigger
 */
static int comp_chars(void *data1, void *data2) {
    char *word1 = data1;
    char *word2 = data2;
    return strcmp(word1, word2);
}

/**
 * the function copies and allocates the given word
 * @param data
 * @return the new allocation of the copied word if the allocation succeeded,
 * else NULL
 */
static void *copy_char(void *data) {
    char *ptr_src = data;
    char *word = malloc(strlen(ptr_src) + 1);
    if (word == NULL) {
        return NULL;
    }
    strcpy(word, ptr_src);
    return word;
}

/**
 * the function initializes a new markov chain
 * @param markov_chain the new marko chain
 * @param database the database to put in the chain
 */
static void
initializing_chain(MarkovChain *markov_chain, LinkedList *database) {
    markov_chain->database = database;
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;
    markov_chain->print_func = print_word;
    markov_chain->is_last = check_if_last;
    markov_chain->free_data = free;
    markov_chain->comp_func = comp_chars;
    markov_chain->copy_func = copy_char;
}

int main(int argc, char *argv[]) {
    if (arguments_check(argc, argv)) {
        return EXIT_FAILURE;
    }
    FILE *tweets_file = fopen(argv[3], "r");
    if (tweets_file == NULL) {
        printf("Error: problem with reading file path.");
        return EXIT_FAILURE;
    }
    int words_to_read = NO_WORDS;
    if (argc == LENGTH_5) {
        char *endptr;
        words_to_read = strtol(argv[4], &endptr, BASE);
    }
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (markov_chain == NULL) {
        printf(ALLOCATION_ERROR_MASSAGE);
        fclose(tweets_file);
        return EXIT_FAILURE;
    }
    LinkedList *database = malloc(sizeof(LinkedList));
    if (database == NULL) {
        printf(ALLOCATION_ERROR_MASSAGE);
        free(markov_chain);
        fclose(tweets_file);
        return EXIT_FAILURE;
    }
    initializing_chain(markov_chain, database);
    if (fill_database(tweets_file, words_to_read, markov_chain)) {
        free_database(&markov_chain);
        fclose(tweets_file);
        return EXIT_FAILURE;
    }
    char *endptr1, *endptr2;
    unsigned int seed = strtol(argv[1], &endptr1, BASE);
    srand(seed);
    int num_tweets = strtol(argv[2], &endptr2, BASE);
    for (int i = 0; i < num_tweets; i++) {
        printf("Tweet %d: ", i + 1);
        MarkovNode *first_node = get_first_random_node(markov_chain);
        generate_tweet(markov_chain, first_node, MAX_TWEET);
        printf("\n");
    }
    fclose(tweets_file);
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}
