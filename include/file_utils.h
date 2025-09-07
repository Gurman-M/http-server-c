#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int read_file_contents(char *filename, long int file_size, char **dst);
long int find_file_size(char *filename);

#endif