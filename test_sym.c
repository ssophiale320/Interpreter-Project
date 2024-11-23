#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void print_symbol(symbol_t * symbol) {
        if(symbol) {
                printf("Found: Name = %s, Value = %d\n", symbol->var_name, symbol->val);
        } else {
                printf("Not found\n");
        }
}

int main() {
        build_table("sym.txt");
        printf("Initial Symbol Table:\n");
        dump_table();

        printf("\nAdding new symbol 'testVar' with value 42...\n");
        add_symbol("testVar", 42);
        dump_table();

        printf("\nLooking up 'myVar': ");
        print_symbol(lookup_table("myVar"));

        printf("Looking up 'nonExistentVar': ");
        print_symbol(lookup_table("nonExistentVar"));

        printf("\nAdding duplicate symbol 'myVar' with value 99...\n");
        add_symbol("myVar", 99);
        dump_table();

        printf("Looking up 'myVar' after re-adding: ");
        print_symbol(lookup_table("myVar"));

        printf("\nCurrent Symbol Table:\n");
        dump_table();

        printf("\nFreeing symbol table...\n");
        free_table();
        printf("Symbol table operations completed.\n");

        return 0;
}
