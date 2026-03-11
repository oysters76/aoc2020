#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX 256

typedef struct {
  char rule[MAX]; 
  char password[MAX]; 
} PasswordSegment;

typedef struct {
  int min;
  int max;
  char letter;
} PasswordRule; 

PasswordSegment get_password_segment(char* line){
  int n = strlen(line);
  int k = 0;
  bool isRule = true;

  PasswordSegment seg = {0}; 
  
  for (int i = 0; i < n; i++){
    char c = line[i];
    if (c == ':'){
      seg.rule[k++] = '\0';
      isRule = false;
      k = 0; 
    }else{
      if (isRule)
        seg.rule[k++] = c;
      else
        {
          if (c != ' '){
             seg.password[k++] = c;
          }
        } 
    }
  }
  seg.password[k++] ='\0';

  return seg; 
}

PasswordRule get_password_rule(PasswordSegment * seg){
  PasswordRule rule = {0}; 
  int r = sscanf(seg->rule, "%d-%d %c", &rule.min, &rule.max, &rule.letter);
  if (r != 3){
    printf("error: failed on %s\n", seg->password);
    exit(1); 
  }
  return rule; 
}

bool is_valid(PasswordSegment * seg, PasswordRule * rule){
  int count = 0; 
  for (int i = 0; i < strlen(seg->password); i++){
    count += seg->password[i] == rule->letter ? 1 : 0; 
  }
  return count >= rule->min && count <= rule->max;
}

bool is_valid_2(PasswordSegment * seg, PasswordRule * rule){
  char l1 = seg->password[rule->min-1];
  char l2 = seg->password[rule->max-1];
  bool m1 = l1 == rule->letter;
  bool m2 = l2 == rule->letter;
  if (!m1 && m2) return true;
  if (m1 && !m2) return true; 
  return false;
}

void act_problem(char * file_name, bool (*check)(PasswordSegment *, PasswordRule *)){
  FILE *f = fopen(file_name, "r");
  if (f == NULL){
    perror("error opening file 'day2.txt', please ensure problem statement exists");
    exit(1); 
  }
  char line[MAX];
  int count = 0; 
  while(fgets(line,sizeof(line), f) != NULL){
    PasswordSegment seg = get_password_segment(line);
    PasswordRule rule = get_password_rule(&seg); 
    count += check(&seg, &rule) ? 1 : 0; 
  }
  fclose(f);
  printf("result: %d\n", count); 
}

int main(void){
  act_problem("day2.txt", is_valid);
  act_problem("day2.txt", is_valid_2);
}
