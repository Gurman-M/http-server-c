#include <stdlib.h>
#include <stdio.h>

#include "file_utils.h"

/**
 * Finds the file size in bytes of a given file
 * 
 * @param filename
 * 
 * @return the file size in bytes if successful, -1 otherwise
 */
long int find_file_size(char *filename) {
    FILE *fp = fopen(filename, "r");
    long int res = -1;

    if (fp == NULL) {
        fprintf(stderr, "Failed to open file\n");
        goto end;
    }

    if (fseek(fp, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Error seeking end of file\n");
        goto end;
    }
    
    res = ftell(fp);

    if (res == -1) {
        perror("ftell");
    }

end:
    fclose(fp);
    return res;
}

/**
 * Reads in the contents of the given file to a string buffer
 * 
 * @param filename
 * @param file_size
 * @param dst a reference to the buffer to write the file contents to
 * 
 * @return 0 on success, -1 otherwise
 */
int read_file_contents(char *filename, long int file_size, char **dst) {
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    size_t bytes_read = fread(*dst, 1, file_size, fp);

    if (bytes_read != file_size) {
        perror("fread failed");
        fclose(fp);
        return -1;
    }

    (*dst)[file_size] = '\0';
    fclose(fp);

    return 0;
}