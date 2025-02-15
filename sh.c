#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

void usage(const char *bin)
{
    printf("Usage: %s [-|/h] [-|/c FILE [ARGS]]\n"
           "\n"
           "\t -|/c FILE [ARGS]\tExecute FILE with arguments ARGS and exit\n"
           "\t\t\t\tWithout this option %s runs interactively\n"
           "\t -|/h\t\t\tShow this help message\n"
           "\n"
           "Note: -|/ means both - and / can be used to specificy arguments (e.g. /h or -h)\n", bin, bin);
}

int main(int argc, char *argv[])
{
  int cmd = 1;
  bool exit = false;
  char buffer[MAX_PATH];
  char cmdline[MAX_PATH];

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD exitcode = 0;

  if (argc > 1) {

    for (int a = 1; a < argc && !exit; a++) {

      if (argv[a][0] == '/' || argv[a][0] == '-') {

        switch (argv[a][1]) {

          case 'C':
          case 'c':
            if (argv[a+1] == NULL) {
              usage(argv[0]);
              return ERROR_BAD_ARGUMENTS;
            }
            cmd = a+1;
            exit = true;
            break;

          case 'h':
            usage(argv[0]);
            return ERROR_SUCCESS;

          default:
            usage(argv[0]);
            return ERROR_BAD_ARGUMENTS;

        }

      }

    }

  }

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  cmdline[0] = 0;
  for (int a = cmd; a < argc; a++) {
    strcat_s(cmdline, MAX_PATH - strlen(cmdline), argv[a]);
    strcat_s(cmdline, MAX_PATH - strlen(cmdline), " ");
  }

  do {

    if (!exit && fgets(buffer, sizeof(buffer), stdin) != NULL) {

      size_t len = strlen(buffer);
      if (len > 0 && buffer[len-1] == '\n')
        buffer[--len] = 0;

      int c = 0;
      for (int b = 0; b < len; b++) {
        if (buffer[b] == '$') {
          b++;
          if (buffer[b] == '?') {
            sprintf_s(&cmdline[c], MAX_PATH - c, "%d", exitcode);
            continue;
          }

          int v = b;
          while (buffer[v] != 0 || buffer[v] == ' '){ v++; };
          buffer[v] = 0;

          int r = GetEnvironmentVariableA(&buffer[b], &cmdline[c], MAX_PATH - c);

          if (r) {
            c += r - 1;
          }

          b += v + 1;
        

        } else {
          cmdline[c] = buffer[b];
          c++;
        }

      }

      cmdline[c] = 0;

      if (strcmp(cmdline, "exit") == 0)
        return 0;

    }

    if(!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

      exitcode = GetLastError();

      switch (exitcode) {

        case 2:
          char *token;
          char *binary = strtok_s(cmdline, " ", &token);
          if (binary == NULL) { binary = cmdline; };
          fprintf(stderr, "%s: %s, not found\n", argv[0], binary);
          break;

        default:
          fprintf(stderr, "%s: could not create process (%d)\n", argv[0], exitcode);
          break;
      }

      continue;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitcode);

  } while (!exit);


  return 0;
}
