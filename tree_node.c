**
 * Implementation of tree node structures for expression parsing and evaluation.
 * This file provides functions to create and manage tree nodes used in binary trees.
 * The nodes are categorized into *interior* nodes, representing operators, and *leaf* nodes,
 * representing operands or constants. The binary tree is utilized to structure and evaluate
 * mathematical or logical expressions.
 *
 * ## Tree Node Types:
 * - **Interior Node**: Contains an operator and two child nodes (left and right)
 * - **Leaf Node**: Contains an expression type and a token (e.g., variable name or constant)
 *
 * @file        tree_node.c
 * @author      Sophia Le (sel5881@rit.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree_node.h"

/**
 * Creates an interior tree node. Interior nodes are used to represent operations
 * in expression trees.
 *
 * @param op: The operator type (e.g., ADD, SUBTRACT, etc.)
 * @param token: The string representation of the operator
 * @param left: Pointer to the left child node
 * @param right: Pointer to the right child node
 * @return: A pointer to the newly created interior node, or NULL if an error occurs
 */
tree_node_t * make_interior(op_type_t op, char * token, tree_node_t * left, tree_node_t * right) {
        if(!left || !right) {
                fprintf(stderr, "Error: NULL left or right node passed to make_interior()\n");
                return NULL;
        }

        tree_node_t * node = malloc(sizeof(tree_node_t));

        if(node == NULL) return NULL;

        interior_node_t * interior = malloc(sizeof(interior_node_t));

        if(interior == NULL) {
                free(node);
                return NULL;
        }

        if(!token || token[0] == '\0') {
                fprintf(stderr, "[make_interior]: received invalid or empty token\n");
                free(interior);
                free(node);
                return NULL;
        }
        printf("[make_interior]: Tok = %s\n", token);
        node->token = strdup(token);
        if(!node->token) {
                fprintf(stderr,"[make_interior]: strdup failed for token '%s'\n", token);
                free(interior);
                free(node);
                return NULL;
        }

        interior->op = op;
        interior->left = left;
        interior->right = right;

        node->type = INTERIOR;
        node->node = interior;
        printf("\t[make_interior]: Created interior node: op='%d', token='%s'\n", op, node->token);
        return node;
}

/**
 * Creates a leaf tree node. Leaf nodes are used to represent constants or
 * variable names in expression trees. They have no children.
 *
 * @param exp_type: The expression type (e.g., CONSTANT, VARIABLE, etc.)
 * @param token: The string representation of the constant or variable
 * @return: A pointer to the newly created leaf node, or NULL if an error occurs
 */
tree_node_t * make_leaf(exp_type_t exp_type, char * token) {
        tree_node_t * node = (tree_node_t *)malloc(sizeof(tree_node_t));

        if(node == NULL) {
                fprintf(stderr, "Failed to allocate memory for tree node\n");
                return NULL;
        }

        leaf_node_t * leaf = malloc(sizeof(leaf_node_t));
//      printf("\t[make_leaf]: Allocated tree_node_t at %p\n", (void*)node);
        if(leaf == NULL) {
                fprintf(stderr, "Failed to allocate memory for leaf node\n");
                free(node);
                return NULL;
        }
//      printf("\t[make_leaf]: Allocated leaf_node_t at %p\n", (void *)leaf);
        leaf->exp_type = exp_type;

        node->type = LEAF;
        node->token = strdup(token);
        if(node->token == NULL) {
                fprintf(stderr, "Failed to duplicate token\n");
                free(node);
                return NULL;
        }
//      printf("\t[make_leaf]: Duplicated token '%s' at %p\n", node->token, (void *)node->token);
        node->node = NULL;
        printf("\t[make_leaf]: SUCCESSFULLY CREATED LEAF NODE\n");
        return node;
}
