#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc < 1) {
    printf("Usage: %s FILE\n"
            "\n"
            "Update access time of FILE\n", argv[0]);

    return ERROR_SUCCESS;
  }

  HANDLE file;

  for (int f = 1; f < argc; f++) {
    file = CreateFileA(argv[f], GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS ,NULL);
    CloseHandle(file);
  }


  return 0;
}
