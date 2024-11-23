/**
 * Implementation of a symbol table for managing variable names
 * and values. Provides functions to manage a symbol table implemented
 * as a linked list. Supports operations such as adding symbols,
 * looking up symbols, dumping the table for debugging, and freeing
 * memory. The symbol table can also be initialized from a file.
 *
 * @file        symtab.c
 * @author      Sophia Le (sel5881@rit.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static symbol_t * symbol_table = NULL; /// Pointer to the head of the symbol table
static int size = 0; /// Current number of symbols in the table

/**
 * Creates a new symbol with the given name and value.
 *
 * @param name: A pointer to the variable name (string)
 * @param val: Initial value of the variable
 * @return: A pointer to the newly created symbol, or NULL if memory allocation fails
 */
symbol_t * create_symbol(char * name, int val) {
        symbol_t * symbol = (symbol_t *)malloc(sizeof(symbol_t));

        if(!symbol) {
                perror("Failed to create symbol");
                return NULL;
        }

        symbol->var_name = strdup(name);

        if(!symbol->var_name) {
                perror("Failed to allocate memory for symbol name");
                free(symbol);
                return NULL;
        }

        symbol->val = val;
        symbol->next = NULL;
        return symbol;
}

/**
 * Adds a new symbol to the symbol table.
 *
 * @param name: A pointer to the variable name (string)
 * @param val: Initial value of the variable
 * @return: A pointer to the added symbol, or NULL if creation fails
 */
symbol_t * add_symbol(char * name, int val) {
        symbol_t * symbol = create_symbol(name, val);

        if(!symbol) return NULL;

        symbol->next = symbol_table;
        symbol_table = symbol;
        size++;
        return symbol;
}

/**
 * Builds the symbol table from a file
 *
 * @param filename: A pointer to the name of the file containing symbols
 */
void build_table(char * filename) {
        if(filename == NULL) {
                symbol_table = NULL;
                size = 0;
                return;
        }

        FILE *file = fopen(filename, "r");

        if(!file) {
                fprintf(stderr, "Error loading symbol table\n");
                exit(EXIT_FAILURE);
        }

        char name[BUFLEN];
        int val;
        while(fscanf(file, "%*s %s %d", name, &val) == 2) {
                add_symbol(name, val);
        }
        fclose(file);
}

/**
 * Dumps the contents of the symbol table to standard output
 */
void dump_table(void) {
        printf("SYMBOL TABLE:\n");

        for(symbol_t * curr = symbol_table; curr != NULL; curr = curr->next) {
                printf("\tName: %s, Value: %d\n", curr->var_name, curr->val);
        }
}

/**
 * Looks up a variable in the symbol table by name
 *
 * @param variable: A pointer to the variable name(string)
 * @return: A pointer to the symbol if found, NULL if not found
 */
symbol_t * lookup_table(char * variable) {
        for(symbol_t * curr = symbol_table; curr != NULL; curr = curr->next) {
                if(strcmp(curr->var_name, variable) == 0) return curr;
        }

        return NULL;
}

/**
 * Frees all memory associated with the symbol table
 */
void free_table(void) {
        symbol_t * curr = symbol_table;

        while(curr != NULL) {
                symbol_t * nxt = curr->next;
                free(curr->var_name);
                free(curr);
                curr = nxt;
        }

        symbol_table = NULL;
        size = 0;

        free(symbol_table);
        symbol_table = NULL;
}
