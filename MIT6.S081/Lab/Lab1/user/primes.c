#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void Action(int *p) {
    int prime, i;
    int flag = 0;
    int np[2];

    while (read(p[0], &i, 4)) {
        if (flag == 0) {
            prime = i;
            flag = 1;
            printf("prime %d\n", prime);
            pipe(np);
            if (fork() == 0) {
                close(np[1]);
                Action(np);
                exit(0);
            } else {
                close(np[0]);
            }
        } else if (i % prime) {
            write(np[1], &i, 4);
        }
    }
    if (flag) {
        close(np[1]);
        wait((int *)0);
    }
}

void
main(int argc, char *argv[])
{
    int prime;
    int flag = 0;
    int np[2];

    for (int i = 2; i <= 35; i++) {
        if (flag == 0) {
            prime = i;
            flag = 1;
            printf("prime %d\n", prime);
            pipe(np);
            if (fork() == 0) {
                close(np[1]);
                Action(np);
                exit(0);
            } else {
                close(np[0]);
            }
        } else if (i % prime) {
            write(np[1], &i, 4);
        }
    }
    if (flag) {
        close(np[1]);
        wait((int *)0);
    }
    exit(0);
}