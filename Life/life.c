#include "universe.h"

#include <getopt.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OPTIONS "tsn:i:o:"

int main(int argc, char **argv) {
    // Neccessary initial files
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int opt;
    int gens = 100;
    bool toro = false;
    bool curses = true;
    int rows = 0;
    int cols = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't': toro = true; break;
        case 's': curses = false; break;
        case 'n':
            gens = atoi(optarg);
            if (gens < 0) {
                printf("Malformed input.\n");
                return 0;
            }
            break;
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                printf("Malformed input.\n");
                return 0;
            }
            break;
        case 'o': outfile = fopen(optarg, "w"); break;
        }
    }

    if (argc == 1) {
        printf("No input...\n");
        return 0;
    }

    // takes input for row and column
    // and initialises universes U and V
    fscanf(infile, "%d %d\n", &rows, &cols);
    Universe *u = uv_create(rows, cols, toro);
    Universe *v = uv_create(rows, cols, toro);
    Universe *temp = u;
    if (uv_populate(u, infile) == false) {
        printf("Malformed input.\n");
        return 0;
    }
    // loads screen and starts game
    initscr();
    noecho();
    curs_set(FALSE);
    for (int g = 0; g < gens; g++) {
        if (curses) {
            clear();
            for (int i = 0; i < uv_rows(u); i++) {
                for (int j = 0; j < uv_cols(u); j++) {
                    if (uv_get_cell(u, i, j)) {
                        mvprintw(i, j, "o");
                    } else {
                        mvprintw(i, j, " ");
                    }
                }
            }
            refresh();
            usleep(50000);
        }
        // Passes generation and stores in v
        for (int i = 0; i < uv_rows(u); i++) {
            for (int j = 0; j < uv_cols(u); j++) {
                int group = uv_census(u, i, j);
                if (group < 2 || group > 3) {
                    uv_dead_cell(v, i, j);
                } else if (uv_get_cell(u, i, j) || group == 3) {
                    uv_live_cell(v, i, j);
                } else {
                    uv_dead_cell(v, i, j);
                }
            }
        }
        temp = u;
        u = v;
        v = temp;
    }
    // closes display and frees space
    endwin();
    uv_print(u, outfile);
    uv_delete(temp);
    uv_delete(u);
    fclose(stdin);
    fclose(stdout);
    printf("\n");
    return 0;
}
