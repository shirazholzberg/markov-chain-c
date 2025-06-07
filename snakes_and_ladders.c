#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define BASE 10
# define VALID_INPUT_LENGTH 3
#define LAST_CELL 100
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case
    // there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there
    // is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database) {
    printf("%s", error_msg);
    if (database != NULL) {
        free_database(database);
    }
    return EXIT_FAILURE;
}

/**
 * the function builds a board of cells and initializes it
 * @param cells
 * @return EXIT_SUCCESS if the board is allocated successfully, else
 * EXIT_FAILURE
 */
int create_board(Cell *cells[BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE, NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++) {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to) {
            cells[from - 1]->ladder_to = to;
        } else {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain) {
    Cell *cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++) {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++) {
        from_node = get_node_from_database(markov_chain,
                                           cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY) {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])
                    ->data;
            add_node_to_frequencies_list
                    (from_node, to_node, markov_chain);
        } else {
            for (int j = 1; j <= DICE_MAX; j++) {
                index_to = ((Cell *) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE) {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])
                        ->data;
                add_node_to_frequencies_list(from_node,
                                             to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++) {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

/**
 * the function checks the arguments given from the user and prints matching
 * messages if needed.
 * @param argc number of arguments
 * @param argv the arguments
 * @return EXIT_SUCCESS if there are 2 arguments, else EXIT_FAILURE.
 */
static int arguments_check(int argc) {
    if (argc != VALID_INPUT_LENGTH) {
        printf("Usage: the program receives only 2 arguments.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * the function checks if the cell is last - if the cell's number is 100
 * @param data a cell
 * @return true if the cell is last, else false
 */
static bool check_if_last(void *data) {
    Cell *cell = data;
    if (cell->number == LAST_CELL) {
        return true;
    }
    return false;
}

/**
 * the function comperes 2 cells by its numbers
 * @param data1 first cell
 * @param data2 second cell
 * @return the function returns 0 if equal
 * a positive value if the first is bigger
 * a negative value if the second is bigger
 */
static int comp_cells(void *data1, void *data2) {
    Cell *cell1 = data1;
    Cell *cell2 = data2;
    return (cell1->number - cell2->number);
}

/**
 * the function allocates and copies the given data into a new cell
 * @param data
 * @return the new allocated cell if the allocation succeeded, else NULL.
 */
static void *copy_cell(void *data) {
    Cell *ptr_src = data;
    Cell *new_cell = malloc(sizeof(Cell));
    if (new_cell == NULL) {
        return NULL;
    }
    new_cell->number = ptr_src->number;
    new_cell->ladder_to = ptr_src->ladder_to;
    new_cell->snake_to = ptr_src->snake_to;
    return new_cell;
}

/**
 * the function prints the cell's number in a specific form
 * @param data the cell for print
 */
static void print_cell(void *data) {
    Cell *cell = data;
    if (cell->number == LAST_CELL) {
        printf("[%d]", cell->number);
    } else if ((cell->snake_to != EMPTY)) {
        printf("[%d]-snake to %d -> ", cell->number, cell->snake_to);
    } else if ((cell->ladder_to != EMPTY)) {
        printf("[%d]-ladder to %d -> ", cell->number, cell->ladder_to);
    } else {
        printf("[%d] -> ", cell->number);
    }
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
    markov_chain->print_func = print_cell;
    markov_chain->is_last = check_if_last;
    markov_chain->free_data = free;
    markov_chain->comp_func = comp_cells;
    markov_chain->copy_func = copy_cell;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    if (arguments_check(argc)) {
        return EXIT_FAILURE;
    }
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (markov_chain == NULL) {
        printf(ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    LinkedList *database = malloc(sizeof(LinkedList));
    if (database == NULL) {
        printf(ALLOCATION_ERROR_MASSAGE);
        free(markov_chain);
        return EXIT_FAILURE;
    }
    initializing_chain(markov_chain, database);
    fill_database(markov_chain);
    char *endptr1, *endptr2;
    unsigned int seed = strtol(argv[1], &endptr1, BASE);
    srand(seed);
    int num_plays = strtol(argv[2], &endptr2, BASE);
    for (int i = 0; i < num_plays; i++) {
        printf("Random Walk %d: ", i + 1);
        MarkovNode *first_node = markov_chain->database->first->data;
        generate_tweet(markov_chain, first_node,
                       MAX_GENERATION_LENGTH);
        printf("\n");
    }
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}