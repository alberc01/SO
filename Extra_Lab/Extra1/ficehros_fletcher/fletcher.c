#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

unsigned short fletcher16_file(FILE *file)
{
    int c;
    unsigned short sum1 = 0;
    unsigned short sum2 = 0;
    while ((c = getc(file)) != EOF)
    {
        sum1 = (sum1 + (unsigned short)c) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (sum2 << 8) | sum1;
}

int main(int argc, char *argv[])
{
    int i;
    unsigned short csum = 0x0000;
    FILE *file;

    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s file1 [file2 file3 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
   for(i = 1; i < argc; i++)
    {
        if ((file = fopen(argv[i], "r")) == NULL)
        {
            fprintf(stderr, "The file '%s' could not be opened: ", argv[i]);
            perror(NULL);
            exit(-1);
        }
        csum = fletcher16_file(file);
        printf("Fetcher16 of %s: 0x%.4X\n", argv[i], csum);
    }
}
