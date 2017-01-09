#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <stdio.h>
int main()
{   pid_t pid;
    long orig_rax, rax;
    long params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct regs;
    int i = 10;
    int dat;
    pid = fork();
    if(pid == 0) {
        //attaching the child to the parent for being traced.
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("test", "test", NULL);
    }
    else {
       while(1) {
           //used single step to catch all the signals and load them in getregs.
            ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
            wait(&status);
            if(WIFEXITED(status)) break;
            //peekdata won't work because we don't have actual location of &i.
            dat = ptrace(PTRACE_PEEKDATA, pid, &i, 0);
            
            ptrace(PTRACE_GETREGS, pid,
                        NULL, &regs);
            if(regs.rax == __NR_write ){
                //when rax == __NR_write it means it is calling the system call write(). in that case the registers are as follows.
                printf("%lld %lld, %lld, %lld\n",regs.rax,
                        regs.rbx, regs.rcx,
                        regs.rdx);
            } 
            if(dat == 10) {
                //as peekdata isn't working so won't this particular block.
                ptrace(PTRACE_POKEDATA, pid, &i, 2341);
                //if this particular code was hit then the code will run without stopping for single step.
                ptrace(PTRACE_CONT, pid, 0, 0);
                break;
            }
       }
   }
   return 0;
}