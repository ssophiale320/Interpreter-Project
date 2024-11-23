
/**
 * Implementation of a simple stack data structure. This file
 * provides basic stack operations such as creation, push, pop,
 * top, and destruction. The stack operates on generic data,
 * allowing the storage of any data type. The stack is implemented
 * as a singly linked list.
 *
 * @file        stack.c
 * @author      Sophia Le (sel5881@rit.edu)
 */
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

/**
 * Creates a new, empty stack
 *
 * @return: A pointer to the newly created stack. Exits the program
 *      if memory allocation fails
 */
stack_t * make_stack(void) {
        stack_t * stk = (stack_t *)malloc(sizeof(stack_t));

        if(!stk) {
                perror("Failed to create stack.");
                exit(EXIT_FAILURE);
        }

        stk->top = NULL;
        return stk;
}

/**
 * Pushes data onto the stack.
 *
 * @param stack: A pointer to the stack
 * @param data: A pointer to the data to be pushed onto the stack
 */
void push(stack_t * stack, void * data) {
        if(!stack) {
                fprintf(stderr, "push: stack is NULL\n");
                exit(EXIT_FAILURE);
        }
        if(!data) {
                fprintf(stderr, "Warning: attempted to push NULL data\n");
                exit(EXIT_FAILURE);
        }

        printf("\t[push]: Pushing %s...\n", (char *)data);
        if(data != NULL && ((char *)data)[0] == '\0') {
                fprintf(stderr, "Warning: attempted to push an empty string\n");
                return;
        }
        stack_node_t * node = (stack_node_t *)malloc(sizeof(stack_node_t));

        if(!node) {
                perror("Failed to push to stack.");
                exit(EXIT_FAILURE);
        }

        node->data = data;
        node->next = stack->top;
        stack->top = node;

        //prints the stack
        printf("\t[push]: Stack after push: ");
        stack_node_t * curr = stack->top;
        while(curr) {
                printf("%s", (char *)curr->data);
                if(curr->next) printf(", ");
                curr = curr->next;
        }
        printf("\n");
}

/**
 * Retrieves the data at the top of the stack without
 * removing it.
 *
 * @param stack: A pointer to the stack
 * @return: A pointer to the data at the top of the stack
 */
void * top(stack_t * stack) {
        if(empty_stack(stack)) {
                fprintf(stderr, "top: stack is empty\n");
                exit(EXIT_FAILURE);
        }
        return stack->top->data;
}

/**
 * Removes the top element of the stack.
 *
 * @param stack: A pointer to the stack
 */
void pop(stack_t * stack) {
        if(empty_stack(stack)) {
                fprintf(stderr, "pop: stack is empty\n");
                exit(EXIT_FAILURE);
        }

        stack_node_t * tmp = stack->top;
        stack->top = tmp->next;

        free(tmp);
        tmp = NULL;

        printf("\t[pop]: Stack after pop: ");
        stack_node_t * curr = stack->top;
        while(curr) {
                printf("%s", (char *)curr->data);
                if(curr->next) printf(", ");
                curr = curr->next;
        }
        printf("\n");
}

/**
 * Checks if the stack is empty
 *
 * @param stack: A pointer to the stack
 * @return: 1 if the stack is empty, 0 otherwise
 */
int empty_stack(stack_t * stack) {
        return stack == NULL || stack->top == NULL;
}

/**
 * Frees all memory associated with the stack.
 *
 * @param stack: A pointer to the stack
 */
void free_stack(stack_t * stack) {
        if(stack == NULL) return;

        stack_node_t *cur = stack->top;
        while(cur->next != NULL) {
                printf("cur: %p, next: %p\n", cur, cur->next);
                stack_node_t *tmp = cur->next;
                if(cur->data != NULL) {
                        free(cur->data);
                        cur->data = NULL;
                }
                free(cur);
                cur = tmp;
        }
}


