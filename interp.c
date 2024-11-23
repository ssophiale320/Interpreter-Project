
 * A postfix expression interpreter with symbol table support.
 * This program evaluates postfix expressions, optionally using a
 * symbol table for variable bindings. It supports basic arithmetic and
 * assignment operations within the expression. The
 * program includes interactive input and file-based symbol table loading.
 *
 * ## Features:
 * - Evaluate postfix expressions with operators: +, -, *, /, =
 * - Load and manage symbols in a symbo ltable
 * - Display expressions in both postfix and infix forms
 *
 * ## Usage:
 * ```bash
 * ./interp [symbol-table-file]
 * ```
 * If a symbol table file is provided, it loads the variables into memory before
 * processing expressions.
 *
 * @file        interp.c
 * @author      Sophia Le (sel5881@rit.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "symtab.h"

#define MAX_LINE_LENGTH 1024
#define MAX_INFIX_LENGTH 1024

/**
 * Loads a symbol table from a file.
 *
 * @param filename: A path to the file containing symbol definitions
 */
void load(const char *filename) {
        FILE *file = fopen(filename, "r");

        if(!file) {
                perror(filename);
                exit(EXIT_FAILURE);
        }

        char line[BUFLEN];
        while(fgets(line, sizeof(line), file)) {
                char *start = line;
                while (isspace(*start)) start++;

                if(*start == '\0' || *start == '#') continue;

                char *com = strchr(start, '#');
                if(com) *com = '\0';

                char name[BUFLEN];
                int val;

                if(sscanf(line, "%s %d", name, &val) == 2) {
                        if(isalpha(name[0])) {
                                for(char *c = name; *c; c++) {
                                        if(!isalnum(*c)) {
                                                fprintf(stderr, "Error: Invalid symbol name '%s'\n", name);
                                                fclose(file);
                                                exit(EXIT_FAILURE);
                                        }
                                }
                                add_symbol(name, val);
                        } else {
                                fprintf(stderr, "Error: Symbol '%s' must start with a letter\n", name);
                                fclose(file);
                                exit(EXIT_FAILURE);
                        }
                } else {
                        fprintf(stderr, "Error processing line: %s\n", line);
                        fclose(file);
                        exit(EXIT_FAILURE);
                }
        }
        fclose(file);
}

/**
 * Evaluates a postfix expression and generates an infix equivalent.
 * Parses the provided postfix expression, evaluates it, and builds its infix
 * representation. Supports integer literals, variable lookup, and operators
 * (+, -, *, /, =). Handles errors such as undefined variables and division by
 * zero.
 *
 * @param exp: A pointer to the postfix expression as a string
 * @param infix: A pointer to a buffer to store the infix representation
 * @return: The result of the evaluated expression as an integer
 */
int eval(const char * exp, char * infix) {
        stack_t *stack = make_stack();
        const char * delim = " ";
        char * tok, *expr = strdup(exp);
        int result = 0;
        symbol_t *symbol = NULL;
        char *name = NULL;

        tok = strtok(expr, delim);
        while(tok != NULL) {
                if(isdigit(tok[0]) || (tok[0] == '-' && isdigit(tok[1]))) {
                        int * n = malloc(sizeof(int));
                        *n = atoi(tok);
                        push(stack, n);
                } else if (isalpha(tok[0])) {
                        symbol = lookup_table(tok);
                        if(symbol) {
                                int * v = malloc(sizeof(int));
                                *v = symbol->val;
                                push(stack, v);
                                snprintf(infix, MAX_INFIX_LENGTH, "%s", tok);
                                if(name == NULL) name = strdup(tok);
                        } else {
                                fprintf(stderr, "Error: Variable '%s' not found\n", tok);
                                free(expr);
                                free_stack(stack);
                                return 0;
                        }
                } else {
                        int * second = top(stack);
                        pop(stack);
                        int * first = top(stack);
                        pop(stack);

                        switch(tok[0]) {
                                case '+':
                                        result = *first + *second;
                                        snprintf(infix, MAX_INFIX_LENGTH, "(%d + %d)", *first, *second);
                                        break;
                                case '-':
                                        result = *first - *second;
                                        snprintf(infix, MAX_INFIX_LENGTH, "(%d - %d)", *first, *second);
                                        break;
                                case '*':
                                        result = *first * *second;
                                        snprintf(infix, MAX_INFIX_LENGTH, "(%d * %d)", *first, *second);
                                        break;
                                case '/':
                                        if(*second == 0) {
                                                fprintf(stderr, "Error: Division by zero\n");
                                                free(expr);
                                                free_stack(stack);
                                                return 0;
                                        }

                                        result = *first / *second;
                                        snprintf(infix, MAX_INFIX_LENGTH, "(%d / %d)", *first, *second);
                                        break;
                                case '=':
                                        if(symbol) {
                                                symbol->val = *second;
                                                snprintf(infix, MAX_INFIX_LENGTH, "(%s=(%s+1))", name, name);
                                        } else {
                                                fprintf(stderr, "Error: Variable '%s' not found for assignment\n", tok);
                                                free(expr);
                                                free_stack(stack);
                                                return 0;
                                        }
                                        result = *second;
                                        break;
                                default:
                                        fprintf(stderr, "Error: Unknown operator '%s'\n", tok);
                                        free(expr);
                                        free_stack(stack);
                                        return 0;
                        }

                        int * ptr = malloc(sizeof(int));
                        *ptr = result;
                        push(stack, ptr);
                        free(first);
                        free(second);
                }
                tok = strtok(NULL, delim);
        }
        free(expr);
        result = *(int *)top(stack);
        free_stack(stack);
        return result;
}

/**
 * Starts a user-interactive session for postfix expression evaluation.
 * The user can enter postfix expressions, which are evaluated and displayed
 * with their infix equivalent and result.
 */
void prompt() {
        printf("Enter postfix expressions (CTRL-D to exit):\n");
        char line[BUFLEN];

        while ( 1 ) {
                printf("> ");
                if(fgets(line, sizeof(line), stdin) == NULL) break;

                char * com = strchr(line, '#');
                if(com) *com = '\0';

                char * trim = strtok(line, "\n");

                if(trim && strlen(trim) > 0) {
                        char infix[MAX_INFIX_LENGTH];
                        int result = eval(trim, infix);

                        if(result != 0) printf("%s = %d\n", infix, result);
                }
        }
}

/**
 * Main entry point for the program. Parses command-line arguments,
 * optionally loads a symbol table, and starts the interactive interpreter.
 *
 * @param argc: The number of arguments
 * @param argv: The argument vector
 * @return: Exit status code
 */
int main(int argc, char *argv[]) {
        if(argc > 2) {
                fprintf(stderr, "usage: interp [sym-table]\n");
                return EXIT_FAILURE;
        }

        if(argc == 2) load(argv[1]);

        dump_table();

        prompt();

        dump_table();

        free_table();
        return EXIT_SUCCESS;
}



