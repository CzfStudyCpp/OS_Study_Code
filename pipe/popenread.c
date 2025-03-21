#include <stdio.h>
#include <stdlib.h>
 
#define BUFF_SIZE   1024
 
int main(void)
{
         FILE * file;
         char buff[BUFF_SIZE+1];
         int cnt;
 
         // system("ls -l > result.txt");
         file = popen("ls -l", "r");
         if (!file) {
                 printf("fopen failed!\n");
                 exit(1);
         }
 
         cnt = fread(buff, sizeof(char), BUFF_SIZE, file);
         if (cnt > 0) {
                 buff[cnt] = '\0';
                 printf("%s", buff);
         }        
 
         pclose(file);
 
         return 0;        
}