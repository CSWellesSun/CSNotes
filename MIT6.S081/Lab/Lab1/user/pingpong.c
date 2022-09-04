#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
main(int argc, char *argv[]) 
{
   int ptc[2];
   int ctp[2];
   pipe(ptc);
   pipe(ctp);
   char ch[1] = {'A'};

   if (fork() == 0) {
    // child
    read(ptc[0], ch, 1);
    close(ptc[0]);
    close(ptc[1]);
    printf("%d: received ping\n", getpid());
    write(ctp[1], ch, 1);
    close(ctp[0]);
    close(ctp[1]);
    exit(0);
   } else {
    write(ptc[1], ch, 1);
    close(ptc[0]);
    close(ptc[1]);
    read(ctp[0], ch, 1);
    close(ctp[0]);
    close(ctp[1]);
    printf("%d: received pong\n", getpid());
    exit(0);
   } 
}