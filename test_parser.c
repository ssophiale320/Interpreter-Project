#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "tree_node.h"
#include "parser.h"

void test_parse_int() {
        stack_t * stk = make_stack();
        push(stk, strdup("3"));
        tree_node_t * result = parse(stk);

        if(result != NULL && result->type == LEAF && result->node != NULL) {
                leaf_node_t * leaf = (leaf_node_t *)result->node;
                if(leaf->exp_type == INTEGER) {
                        printf("Test Successful: Parsed integer: %s\n\n", result->token);
                } else {
                        printf("Test Failed: Expected INTEGER type\n");
                }
        } else {
                printf("Test Failed: Failed to parse integer.\n");
        }
        free_stack(stk);
        cleanup_tree(result);
}

void test_parse_sym() {
        stack_t *stk = make_stack();
        push(stk, strdup("x"));
        tree_node_t * result = parse(stk);

        if(result != NULL && result->type == LEAF && result->node != NULL) {
                leaf_node_t * leaf = (leaf_node_t *)result->node;
                if(leaf->exp_type == SYMBOL) {
                        printf("Test Successful: Parsed symbol: %s\n\n", result->token);
                } else {
                        printf("Test Failed: Expected SYMBOL type\n");
                }
        } else {
                printf("Test Failed: Failed to parse symbol.\n");
        }
        free_stack(stk);
        cleanup_tree(result);
}

void test_arithmetic() {
        stack_t *stk = make_stack();
        push(stk, strdup("3"));
        push(stk, strdup("4"));
        push(stk, strdup("+"));
        tree_node_t * result1 = parse(stk);

        if(result1 != NULL) printf("\tTest Successful: Parsed basic arithmetic expression\n");
        else printf("\tTest Failed: Failed to parse basic arithmetic expression\n");

        free_stack(stk);
        cleanup_tree(result1);
}

void test_ternary() {
        stack_t *stk = make_stack();
        push(stk, strdup("1"));
        push(stk, strdup("3"));
        push(stk, strdup("5"));
        push(stk, strdup("?"));

        tree_node_t *result = parse(stk);
        if(result != NULL && result->type == INTERIOR && result->node != NULL) {
                interior_node_t *ternary = (interior_node_t *)result->node;
                if(ternary->op == Q_OP) printf("\tTest Successful: Parsed ternary expression\n");
                else printf("\tTest Failed: Expected TERNARY_OP type\n");
        } else {
                printf("\tTest Failed: Failed to parse ternary expression\n");
        }
        free_stack(stk);
        cleanup_tree(result);
}

void test_eval() {
        stack_t *stk = make_stack();
        push(stk, strdup("5"));
        push(stk, strdup("3"));
        push(stk, strdup("+"));

        tree_node_t * n1 = parse(stk);
        int result1 = eval_tree(n1);
        printf("Result: %d\n", result1);
        if(result1 != 8) printf("Test Successful: Result of '5 + 3' = %d\n", result1);
        else printf("Test Failed for eval\n");
        free_stack(stk);
        cleanup_tree(n1);
}

int main() {
        printf("Testing for integer parsing...\n");
        test_parse_int();
        printf("Testing for symbol parsing...\n");
        test_parse_sym();
        printf("Testing for arithmetic expression parsing...\n");
        test_arithmetic();
        printf("Testing for ternary parsing...\n");
        test_ternary();

        printf("Testing eval...\n");
        test_eval();

        return 0;
}

