#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_NAME_LEN 110
#define ll long long
#define file1 1
#define file2 0
#define RESULT_ERROR (-1)

// 750Mb loading will use around 2.2Gb-3.5Gb RAM will a bit fast
ll halfGB = 805306368;
ll INIT_ESTIMATE = (1024 * 1024 * 128);
ll INCREMENTAL_SIZE = (1024 * 1024 * 64);

char **str;
ll num = 0;
ll currMaxSize = (1024 * 1024 * 128);
ll fileByteCounter = 0;
ll fileSize = 0;

void init()
{
    str = (char **)malloc(INIT_ESTIMATE * sizeof(char *));
    printf("Inital Memory Allocated\n");
}

void add(char *name)
{
    if (num >= currMaxSize)
    {
        ll temp = currMaxSize;
        currMaxSize += INCREMENTAL_SIZE;
        str = (char **)realloc(str, currMaxSize * sizeof(char *));
        printf("Heap Incremented\n");
    }
    str[num] = (char *)malloc(strlen(name) + 1);
    fileByteCounter += strlen(name) + 1;
    strcpy(str[num++], name);
}

int myCompare(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void sortMergeFile(char *file__1, char *file__2, char *file__out)
{
    FILE *f1 = fopen(file__1, "r");
    FILE *f2 = fopen(file__2, "r");
    FILE *fileOut = fopen(file__out, "w+");
    bool file1_done = false;
    bool file2_done = false;
    char tempStr[MAX_NAME_LEN];
    char tempStr1[MAX_NAME_LEN] = {'\0'};
    char tempStr2[MAX_NAME_LEN] = {'\0'};
    int access = file1;
    int asa = 1;
    fscanf(f2, "%s", tempStr2);

    while (!file1_done || !file2_done)
    {
        char temp[MAX_NAME_LEN];
        int ff2 = (!file2_done && !access) ? fscanf(f2, "%s", tempStr2) : -3;
        int ff1 = (!file1_done && access) ? fscanf(f1, "%s", tempStr1) : -3;

        if (ff1 == EOF)
        {
            file1_done = true;
            access = file2;
        }
        if (ff2 == EOF)
        {
            file2_done = true;
            access = file1;
        }
        if (!file1_done && file2_done)
        {
            fprintf(fileOut, "%s\n", tempStr1);
            continue;
        }
        if (file1_done && !file2_done)
        {
            fprintf(fileOut, "%s\n", tempStr2);
            continue;
        }
        if (!file1_done && !file2_done)
        {
            if (strcmp(tempStr1, tempStr2) < 0)
            {
                fprintf(fileOut, "%s\n", tempStr1);
                access = file1;
            }
            else
            {
                fprintf(fileOut, "%s\n", tempStr2);
                access = file2;
            }
        }
    }
    fclose(fileOut);
}

ll getFileSize(char *fileName)
{
    struct stat fileStatus;
    if (stat(fileName, &fileStatus) < 0)
        return RESULT_ERROR;
    return fileStatus.st_size;
}

void freeRes()
{
    for (ll v = 0; v < num; v++)
        free(str[v]);
    free(str);
    num = 0;
    printf("Resources freed\n");
}

char tempName__[MAX_NAME_LEN];

void sortFile(char *fileName, char *outFileName)
{
    ll num_of_files = (fileSize + halfGB - 1) / halfGB;
    FILE *f1 = fopen(fileName, "r");
    for (int i = 0; i < num_of_files; i++)
    {
        clock_t start, end;
        start = clock();
        init();
        while (fscanf(f1, "%s", tempName__) != EOF)
        {
            add(tempName__);
            ll tttt = (ll)halfGB * (i + 1);
            // printf("%lld\n", fileByteCounter);
            if (fileByteCounter >= tttt)
            {
                break;
            }
        }
        printf("Processed till > %lld\n", fileByteCounter);
        char snum[5];
        sprintf(snum, "%d", i);
        char tempFileName[MAX_NAME_LEN];
        strcpy(tempFileName, outFileName);
        strcat(tempFileName, snum);
        FILE *f2 = fopen(tempFileName, "w+");

        qsort(str, num, sizeof(const char *), myCompare);

        for (ll i = 0; i < num; i++)
            fprintf(f2, "%s\n", str[i]);
        end = clock();
        double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("%s CREATED in %lfs\n", tempFileName, cpu_time_used);
        fclose(f2);
        freeRes();
    }
}

int main(int argv, char **argc)
{
    if (argv != 4)
        return 0;

    if (strcmp(argc[3], "250") == 0)
    {
        /* //250Mb loading will use around 600Mb-1.2Gb RAM will a bit slow*/
        halfGB = 268435456;
        INIT_ESTIMATE = (1024 * 1024 * 64);
        INCREMENTAL_SIZE = (1024 * 1024 * 32);
        currMaxSize = (1024 * 1024 * 64);
    }
    else if (strcmp(argc[3], "750") == 0)
    {
    }
    else
        return 0;

    clock_t start, end;
    printf("STARTED>>>\n");
    start = clock();

    fileSize = getFileSize(argc[1]);
    printf("FileSize > %lld bytes\n", fileSize);

    if (fileSize <= halfGB)
    {
        sortFile(argc[1], "OUTPUT_SORTED_FILE_FINAL");
        rename("OUTPUT_SORTED_FILE_FINAL0", argc[2]);
    }
    else if (fileSize > halfGB)
    {
        ll num_of_files = (fileSize + halfGB - 1) / halfGB;
        printf("WILL BREAK INTO %lld FILES\n", num_of_files);
        sortFile(argc[1], "TEMP_SORTED_FILE");

        // MERGING FILES
        printf("MERGING STARTED\n");
        while (1)
        {
            ll temp = (num_of_files) / 2;
            ll temp2 = num_of_files;
            num_of_files = (num_of_files + 1) / 2;
            printf("MERGING %lld TO %lld FILES....\n", temp2, num_of_files);
            for (int i = 0; i < temp; i++)
            {
                char snum[5];
                sprintf(snum, "%d", 2 * i);
                char snum1[5];
                sprintf(snum1, "%d", 2 * i + 1);
                char tempFileName[MAX_NAME_LEN];
                char tempFileName1[MAX_NAME_LEN];
                strcpy(tempFileName, "TEMP_SORTED_FILE");
                strcat(tempFileName, snum);
                strcpy(tempFileName1, "TEMP_SORTED_FILE");
                strcat(tempFileName1, snum1);

                char snum3[5];
                sprintf(snum3, "%d", i);
                char tempFileName2[MAX_NAME_LEN];
                strcpy(tempFileName2, "TEMP_SORTED_FILE");
                strcat(tempFileName2, snum3);

                sortMergeFile(tempFileName, tempFileName1, "TEMP_MERGED_SORTED_FILE");
                remove(tempFileName);
                remove(tempFileName1);
                rename("TEMP_MERGED_SORTED_FILE", tempFileName2);
            }

            if (temp2 % 2)
            {
                char snum[5];
                sprintf(snum, "%lld", temp2 - 1);
                char tempFileName[MAX_NAME_LEN];
                strcpy(tempFileName, "TEMP_SORTED_FILE");
                strcat(tempFileName, snum);

                char snum1[5];
                sprintf(snum1, "%lld", temp2 / 2);
                char tempFileName1[MAX_NAME_LEN];
                strcpy(tempFileName1, "TEMP_SORTED_FILE");
                strcat(tempFileName1, snum1);
                rename(tempFileName, tempFileName1);
            }
            if (num_of_files == 1)
            {
                rename("TEMP_SORTED_FILE0", argc[2]);
                printf("MERGING COMPLETED\n");
                break;
            }
        }
    }
    printf("SORTED FILE SUCCESSFULLLY TO `FINAL_SORTED_FILE`\n");
    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("completed in %lfmins\n", cpu_time_used / 60);
    return 0;
}