#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>

int main()
{
    pid.t pid;
    
    pid= fork();

    if (pid < 0){
    fprint(stderr,"Fork Faild");
    return 1;
    }
    else if (pid == 0){
        execIp("/bin/ls","ls",NULL);
    }
    else{
        wait(NULL);
        printf("Child Completed");
    }
return 0;
}