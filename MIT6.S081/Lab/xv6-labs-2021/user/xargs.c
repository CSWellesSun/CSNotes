#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void
main(int argc, char *argv[]) {
    char *nargv[MAXARG]; // argv from the pipe 
    char *xargv[MAXARG]; // original argv with new argv from pipe
    int nargc = 0;
    char buff[MAXARG][20];
    char *p = buff[0];

    while (read(0, p, 1)) {
        if (*p == '\n') {
            *p = 0;
            nargv[nargc] = buff[nargc];
            nargc++;
            p = buff[nargc];
            if (nargc >= MAXARG) {
                fprintf(2, "Too many arguments!\n");
                exit(1);
            }
            continue;
        }
        p++;
    }

    for (int i = 0; i < argc - 1; i++)
        xargv[i] = argv[i + 1];
    xargv[argc] = 0;

    for (int i = 0; i < nargc; i++) {
        xargv[argc - 1] = nargv[i];
        if (fork() == 0) {
            exec(xargv[0], xargv);
            fprintf(2, "exec: error\n");
            exit(1);
        } else {
            wait((int *)0);
        } 
    }

    exit(0);
}