#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

static DWORD exitcode = 0;

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

size_t terminate(char *buffer)
{
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
      buffer[--len] = 0;

    return len;
}

void expand(char *line, char *token)
{
  if (*token != '$') {
    strcat_s(line, MAX_PATH - strlen(line), token);
    strcat_s(line, MAX_PATH - strlen(line), " ");
    return;
  }

  if (*(++token) == '?') {
    sprintf_s(line, MAX_PATH - strlen(line), "%s%d", line, exitcode);
    return;
  }

  if(GetEnvironmentVariableA(token, line + strlen(line), MAX_PATH - (DWORD)strlen(line)))
    return;

}

int main(int argc, char *argv[])
{
  int cmd = 1;
  bool exit = false;
  char buffer[MAX_PATH];
  char cmdline[MAX_PATH];
  char *line = cmdline;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

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

  line[0] = 0;
  for (int a = cmd; a < argc; a++) {
    strcat_s(line, MAX_PATH - strlen(line), argv[a]);
    strcat_s(line, MAX_PATH - strlen(line), " ");
  }

  do {

    if (!exit && fgets(buffer, sizeof(buffer), stdin) != NULL) {

      size_t len = terminate(buffer);

      if (len == 0)
        continue;

      char *ctx;
      char *token = strtok_s(buffer, " \t", &ctx);

      if (strcmp(token, "exit") == 0)
        return 0;

      if (strcmp(token, "cd") == 0) {
        token = strtok_s(NULL, " \t", &ctx);

        if (token != NULL) {

          if (!SetCurrentDirectory(token)) {

            int e = GetLastError();
            switch (e)
            {
              case 2:
                fprintf(stderr, "%s: %s, no such directory\n", argv[0], token);
                break;

              default:
                fprintf(stderr, "%s: unable to change directory (%d)\n", argv[0], e);
                break;
            }

          }

        }

        continue;

      }

      *line = 0;

      do {
        expand(line, token);
      } while ((token = strtok_s(NULL, " \t", &ctx)) != NULL);

    }

    if(*line != 0 && !CreateProcess(NULL, line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

      exitcode = GetLastError();

      switch (exitcode) {

        case 2:
          char *ctx;
          char *binary = strtok_s(line, " ", &ctx);
          if (binary == NULL) { binary = line; };
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


  return exitcode;
}
