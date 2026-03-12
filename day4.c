#include <regex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 256
#define KEY_COUNT 8

#define BYR 0
#define IYR 1
#define EYR 2
#define HGT 3
#define HCL 4
#define ECL 5
#define PID 6
#define CID 7

#define IS_PROBLEM_2 true
#define RUN_TESTS false
#define VERBOSE false
#define EXTRA_VERBOSE false

// Memory alignment
typedef struct {
  char key[SIZE];             // char array
  char val[SIZE];             // char array
  bool key_state[KEY_COUNT];  // bool array
  int valid_passports;        // 4 bytes
  int k_i;                    // 4 bytes
  int v_i;                    // 4 bytes
  int key_count;
  bool is_key;  // 1 byte
} State;

typedef void (*on_char_fn)(char c);
typedef void (*on_newline_fn)(char c);
void print_state(const State *);

char *keys[] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid", "cid"};

State state = {0};

bool check_year(char *val, int min, int max) {
  if (strlen(val) != 4) return false;
  for (int i = 0; i < 4; i++) {
    if (val[i] < '0' || val[i] > '9') return false;
  }
  int year = atoi(val);
  return year >= min && year <= max;
}

bool check_regex(const char *val, const char *pattern) {
  regex_t re;
  int result = regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB);
  if (result) return false;
  result = regexec(&re, val, 0, NULL, 0);
  regfree(&re);
  return result == 0;
}

bool check_in_group(const char *val, ...) {
  va_list args;
  va_start(args, val);
  const char *option;
  while ((option = va_arg(args, const char *)) != NULL) {
    if (strcmp(val, option) == 0) {
      va_end(args);
      return true;
    }
  }
  va_end(args);
  return false;
}

bool check_height(const char *val) {
  int n;
  char unit[3];
  if (sscanf(val, "%d%2s", &n, unit) != 2) return false;
  if (strcmp(unit, "cm") == 0) return n >= 150 && n <= 193;
  if (strcmp(unit, "in") == 0) return n >= 59 && n <= 76;
  return false;
}

bool is_value_valid(int key, char *val) {
  if (!IS_PROBLEM_2) return true;
  bool result = false;
  switch (key) {
    case BYR:
      result = check_year(val, 1920, 2002);
      break;
    case IYR:
      result = check_year(val, 2010, 2020);
      break;
    case EYR:
      result = check_year(val, 2020, 2030);
      break;
    case HGT:
      result = check_height(val);
      break;
    case HCL:
      result = check_regex(val, "^#[0-9a-f]{6}$");
      break;
    case ECL:
      result = check_in_group(val, "amb", "blu", "brn", "gry", "grn", "hzl",
                              "oth", NULL);
      break;
    case PID:
      result = check_regex(val, "^[0-9]{9}$");
      break;
    case CID:
      result = true;
      break;
  }
  if (result && VERBOSE) {
    printf("key:%s val:%s\n", keys[key], val);
  }
  return result;
}

void on_char_event(char c) {
  if (c == ':') {
    state.key[state.k_i++] = '\0';
    state.is_key = false;
    state.k_i = 0;
  } else if (c == ' ' || c == '\n') {
    state.val[state.v_i++] = '\0';
    for (int i = 0; i < KEY_COUNT; i++) {
      if (strcmp(keys[i], state.key) == 0) {
        state.key_state[i] = is_value_valid(i, state.val);
        break;
      }
    }
    state.is_key = true;
    state.v_i = 0;
  } else {
    if (state.is_key) {
      state.key[state.k_i++] = c;
    } else {
      state.val[state.v_i++] = c;
    }
  }
  if (EXTRA_VERBOSE) print_state(&state);
}

void print_state(const State *s) {
  printf("┌─────────────────────────────┐\n");
  printf("│           STATE             │\n");
  printf("├─────────────────────────────┤\n");
  printf("│ key:             %-10s  │\n", s->key);
  printf("│ val:             %-10s  │\n", s->val);
  printf("│ is_key:          %-10s  │\n", s->is_key ? "true" : "false");
  printf("│ k_i:             %-10d  │\n", s->k_i);
  printf("│ v_i:             %-10d  │\n", s->v_i);
  printf("│ key_count:       %-10d  │\n", s->key_count);
  printf("│ valid_passports: %-10d  │\n", s->valid_passports);
  printf("├─────────────────────────────┤\n");
  printf("│ key_state:                  │\n");
  for (int i = 0; i < KEY_COUNT; i++) {
    printf("│   %-4s -> %s              │\n", keys[i],
           s->key_state[i] ? "✓" : "✗");
  }
  printf("└─────────────────────────────┘\n");
}

void reset_state() {
  state.is_key = true;
  state.key_count = 0;
  state.k_i = 0;
  state.v_i = 0;
  for (int i = 0; i < KEY_COUNT; i++) {
    state.key_state[i] = false;
  }
}

