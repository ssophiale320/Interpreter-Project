#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main() {
        stack_t * stk = make_stack();

        int * x = malloc(sizeof(int));
        *x = 10;

        int * y = malloc(sizeof(int));
        *y = 20;

        push(stk, x);
        push(stk, y);

        printf("Top: %d\n", *(int *)top(stk));

        pop(stk);

        printf("Top after pop: %d\n", *(int *)top(stk));

        pop(stk);
        free(x);
        free(y);
        free_stack(stk);

        printf("TEST STACK SUCCESSFUL\n");
        return 0;
}
