#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * "414141" => "\x41\x41\x41"
 */
unsigned char* str_to_bytes_array(char* str)
{
    int i;
    int len;
    char buffer[2];
    unsigned char* bytes;

    if ((len = strlen(str)) % 2 != 0) {
        fprintf(stderr, "str_to_bytes_array arg's len should be even\n");
        exit(EXIT_FAILURE);
    }

    bytes = malloc(sizeof(unsigned char) * (len / 2));
    i = 0;
    while (i < len) {
        bzero(buffer, 2);
        buffer[0] = str[i];
        buffer[1] = str[i + 1];
        bytes[i / 2] = strtol(buffer, NULL, 16);
        i += 2;
    }
    return bytes;
}

int bytescmp(unsigned char* seq_1, unsigned char* seq_2, size_t len)
{
    size_t i;

    i = 0;
    while (i < len) {
        if (seq_1[i] != seq_2[i])
            return -1;
        i++;
    }
    return (0);
}

int main(int ac, char** av)
{
    size_t seek_seq_len;
    FILE* file;
    char* path;
    unsigned char* buffer;
    unsigned char* seek_seq;
    unsigned char* replace_seq;

    if (ac != 4) {
        fprintf(stderr, "[USAGE] - ./byteswitch <old sequence> <new sequence> <binary_file>\n");
        exit(EXIT_FAILURE);
    }

    int arg_len = strlen(av[1]);
    if (arg_len % 2 != 0 || arg_len != strlen(av[2])) {
        fprintf(stderr, "arg's length should be the same\n");
        exit(EXIT_FAILURE);
    }

    seek_seq = str_to_bytes_array(av[1]);
    seek_seq_len = (size_t)strlen(av[1]) / 2;
    replace_seq = str_to_bytes_array(av[2]);
    path = av[3];

    if ((file = fopen(path, "r+b")) == NULL) {
        fprintf(stderr, "Can't open %s\n", path);
        return (EXIT_FAILURE);
    }

    buffer = malloc(sizeof(unsigned char) * seek_seq_len);

    size_t i = 0;
    int occurence = 0;
    fpos_t position;
    while (fread(buffer, 1, seek_seq_len, file) == seek_seq_len) {
        fgetpos(file, &position);

        if (bytescmp(seek_seq, buffer, seek_seq_len) == 0) {
            fseek(file, position - seek_seq_len, SEEK_SET);
            fwrite(replace_seq, 1, seek_seq_len, file);
            occurence++;
        } else {
            fseek(file, position + 1 - seek_seq_len, SEEK_SET);
        }
        bzero(buffer, seek_seq_len);
    }

    printf("Replaced %d occurences\n", occurence);

    fclose(file);
    free(buffer);
    free(seek_seq);
    free(replace_seq);
    return (EXIT_SUCCESS);
}
