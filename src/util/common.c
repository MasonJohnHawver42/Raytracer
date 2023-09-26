#include <stdio.h>
#include <stdlib.h>

#include "util/common.h"

char* file_loaddata(const char* file_name, int* dataSize) 
{
    unsigned char *data = NULL;

    if (file_name == NULL || dataSize == NULL) 
    { 
        printf("Resources::Error [load_filedata : passed NULL ]\n");
        return data; 
    }


    FILE* in_file = fopen(file_name, "r");
    *dataSize = 0;

    if (in_file == NULL) 
    {
        printf("Resources::Error [load_filedata : could not open file]\n");
        return data;
    }

    fseek(in_file, 0, SEEK_END);
    int size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    if (size <= 0) 
    { 
        printf("Resources::Error [load_filedata : file empty]\n");
        return data; 
    }

    data = malloc(size * sizeof(char));

    if (data == NULL) 
    {
        printf("Resources::Error [load_filedata : couldnt allocate data]\n");
        return data;
    }

    unsigned int count = (unsigned int)fread(data, sizeof(char), size, in_file);
    *dataSize = count;

    if (count != size) 
    {
        printf("Resources::Error [load_filedata : size mismatch]\n");
        return data;
    }

    fclose(in_file);

    return data;
}