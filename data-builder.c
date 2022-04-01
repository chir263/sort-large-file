#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    srand(time(0));
    long long s = 0;
    printf("arg : ");
    scanf("%lld", &s);
    clock_t start, end;
    start = clock();
    FILE *fp = fopen("testaa", "w+");
    printf("started\n");
    for (long long i = 0; i < 1024 * 1024 * s; i++)
    {
        for (int j = 0; j < rand()%100 + 1; j++)
            fprintf(fp, "%c",(char) (97 + (rand()%26)));
        fprintf(fp, "\n");
    }
    fclose(fp);
    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("completed in %lfms\n", cpu_time_used * 1000);
    return 0;
}