void on_newline_event(char c) {
  if (VERBOSE) print_state(&state);
  bool is_ok = true;
  for (int i = 0; i < KEY_COUNT; i++) {
    if (i == CID) {
      continue;
    }
    is_ok &= state.key_state[i];
  }
  if (is_ok) {
    state.valid_passports += 1;
  }
  reset_state();
}

/* File reader that fires events */
int read_file_with_events(const char *path, on_char_fn on_char,
                          on_newline_fn on_newline) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  int ch;
  char prev = 0;

  while ((ch = fgetc(fp)) != EOF) {
    char c = (char)ch;
    if (c == '\n' && prev == '\n') {
      if (on_newline) on_newline(c);
    } else {
      if (on_char) on_char(c);
    }
    prev = c;
  }

  fclose(fp);
  return 0;
}

void assert_bool(const char *label, bool result, bool expected, int *passed,
                 int *failed) {
  if (result == expected) {
    printf("  ✓ %s\n", label);
    (*passed)++;
  } else {
    printf("  ✗ %s (expected %s, got %s)\n", label, expected ? "true" : "false",
           result ? "true" : "false");
    (*failed)++;
  }
}

void run_tests() {
  int passed = 0, failed = 0;

  printf("\n── check_year ──────────────────────\n");
  assert_bool("valid: 1920 in [1920,2002]", check_year("1920", 1920, 2002),
              true, &passed, &failed);
  assert_bool("valid: 2002 in [1920,2002]", check_year("2002", 1920, 2002),
              true, &passed, &failed);
  assert_bool("invalid: 1919 below min", check_year("1919", 1920, 2002), false,
              &passed, &failed);
  assert_bool("invalid: 20021 too long", check_year("20021", 1920, 2002), false,
              &passed, &failed);

  printf("\n── check_regex (hcl) ───────────────\n");
  assert_bool("valid: #a1b2c3", check_regex("#a1b2c3", "^#[0-9a-f]{6}$"), true,
              &passed, &failed);
  assert_bool("valid: #000000", check_regex("#000000", "^#[0-9a-f]{6}$"), true,
              &passed, &failed);
  assert_bool("invalid: #xyz123", check_regex("#xyz123", "^#[0-9a-f]{6}$"),
              false, &passed, &failed);
  assert_bool("invalid: #12345", check_regex("#12345", "^#[0-9a-f]{6}$"), false,
              &passed, &failed);

  printf("\n── check_regex (pid) ───────────────\n");
  assert_bool("valid: 000000001", check_regex("000000001", "^[0-9]{9}$"), true,
              &passed, &failed);
  assert_bool("valid: 123456789", check_regex("123456789", "^[0-9]{9}$"), true,
              &passed, &failed);
  assert_bool("invalid: 12345678", check_regex("12345678", "^[0-9]{9}$"), false,
              &passed, &failed);
  assert_bool("invalid: 00000000a", check_regex("00000000a", "^[0-9]{9}$"),
              false, &passed, &failed);

  printf("\n── check_in_group (ecl) ────────────\n");
  assert_bool("valid: amb",
              check_in_group("amb", "amb", "blu", "brn", "gry", "grn", "hzl",
                             "oth", NULL),
              true, &passed, &failed);
  assert_bool("valid: oth",
              check_in_group("oth", "amb", "blu", "brn", "gry", "grn", "hzl",
                             "oth", NULL),
              true, &passed, &failed);
  assert_bool("invalid: xxx",
              check_in_group("xxx", "amb", "blu", "brn", "gry", "grn", "hzl",
                             "oth", NULL),
              false, &passed, &failed);
  assert_bool(
      "invalid: empty",
      check_in_group("", "amb", "blu", "brn", "gry", "grn", "hzl", "oth", NULL),
      false, &passed, &failed);

  printf("\n── check_height ────────────────────\n");
  assert_bool("valid: 150cm", check_height("150cm"), true, &passed, &failed);
  assert_bool("valid: 76in", check_height("76in"), true, &passed, &failed);
  assert_bool("invalid: 149cm", check_height("149cm"), false, &passed, &failed);
  assert_bool("invalid: 77in", check_height("77in"), false, &passed, &failed);

  printf("\n────────────────────────────────────\n");
  printf("  passed: %d  failed: %d\n\n", passed, failed);
}

int main(int argc, char *argv[]) {
  if (RUN_TESTS) {
    run_tests();
    return 0;
  }
  const char *path = argc > 1 ? argv[1] : "input.txt";
  reset_state();
  state.valid_passports = 0;
  read_file_with_events(path, on_char_event, on_newline_event);
  printf("valid passports: %d\n", state.valid_passports);
}
