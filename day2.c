#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULE 128
#define MAX_PASSWORD 256

typedef struct {
  char rule[MAX_RULE];
  char password[MAX_PASSWORD];
} PasswordSegment;

typedef struct {
  int min;
  int max;
  char letter;
} PasswordRule;

bool in_bounds(int min, int max, int v) { return v >= min && v <= max; }

PasswordSegment get_password_segment(char *line) {
  int n = strlen(line);
  if (!in_bounds(0, (MAX_RULE + MAX_PASSWORD) - 1, n)) {
    printf("password '%s' too long\n", line);
    exit(1);
  }
  int k = 0;
  bool isRule = true;

  PasswordSegment seg = {0};

  for (int i = 0; i < n; i++) {
    char c = line[i];
    if (c == ':') {
      seg.rule[k++] = '\0';
      isRule = false;
      k = 0;
    } else {
      if (isRule && in_bounds(0, MAX_RULE - 1, k))
        seg.rule[k++] = c;
      else {
        if (c != ' ') {
          if (in_bounds(0, MAX_PASSWORD - 1, k)) {
            seg.password[k++] = c;
          } else {
            fprintf(stderr, "error copying password: %s, buffer full", line);
            exit(1);
          }
        }
      }
    }
  }
  seg.password[k] = '\0';

  return seg;
}

PasswordRule get_password_rule(PasswordSegment *seg) {
  PasswordRule rule = {0};
  int r = sscanf(seg->rule, "%d-%d %c", &rule.min, &rule.max, &rule.letter);
  if (r != 3) {
    printf("error: failed on rule:%s, password: %s\n", seg->rule,
           seg->password);
    exit(1);
  }
  return rule;
}

bool is_valid(PasswordSegment *seg, PasswordRule *rule) {
  int count = 0;
  int n = strlen(seg->password);
  for (int i = 0; i < n; i++) {
    count += seg->password[i] == rule->letter ? 1 : 0;
  }
  return in_bounds(rule->min, rule->max, count);
}

bool is_valid_2(PasswordSegment *seg, PasswordRule *rule) {
  int len = strlen(seg->password);
  if (rule->min < 1 || rule->max < 1) return false;
  if (rule->min - 1 >= len || rule->max - 1 >= len) return false;
  char l1 = seg->password[rule->min - 1];
  char l2 = seg->password[rule->max - 1];
  return (l1 == rule->letter) != (l2 == rule->letter);
}

void act_problem(char *file_name,
                 bool (*check)(PasswordSegment *, PasswordRule *)) {
  FILE *f = fopen(file_name, "r");
  if (f == NULL) {
    fprintf(stderr, "error opening file '%s'\n", file_name);
    exit(1);
  }
  char line[MAX_RULE + MAX_PASSWORD];
  int count = 0;
  while (fgets(line, sizeof(line), f) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    PasswordSegment seg = get_password_segment(line);
    PasswordRule rule = get_password_rule(&seg);
    count += check(&seg, &rule) ? 1 : 0;
  }
  fclose(f);
  printf("result: %d\n", count);
}

int main(void) {
  act_problem("day2.txt", is_valid);
  act_problem("day2.txt", is_valid_2);
}
