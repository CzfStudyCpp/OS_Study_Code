#include <stdio.h>
#include <stdlib.h>

int main(void)
{       int ret = 0;
        char buff[80] = {0,};

        ret = scanf("%s", buff);
        printf("[ret: %d]buff=%s\n", ret, buff);

        ret = scanf("%s", buff);
        printf("[ret: %d]buff=%s\n", ret, buff);
        return 0;
}
