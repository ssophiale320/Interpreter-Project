/*
 * Implements a recursive parser for constructing and evaluating an abstract syntax tree (AST)
 * from arithmetic expressions in postfix notation. Includes utilities to evaluate expressions,
 * print them in infix notation, and memory cleanup.
 *
 * @file        parser.c
 * @author      Sophia Le (sel5881@rit.edu)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "tree_node.h"
#include "stack.h"
#include "symtab.h"

/**
 * Determines if a string represents a valid integer.
 *
 * @param str: A pointer to the string to be checked
 * @return: 1 if the string is a valid integer, 0 otherwise
 */
int is_num(char * str) {
        if(str == NULL || *str == '\0') return 0;
        if(*str == '-') str++;

        while(*str) {
                if(*str < '0' || *str > '9') return 0;
                str++;
        }
        return 1;
}

/**
 * Checks if a string matches a recognized operator (+, -, *, /, %, =)
 *
 * @param token: A pointer to the string to be checked
 * @return: 1 if the string is a valid operator, 0 otherwise
 */
int is_operator(const char * token) {
        return (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
                strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
                strcmp(token, "%") == 0 || strcmp(token, "=") == 0);
}

/**
 * Constructs an AST from a space-separated postfix expression string.
 *
 * @param exp: The postfix expression string
 * @return: Pointer to the root of the constructed AST or NULL on error
 */
tree_node_t * make_parse_tree(char * exp) {
        if(!exp || strlen(exp) == 0) {
                fprintf(stderr, "Error: empty expression\n");
                return NULL;
        }

        stack_t * stk = make_stack();
        char * tok = strtok(exp, " ");

        while(tok != NULL) {
                if(tok[0] != '\0' && strspn(tok, " \t\n\r") != strlen(tok)) {
                        push(stk, strdup(tok));
                }
                tok = strtok(NULL, " ");
        }

        if(empty_stack(stk)) {
                fprintf(stderr, "Error: Empty expression\n");
                free_stack(stk);
                return NULL;
        }

        tok = (char *)top(stk);
        pop(stk);
        if(tok == NULL || tok[0] == '\0' || strspn(tok, " \t\n\r") == strlen(tok)) {
                fprintf(stderr, "Error: invalid token\n");
                free_stack(stk);
                return NULL;
        }

        if(is_num(tok)) {
                tree_node_t * node = make_leaf(INTEGER, tok);
                push(stk, node);
        } else if(isalpha(tok[0])) {
                tree_node_t * node = make_leaf(SYMBOL, tok);
                push(stk, node);
        } else {
                fprintf(stderr, "Error: Unknown token '%s'\n", tok);
                free_stack(stk);
                return NULL;
        }

        if(stk->top != NULL && stk->top->next == NULL) {
                tree_node_t *root = (tree_node_t *)top(stk);
                pop(stk);
                free_stack(stk);
                return root;
        } else {
                fprintf(stderr, "Error: Invalid expression, too many tokens\n");
                free_stack(stk);
                return NULL;
        }
}

/**
 * Recursively parses through the tokens from a stack into an AST.
 *
 * @param stack: A pointer to the stack containing tokens in postfix order
 * @return: A pointer to the root of the subtree or NULL upon error
 */
