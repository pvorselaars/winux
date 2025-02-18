#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
  int opt = 1;
  bool list = false, hidden = false;

  for (int i = 1; i < argc; i++) {

    if (argv[i][0] == '-') {

      for (int j = 1; argv[i][j]; j++) {

        switch (argv[i][j]) {
          case 'l':
            list = true;
            break;

          case 'a':
            hidden = true;
            break;

          default:
            fprintf(stderr, "%s: unknown option %c\n", argv[0], argv[i][j]);
            break;
        }

      }

      argv[i] = 0;
      opt++;

      continue;
    }

  }

  if (opt == argc)
    argv[argc++] = "*";

  for (int i = 1; i < argc; i++) {
    if (argv[i]) {

      WIN32_FIND_DATA data;
      HANDLE f = FindFirstFile(argv[i], &data);

      if (f == INVALID_HANDLE_VALUE)
        return GetLastError();

      do {

        if (data.cFileName[0] != '.' || hidden) {
          printf(data.cFileName);
          list ? putc('\n', stdout) : putc(' ', stdout);
        }

      } while (FindNextFile(f, &data) != 0);

      FindClose(f);

    }
  }

  return GetLastError();
}
