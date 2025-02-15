#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FILE *f = NULL;
  char buf[4096];
  size_t n;

  for (int i = 1; i < argc; i++) {

    int e = fopen_s(&f, argv[i], "r");

    if (e) {
      if (e == ERROR_FILE_NOT_FOUND)
        fprintf(stderr, "%s: %s not found", argv[0], argv[i]);
      return e;
    } else {
      while((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        fwrite(buf, 1, n, stdout);
      }
    }

    fclose(f);
  }

  return 0;
}