tree_node_t * parse(stack_t * stack) {
        if(empty_stack(stack)) {
                fprintf(stderr, "Error: Empty stack\n");
                return NULL;
        }

        tree_node_t * node = NULL;

        while(!empty_stack(stack)) {
                char * tok = (char *)top(stack);
                pop(stack);
                printf("[parser] Popped tok: '%s'\n", tok);
                if(!tok || tok[0] == '\0') {
                        fprintf(stderr, "Error: null token\n");
                        free_stack(stack);
                        return NULL;
                }

                printf("[parser] Parsing token: '%s\n", tok);
                if(is_num(tok)){
                        printf("[DETECTED INTEGER TOKEN: '%s']\n", tok);
                        node = make_leaf(INTEGER, tok);
                        if(!node) {
                                fprintf(stderr, "\tError: Failed to create leaf node for integer\n");
                                free_stack(stack);
                                return NULL;
                        }
                        return node;
                } else if(isalpha(tok[0])) {
                        printf("[DETECTED SYMBOL TOKEN '%s']\n", tok);
                        node = make_leaf(SYMBOL, tok);
                        if(!node) {
                                fprintf(stderr, "\tError: Failed to create leaf node for symbol\n");
                                free_stack(stack);
                                return NULL;
                        }
                        printf("[parser] Successfully created leaf node for symbol\n");
                        return node;
                } else if(is_operator(tok)) {
                        printf("[DETECTED OPERATOR TOKEN: '%s']\n", tok);
                        if(stack->top == NULL || stack->top->next == NULL) {
                                fprintf(stderr, "\tError: not enough operands for operator '%s'\n", tok);
                                free_stack(stack);
                                return NULL;
                        }
                        char * oper = tok;
                        tree_node_t *right = parse(stack);
                        tree_node_t *left = parse(stack);

                        op_type_t op;
                        if(strcmp(tok, ADD_OP_STR) == 0) op = ADD_OP;
                        else if(strcmp(oper, SUB_OP_STR) == 0) op = SUB_OP;
                        else if(strcmp(oper, MUL_OP_STR) == 0) op = MUL_OP;
                        else if(strcmp(oper, DIV_OP_STR) == 0) op = DIV_OP;
                        else if(strcmp(oper, MOD_OP_STR) == 0) op = MOD_OP;
                        else if(strcmp(oper, ASSIGN_OP_STR) == 0) op = ASSIGN_OP;
                        else {
                                fprintf(stderr, "\tError: Unknown operator '%s'\n", tok);
                                free(tok);
                                free_stack(stack);
                                return NULL;
                        }
                        node = make_interior(op, oper, left, right);
                        //printf("[parser]: Tok after = %s\n", tok);
                        if(!node || !node->token || node->token[0] == '\0') {
                                fprintf(stderr, "\tError: Failed to create interior node for operator '%s'\n", tok);
                                free(tok);
                                free_stack(stack);
                                return NULL;
                        }
                        return node;
                } else if(strcmp(tok, "?") == 0) {
                        printf("[DETECTED TERNARY OPERATOR: '%s']\n", tok);
                        tree_node_t *con = (tree_node_t *)parse(stack);
                        printf("\t[parser]: Created condition for ternary op\n");
                        if(!con) {
                                fprintf(stderr, "Error: Missing condition for ternary op\n");
                                return NULL;
                        }

                        tree_node_t *t = parse(stack);
                        printf("\t[parser]: Created true expression for ternary op\n");
                        if(t == NULL) {
                                fprintf(stderr, "\tError: failed to parse true expression after '?'\n");
                                cleanup_tree(t);
                                return NULL;
                        }

                        tree_node_t *f = parse(stack);
                        printf("\t[parser]: Created false expression for ternary op\n");
                        if(f == NULL) {
                                fprintf(stderr, "\tError: Failed to parse false expression after ':'\n");
                                cleanup_tree(f);
                                return NULL;
                        }
                        tree_node_t * n = make_interior(ALT_OP, ":", t, f);
                        if(!n) {
                                fprintf(stderr, "\tError: failed to create ternary node\n");
                                free_stack(stack);
                                return NULL;
                        }
                        node = make_interior(Q_OP, "?", con, n);
                        return node;
                } else if(strcmp(tok, ":")) {
                        printf("\t[DETECTED ALT OPERATION: '%s']\n", tok);
                        tree_node_t * t = parse(stack);
                        tree_node_t * f = parse(stack);
                        if(!t || !f) {
                                fprintf(stderr, "Error: Invalid T/F expressions for alt op\n");
                                cleanup_tree(t);
                                cleanup_tree(f);
                                return NULL;
                        }
                        node = make_interior(ALT_OP, ":", t, f);
                        return node;
                } else {
                        fprintf(stderr, "\tError: Invalid token '%s'\n", tok);
                        free(tok);
                        free_stack(stack);
                        return NULL;
                }
                free(tok);
        }
        return node;
}

