#include "markov_chain.h"

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
    Node *result = get_node_from_database(markov_chain, data_ptr);
    if (result != NULL) {
        return result;// the given data exists
    } else {
        MarkovNode *m_node = malloc(sizeof(MarkovNode));
        if (m_node == NULL) {
            printf(ALLOCATION_ERROR_MASSAGE);
            return NULL;
        }
        void* data = markov_chain->copy_func(data_ptr);
        if(data == NULL){
            printf(ALLOCATION_ERROR_MASSAGE);
            return NULL;
        }
        *m_node = (MarkovNode) {data, NULL, 0};
        if (add(markov_chain->database, m_node)) {
            return NULL; // failed to add
        }
        return markov_chain->database->last;
    }
}

/**
 * as described in markov_chain.h
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr){
    Node *current = markov_chain->database->first;
    while (current) { //searching if the given data is already in the chain
        if (markov_chain->comp_func(current->data->data, data_ptr) == 0) {
            return current;// the given data exists
        }
        current = current->next;
    }
    return NULL;// the given data doesn't exist
}

/**
 * as described in markov_chain.h
 */
bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
    for (int i = 0; i < first_node->frequencies_list_len; i++) {
        if (markov_chain->comp_func(second_node->data,
                   first_node->frequencies_list[i].markov_node->data) == 0) {
            first_node->frequencies_list[i].frequency++;
            return true;
        }
    }
    //the second node is new:
    if (first_node->frequencies_list == NULL) { //if the list is empty
        first_node->frequencies_list =
                malloc(sizeof(MarkovNodeFrequency));
        if (first_node->frequencies_list == NULL) {
            printf(ALLOCATION_ERROR_MASSAGE);
            return false;
        }
    } else { //the list is not empty and we need to expand it:
        MarkovNodeFrequency
                *temp = realloc(first_node->frequencies_list,
                                (sizeof(MarkovNodeFrequency) *
                                 ((first_node->frequencies_list_len) + 1)));
        if (!temp) {
            printf(ALLOCATION_ERROR_MASSAGE);
            return false;
        }
        first_node->frequencies_list = temp;
    }
    first_node->frequencies_list[(first_node->frequencies_list_len)] =
            (MarkovNodeFrequency) {second_node, 1};
    first_node->frequencies_list_len++;
    return true;
}

/**
 * as described in markov_chain.h
 */
void free_database(MarkovChain **markov_chain){
    Node *current = (*markov_chain)->database->first;
    while (current) { // running on every node in the chain
        free(current->data->frequencies_list);//free frequencies_list
        current->data->frequencies_list = NULL;
        current->data->frequencies_list_len = 0;
        (*markov_chain)->free_data(current->data->data); //free the data
        current->data->data = NULL;
        free(current->data); // free the markov_node fo the current node
        Node *next = current->next;
        current->next = NULL;
        free(current);
        current = next;
    }
    (*markov_chain)->database->first = NULL;
    (*markov_chain)->database->last = NULL;
    (*markov_chain)->database->size = 0;
    free((*markov_chain)->database); // free the database
    (*markov_chain)->database = NULL;
    free(*markov_chain);// free the markov chain
    *markov_chain = NULL;
}

/**
 * as described in markov_chain.h
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    int i = get_random_number(markov_chain->database->size);
    Node *cur = markov_chain->database->first;
    for (int j = 0; j < i; j++) {
        cur = cur->next;
    }
    while (markov_chain->is_last(cur->data->data)) {
        i = get_random_number(markov_chain->database->size);
        cur = markov_chain->database->first;
        for (int j = 0; j < i; j++) {
            cur = cur->next;
        }
    }
    return cur->data;
}

/**
 * as described in markov_chain.h
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
    int sum = 0;
    for (int j = 0; j < state_struct_ptr->frequencies_list_len; j++) {
        sum += state_struct_ptr->frequencies_list[j].frequency;
    }
    int i = get_random_number(sum);
    MarkovNodeFrequency *current = state_struct_ptr->frequencies_list;
    while (i >= current->frequency) {
        i -= current->frequency;
        current++;
    }
    return current->markov_node;
}

/**
 * as described in markov_chain.h
 */
void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length){
    if (first_node == NULL) {
        first_node = get_first_random_node(markov_chain);
    }
    markov_chain->print_func(first_node->data);
    for (int i = 1; i < max_length; i++) {
        MarkovNode *next_node = get_next_random_node(first_node);
        markov_chain->print_func(next_node->data);
        if (markov_chain->is_last(next_node->data)) {//the end
            break;
        }
        first_node = next_node;
    }
}

/**
 * as described in markov_chain.h
 */
int get_random_number(int max_number) {
    return rand() % max_number;
}


