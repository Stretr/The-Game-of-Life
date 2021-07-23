#include "universe.h"

#include <getopt.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Universe {
    int rows;
    int cols;
    bool **grid;
    bool toroidal;
};

Universe *uv_create(int rows, int cols, bool toroidal) {
    Universe *u = (Universe *) calloc(1, sizeof(Universe));
    if (!u) {
        return 0;
    }
    u->rows = rows;
    u->cols = cols;
    u->toroidal = toroidal;
    u->grid = (bool **) calloc(rows, sizeof(bool *));
    for (int i = 0; i < rows; i++) {
        u->grid[i] = (bool *) calloc(cols, (sizeof(bool)));
    }

    return u;
}

// MY FUNCTIONS

// Both functions return the looped location if t
// and an oob location (false) if flat
int left(Universe *u, int pos, int size) {
    if (u->toroidal) {
        return (pos + size - 1) % size;
    }
    return pos - 1;
}

int right(Universe *u, int pos, int size) {
    if (u->toroidal) {
        return (pos + size + 1) % size;
    }
    return pos + 1;
}

// END MY FUNCTIONS

void uv_delete(Universe *u) {
    for (int i = uv_rows(u) - 1; i >= 0; i--) {
        free(u->grid[i]);
    }
    free(u->grid);
    free(u);
    return;
}

int uv_rows(Universe *u) {
    return u->rows;
}

int uv_cols(Universe *u) {
    return u->cols;
}

void uv_live_cell(Universe *u, int r, int c) {
    if ((r >= 0 && r < uv_rows(u)) && (c >= 0 && c < uv_cols(u))) {
        u->grid[r][c] = true;
    }
    return;
}

void uv_dead_cell(Universe *u, int r, int c) {
    if ((r >= 0 && r < uv_rows(u)) && (c >= 0 && c < uv_cols(u))) {
        u->grid[r][c] = false;
    }
    return;
}

bool uv_get_cell(Universe *u, int r, int c) {
    if ((r >= 0 && r < uv_rows(u)) && (c >= 0 && c < uv_cols(u))) {
        return u->grid[r][c];
    }
    return false;
}

bool uv_populate(Universe *u, FILE *infile) {
    int r;
    int c;
    // returns false if any location is oob
    while (fscanf(infile, "%d %d\n", &r, &c) != EOF) {
        if ((r >= 0 && r < uv_rows(u)) && (c >= 0 && c < uv_cols(u))) {
            uv_live_cell(u, r, c);
        } else {
            return false;
        }
    }
    return true;
}

int uv_census(Universe *u, int r, int c) {
    int live = 0;
    int y = left(u, r, uv_rows(u));
    // there is only one for loop
    // to use the left and right functions
    for (int i = r - 1; i <= r + 1; i++) {
        if (uv_get_cell(u, i, left(u, c, uv_cols(u)))) {
            live += 1;
        }
        if (uv_get_cell(u, y, c) && i != r) {
            live += 1;
            // y is made right to check the row below r
        }
        if (uv_get_cell(u, i, right(u, c, uv_cols(u)))) {
            live += 1;
        }
        y = right(u, r, uv_rows(u));
    }
    return live;
}

void uv_print(Universe *u, FILE *outfile) {
    for (int i = 0; i < uv_rows(u); i++) {
        for (int j = 0; j < uv_cols(u); j++) {
            if (uv_get_cell(u, i, j)) {
                fprintf(outfile, "o");
            } else {
                fprintf(outfile, ".");
            }
        }
        fprintf(outfile, "\n");
    }
    return;
}