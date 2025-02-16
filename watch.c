#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DATE_LENGTH 64
#define TIME_LENGTH 16

int main(int argc, char *argv[])
{
  int sec = 2;
  char shell[MAX_PATH];

  if (!GetEnvironmentVariableA("SHELL", shell, MAX_PATH)) {
    strcpy_s(shell, MAX_PATH, "C:\\Windows\\System32\\cmd.exe");
  }

  char *cmd = NULL;
  char cmdline[MAX_PATH];
  int timelength, datelength, cmdlength, digits = 1;
  bool timestamp = true;
  bool quiet = false;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD exitcode = 0;

  SYSTEMTIME t;
  wchar_t date[DATE_LENGTH];
  wchar_t time[TIME_LENGTH];

  if (argc < 2) {
    printf("Usage: %s [-n SEC] [-t] [-q] PROG\n"
           "\n"
           "Run PROG periodically\n"
           "\n"
           "\t-n SEC\tPeriod in seconds (default 2)\n"
           "\t-t\tDo not print header\n"
           "\t-q\tDo not alert when exit code is not 0\n", argv[0]);

    return ERROR_SUCCESS;
  }

  for (int a = 1; a < argc; a++) {

    // process options
    if (argv[a][0] == '-') {

      switch (argv[a][1]) {

        case 'n':
          if (argv[++a] == NULL)
            return ERROR_BAD_ARGUMENTS;

          sec = atoi(argv[a]);
          digits = (int)strlen(argv[a]);

          if (sec == 0)
            return ERROR_BAD_ARGUMENTS;
          continue;

        case 't':
          timestamp = false;
          continue;

        case 'q':
          quiet = true;
          continue;

        default:
          return ERROR_BAD_ARGUMENTS;
      }

    }

    if (cmd != NULL)
      return ERROR_BAD_ARGUMENTS;

    cmd = argv[a];

  }

  if (cmd == NULL)
    return ERROR_BAD_ARGUMENTS;

  cmdlength = (int)strlen(cmd);
  snprintf(cmdline, MAX_PATH, "/c %s", cmd);

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  while (1) {
    printf("\033[2J\033[H");

    if (timestamp) {
      printf("Every %ds: %s", sec, cmd);

      GetLocalTime(&t);
      datelength = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &t, NULL, date, DATE_LENGTH, NULL) - 1;
      timelength = GetTimeFormatEx(LOCALE_NAME_INVARIANT, 0, &t, NULL, time, TIME_LENGTH) - 1;

      printf("\033[1000G\033[%dD", datelength+timelength);
      fputws(time, stdout);
      fputws(L" ", stdout);
      fputws(date, stdout);
      fputws(L"\n\n", stdout);

    }

    if(!CreateProcess(shell, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      return GetLastError();

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitcode);

    if (exitcode && !quiet)
      printf("\a");

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    Sleep(sec*1000);
  }

  return 0;
}
