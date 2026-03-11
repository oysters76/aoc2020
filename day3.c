#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST "day3_test.txt"
#define ACT "day3.txt"

typedef struct {
  int right;
  int bottom;
} Vector2;

int compute(char **grid, int h, int w, int r, int b) {
  int x = 0;
  int y = 0;
  int count = 0;
  while (y < h) {
    x += r;
    y += b;
    if (x >= w) {
      printf("premature exit in x\n");
      exit(1);
    }
    if (y >= h) {
      printf("premature exit in y\n");
      break;
    }
    bool isTree = grid[y][x] == '#';
    count += isTree ? 1 : 0;
  }
  return count;
}

int main(void) {
  char *filename = ACT;
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "error opening file '%s'\n", filename);
    exit(1);
  }
  int w = 0;
  int h = 0;
  char line[256];
  while (fgets(line, sizeof(line), f) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    w = strlen(line);
    h++;
  }
  fclose(f);

  f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "error opening file '%s'\n", filename);
    exit(1);
  }

  int copy_size = w * 5;
  char **grid = malloc(h * sizeof(char *));
  int h_i = 0;

  while (fgets(line, sizeof(line), f) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    grid[h_i] = malloc((w * copy_size) * sizeof(char));
    for (int k = 0; k < copy_size; k++) {
      int w_i = (w * k);
      for (int i = 0; i < w; i++) {
        grid[h_i][w_i++] = line[i];
      }
    }
    h_i++;
  }

  fclose(f);
  printf("problem 1: %d\n", compute(grid, h, w * copy_size, 3, 1));

  Vector2 vecs[] = {{.right = 1, .bottom = 1},
                    {.right = 3, .bottom = 1},
                    {.right = 5, .bottom = 1},
                    {.right = 7, .bottom = 1},
                    {.right = 1, .bottom = 2}};
  long product = 1;
  for (int i = 0; i < 5; i++) {
    int c = compute(grid, h, w * copy_size, vecs[i].right, vecs[i].bottom);
    printf("\t%d\n", c);
    product *= c;
  }
  printf("problem 2: %ld\n", product);

  for (int i = 0; i < h; i++) {
    free(grid[i]);
  }
  free(grid);
}
