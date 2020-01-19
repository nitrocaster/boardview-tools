#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bdv_decode_str(char *src, int str_index)
{    
    int idx = str_index%127;
    for (int i = 0; ; i++)
    {
        if (!src[i])
            break;
        if (src[i]=='\n')
            break;
        if (src[i]<=127)
            src[i] = 127-src[i]+' '+idx;
        else
            src[i] = 127-src[i]+' '-idx;
    }
}

char *brd_decode_str(char *src, char *dst)
{    
    if (!*src)
        return NULL;
    int i;
    for (i = 0; ; i++)
    {
        if (!src[i])
            break;
        char n = ~(4*src[i] | (src[i] & 0xC0) >> 6);
        if (n==(char)0xCB)
            n = '\r';
        if (n==(char)0xD7)
            n = '\n';
        dst[i] = n;
    }
    dst[i] = 0;
    return dst;
}

enum
{
    FORMAT_BRD, // apple
    FORMAT_BDV, // thinkpads
    FORMAT_UNKNOWN = -1
};

int brd_decode(FILE *src, FILE *dst)
{
    const int buf_sz = 4096;
    const int dst_buf_sz = 8192;
    char *buf = malloc(buf_sz);
    char *dst_buf = malloc(dst_buf_sz);
    while (fgets(buf, buf_sz, src))
    {
        char *dec = brd_decode_str(buf, dst_buf);
        if (dec)
            fputs(dec, dst);
        else
            fputs("\n", dst);
    }
    free(buf);
    free(dst_buf);
    return 0;
}

int bdv_decode(FILE *src, FILE *dst)
{
    const int buf_sz = 4096;
    char *buf = malloc(buf_sz);
    int i = 1;
    while (fgets(buf, buf_sz, src))
    {
        bdv_decode_str(buf, i++);
        fputs(buf, dst);
    }
    free(buf);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc!=4)
    {
        printf("usage: %s {brd|bdv} src dst\n", argv[0]);
        return 1;
    }
    int fmt = FORMAT_UNKNOWN;
    if (!strcmp(argv[1], "brd"))
        fmt = FORMAT_BRD;
    else if (!strcmp(argv[1], "bdv"))
        fmt = FORMAT_BDV;
    if (fmt==FORMAT_UNKNOWN)
    {
        printf("unknown format: %s\n", argv[1]);
        return 1;
    }
    FILE *src = fopen(argv[2], "rb");
    if (!src)
    {
        printf("can't open %s\n", argv[2]);
        return 1;
    }
    FILE *dst = fopen(argv[3], "wb");
    if (!dst)
    {
        printf("can't open %s\n", argv[3]);
        return 1;
    }
    switch (fmt)
    {
    case FORMAT_BDV:
        bdv_decode(src, dst);
        break;
    case FORMAT_BRD:
        brd_decode(src, dst);
        break;
    }
    fclose(src);
    fclose(dst);
    return 0;
}
