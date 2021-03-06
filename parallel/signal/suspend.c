#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

static void int_handler(int s)
{
    write(1,"!",1);
}


int main()
{

    int i,j ;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set,SIGINT);

    signal(SIGINT,int_handler);
    sigprocmask(SIG_BLOCK,&set,NULL);
    for(j=0;j<100;j++)
    {
        for(i = 0;i < 5; i++)
        {
            write(1,"*",1);
            usleep(50000);
        }
        write(1,"\n",1);
        sigsuspend(&set);   // 原子操作,信号一定会砸在这个函数上
        // sigprocmask(SIG_UNBLOCK,&set,NULL);
        // pause();
    }
//  有回显 但是没有'!' 信号响应.
    exit(0);
}