/**
 * Evaluates the result of an expression represented by an AST
 *
 * @param node: A pointer to the root of the AST
 * @return: The integer result of the evaluation
 */
int eval_tree(tree_node_t * node) {
        if(node == NULL) return 0;
        if(node->type == LEAF) {
                printf("[DETECTED LEAF NODE]\n");
                leaf_node_t * leaf = (leaf_node_t *)node->node;
                if(leaf->exp_type == INTEGER) {
                        printf("\t[eval]: Found integer node\n");
                        return atoi(node->token);
                } else if(leaf->exp_type == SYMBOL) {
                        printf("\t[eval]: Found symbol node\n");
                        symbol_t * symbol = lookup_table(node->token);
                        if(symbol != NULL) {
                                printf("\t[eval]: Symbol: %d\n", symbol->val);
                                return symbol->val;
                        } else {
                                fprintf(stderr, "Error: undefined symbol '%s'\n", node->token);
                                return 0;
                        }
                }
        } else if(node->type == INTERIOR) {
                printf("[DETECTED INTERIOR NODE]\n");
                interior_node_t * interior = (interior_node_t *)node->node;
                int left = eval_tree(interior->left);
                printf("\t[eval]: Evaluated left node\n");
                int right = eval_tree(interior->right);
                printf("\t[eval]: Evaluted right node\n");

                switch(interior->op) {
                        case ADD_OP:
                                printf("\t[eval]: add operation\n");
                                return left + right;
                        case SUB_OP:
                                printf("\t[eval]: sub operation\n");
                                return left - right;
                        case MUL_OP:
                                printf("\t[eval]: mul operation\n");
                                return left * right;
                        case DIV_OP:
                                printf("\t[eval]: div operation\n");
                                if(right == 0) {
                                        fprintf(stderr, "Error: division by zero\n");
                                        return 0;
                                }
                                return left / right;
                        case ASSIGN_OP:
                                printf("\t[eval]: assign operation\n");
                                if(interior->left->type == LEAF && ((leaf_node_t *)interior->left->node)->exp_type == SYMBOL) {
                                        symbol_t * symbol = lookup_table(interior->left->token);
                                        if(symbol != NULL) {
                                                symbol->val = right;
                                                return right;
                                        }
                                } else {
                                        fprintf(stderr, "Error: invalid left-hand side for assignment\n");
                                        return 0;
                                } break;
                        case Q_OP:
                                printf("\t[eval]: ternary operation\n");
                                int condition = left;
                                interior_node_t * r = (interior_node_t *)interior->right->node;
                                if(condition) return eval_tree(r->left);
                                else return eval_tree(r->right);

                        default:
                                fprintf(stderr, "Error: unknown operation type\n");
                                return 0;
                }
        }
        return 0;
}

/**
 * Prints the AST in human-readable infix notation.
 *
 * @param node: A pointer to the root of the AST
 */
void print_infix(tree_node_t * node) {
        if(node == NULL) return;

        if(node->type == LEAF) {
                printf("%s ", node->token);
        } else if(node->type == INTERIOR) {
                interior_node_t * interior = (interior_node_t *)node->node;
                printf("(");
                print_infix(interior->left);
                printf(" %s ", node->token);
                print_infix(interior->right);
                printf(")");
        }
}

/**
 * Frees memory associated with an AST
 *
 * @param node: A pointer to the root of the AST
 */
void cleanup_tree(tree_node_t * node) {
        if(node == NULL) return;

        if(node->type == INTERIOR) {
                interior_node_t * interior = (interior_node_t *)node->node;
                cleanup_tree(interior->left);
                cleanup_tree(interior->right);
                free(interior);
        } else if(node->type == LEAF) {
                if(node->token != NULL) {
                        free(node->token);
                        node->token = NULL;
                }
                free(node->node);
        }
        free(node);
        node = NULL;
}



