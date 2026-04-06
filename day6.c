#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  int type;
  int count; 
} Answer; 

void reset_answers(Answer * answers){
  for (int i = 0; i < 26; i++){
    answers[i] = (Answer){.type=i,
                          .count=0};
  }
}

int calc_group(Answer * answers, int group_count){
  int count = 0; 
  for (int i = 0; i < 26; i++){
    if (answers[i].count == 0) continue;
    if (answers[i].count != group_count) continue;
    count++; 
  }
  return count; 
}

void process_line(bool * answers, char * line, int line_index){
  int n = strlen(line);
  //  printf("n=%d\n", n);
  for (int i = 0; i < n; i++){
    int c = line[i];
    int index = c-97;
    answers[index] = true; 
  }
  if (line_index == 0) return;
}

void process_answer(Answer * answers, char * line){
  int n = strlen(line);
  for (int i = 0; i < n; i++){
    int c = line[i];
    int index = c-97;
    answers[index].count++; 
  }
}

int main(){
  FILE *fp = fopen("day6.tx", "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    char line[256];
    int sum = 0;
    Answer answers[26];
    reset_answers(answers);
    int group_count = 0; 
    while (fgets(line, sizeof(line), fp)) {
      /* strip trailing newline / carriage return */
      line[strcspn(line, "\r\n")] = '\0';
      //  printf("%s\n", line); 
      if (line[0] == '\0'){
        sum += calc_group(answers, group_count);
        group_count = 0; 
        reset_answers(answers);
      }else{
        process_answer(answers, line);
        group_count++; 
      }
    }
    if (group_count > 0){
      sum += calc_group(answers, group_count);
      group_count = 0;
      reset_answers(answers);
    }
    printf("sum=%d\n", sum);
    fclose(fp);
    
}
