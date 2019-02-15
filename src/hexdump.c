/**
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

/**
 * Create a hexdump to include in C/C++, similar to the
 * output of "xxd -i". Useful on Windows or other environments
 * where such a tool is missing.
 */

#include <stdio.h>
#include <string.h>

/* strlen("0xff,")*16 == 80 */
#define BUF_SIZE 16


int main(int argc, char *argv[])
{
  FILE *fp = NULL;
  //long fileSize = 0;
  unsigned char buf[BUF_SIZE];
  char varName[48] = {0};
  const char *in = NULL;

  if (argc < 2) {
    fprintf(stderr, "usage: %s input > output\n", argv[0]);
    return 1;
  }

  in = argv[1];

  if ((fp = fopen(in, "rb")) == NULL) {
    fprintf(stderr, "error: cannot read file `%s'\n", in);
    return 1;
  }

  if (in[0] >= '0' && in[0] <= '9') {
    varName[0] = '_';
    strncpy(varName + 1, in, sizeof(varName) - 2);
  } else {
    strncpy(varName, in, sizeof(varName) - 1);
  }

  for (size_t i = 0; i < strlen(varName); ++i) {
    char c = varName[i];

    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
      varName[i] = '_';
    }
  }

  printf("unsigned char %s[] =\n{", varName);

  while (feof(fp) == 0)
  {
    size_t items = fread(buf, 1, BUF_SIZE, fp);

    if (ferror(fp) != 0) {
      fprintf(stderr, "error: fread()\n");
      fclose(fp);
      return 1;
    }

    putchar('\n');

    for (size_t i = 0; i < items; ++i) {
      printf("0x%02x,", buf[i]);
    }
    //fileSize += (long)items;
  }

  //printf("\n};\nunsigned int %s_len = %ld;\n", varName, fileSize);
  printf("\n};\n\n");

  if (fp) {
    fclose(fp);
  }

  return 0;
}
