#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    uint8_t *buf;
    size_t   cap;
    size_t   pos;
} Arena;

Arena arena_create(size_t cap) {
    return (Arena){ .buf = malloc(cap), .cap = cap, .pos = 0 };
}

void arena_destroy(Arena *a) {
    free(a->buf);
    a->buf = NULL;
    a->pos = a->cap = 0;
}

void *arena_alloc(Arena *a, size_t size, size_t align) {
    size_t aligned = (a->pos + (align - 1)) & ~(align - 1);
    assert(aligned + size <= a->cap && "arena out of memory");
    a->pos = aligned + size;
    return a->buf + aligned;
}

#define ARENA_NEW(arena, T) ((T *)arena_alloc((arena), sizeof(T), _Alignof(T)))

typedef struct Node {
    int          start;
    int          end;
    struct Node *left;
    struct Node *right;
} Node;

static Node *init_node(Arena *a, int start, int end, Node *left, Node *right) {
    Node *n  = ARENA_NEW(a, Node);
    n->start = start;
    n->end   = end;
    n->left  = left;
    n->right = right;
    return n;
}

Node *build_tree(Arena *a, int val, int stack_capacity) {

    Node  *root       = init_node(a, 0, val - 1, NULL, NULL);
    Node **stack      = malloc((size_t)stack_capacity * sizeof(Node *));
    int    stack_size = 0;

    stack[stack_size++] = root;

    while (stack_size > 0) {
        Node *node          = stack[--stack_size];
        bool  is_range_less = (node->end - node->start) <= 1;

        if (is_range_less) {
            node->left  = init_node(a, node->start, node->start, NULL, NULL);
            node->right = init_node(a, node->end,   node->end,   NULL, NULL);
            continue;
        }

        int   m  = (node->start + node->end) / 2;
        Node *n1 = init_node(a, node->start, m,         NULL, NULL);
        Node *n2 = init_node(a, m + 1,       node->end, NULL, NULL);
        node->left  = n1;
        node->right = n2;

        assert(stack_size + 1 < stack_capacity);
        stack[stack_size++] = n1;

        assert(stack_size + 1 < stack_capacity);
        stack[stack_size++] = n2;
    }

    if (stack_size >= stack_capacity) {
        exit(1);
    }

    free(stack);
    return root;
}

typedef struct {
    int row;
    int col;
    int seat_id;
    int valid;  // 0 if parsing failed
} SeatResult;

SeatResult decode_seat(const char *path, Node *seat_rows, Node *seat_cols) {
    SeatResult result = {0, 0, 0, 0};

    if (!path || !seat_rows || !seat_cols) return result;

    int n = strlen(path);
    int i = 0;

    // Decode row via F/B
    Node *node_row = seat_rows;
    for (i = 0; i < n; i++) {
        char p = path[i];
        if (node_row == NULL)              return result;
        if (p != 'F' && p != 'B')         break;
        node_row = (p == 'F') ? node_row->left : node_row->right;
    }

    // Decode col via L/R
    Node *node_col = seat_cols;
    for (; i < n; i++) {
        char p = path[i];
        if (node_col == NULL)              return result;
        if (p != 'L' && p != 'R')         break;
        node_col = (p == 'R') ? node_col->right : node_col->left;
    }

    if (!node_row || !node_col) return result;

    result.row     = node_row->start;
    result.col     = node_col->start;
    result.seat_id = node_row->start * 8 + node_col->start;
    result.valid   = 1;
    return result;
}

int main(void) {
    printf("[DEBUG] main: starting\n");

    Arena a         = arena_create(1024 * 1024);
    Node *seat_rows = build_tree(&a, 128, 100);
    Node *seat_cols = build_tree(&a, 8, 20);

    FILE *fp = fopen("day5.txt", "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    int m = 0;

    char line[256];

    bool seat_state[128*8]; 
    while (fgets(line, sizeof(line), fp)) {
        /* strip trailing newline / carriage return */
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0') continue; /* skip blank lines */

        SeatResult seat = decode_seat(line, seat_rows, seat_cols);
        if (!seat.valid) {
            fprintf(stderr, "Failed to decode boarding pass\n");
        } else {
          //printf("row: %d, col: %d, seat_id: %d\n",
          //       seat.row, seat.col, seat.seat_id);
            seat_state[seat.seat_id] = true; 
            if (m < seat.seat_id) m = seat.seat_id;
        }
    }

    fclose(fp);
    printf("Max seat ID: %d\n", m);
    for (int i = 0; i < 128*8; i++)
      printf("seat: %d - %s\n", i, seat_state[i] ? "filled" : "no"); 
    arena_destroy(&a); 
    return 0;
}
