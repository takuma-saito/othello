
/***********************************************
 * search.h のハッシュテーブルを試すスクリプト *
 ***********************************************/

#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1024
#define BUFF_SIZE 128

int main(void) {
  char line[MAXLINE];
  ENTRY item;
  ENTRY *found_item;
  printf("usage:\nenter >> <key> <value>\nenter finish\ninput <key>\n\n");

  hcreate(30);
  while (1) {
    char key[BUFF_SIZE], data[BUFF_SIZE];
    printf("key value >> ");
    fgets(line, MAXLINE, stdin);
    line[strlen(line) - 1] = '\0';
    if (strcmp(line, "quit") == 0 || strcmp(line, "q") == 0) exit(0);
    if (strcmp(line, "fin") == 0 || strcmp(line, "finish") == 0) break;
    sscanf(line, "%s %s", key, data);
    item.key = strdup(key);
    item.data = strdup(data);
    hsearch(item, ENTER);
  }
  while (1) {
    printf("key >> ");
    fgets(line, MAXLINE, stdin);
    line[strlen(line) - 1] = '\0';
    if (strcmp(line, "quit") == 0 || strcmp(line, "q") == 0) exit(0);
    item.key = line;
    if ((found_item = hsearch(item, FIND)) != NULL) {
      printf("key: %s, value:%s\n", (char *)found_item->key, (char *)found_item->data);
    }
    else {
      printf("no such key\n");
    }
  }
  hdestroy();
  return 0;
}
