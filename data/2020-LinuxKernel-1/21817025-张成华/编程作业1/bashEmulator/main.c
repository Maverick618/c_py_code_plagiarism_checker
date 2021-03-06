/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: zch
 *模仿bash程序做一个多任务控制台，包括以下命令：
 * 1) run：运行某个程序，可带有参数，也可以指定在哪个CPU上运行。
 * 2) list: 列出当前运行的进程，尽可能的包含一些进程的信息。
 * 3) kill: 杀死某个运行的程序。
 * 4) quit: 退出多任务控制台。
 * 提交内容： 源代码，Make文件，程序说明，程序测试运行的录屏。
 * 得分因素：功能完成度，代码的可读性，包括恰当的注解和文档。
 * 
 * Created on 2020年6月15日, 上午6:29
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/wait.h>
#include<pwd.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/utsname.h>
#include<sys/sysinfo.h>
#define __USE_GNU
#include<sched.h>
#include<pthread.h>
#include<regex.h>

#define CMDLEN 128
#define NUM 20
#define MINUSNUM_PATTERN "-[0-9]*"
#define NUM_PATTERN "[0-9]*"

typedef struct {
    int pid;
    int ppid;
    char userName[20];
    char name[CMDLEN];
    char cmd[CMDLEN];
    char state;
    int vmRSS;
    int effecitveuid;
} simpleProc;
char cmd[CMDLEN];
int pids[100] = {0};
int pidCount = 0;

void rmPid(int index) {
    for (int i = index; i < pidCount - 1; i++) {
        pids[index] = pids[index + 1];
    }
    pidCount--;
}

void addPid(int pid) {
    pids[pidCount++] = pid;
}
int hasPid(int pid){
    for(int i=0;i<pidCount;i++){
        if(pid==pids[i])
            return 0;
    }
    return 1;
}
//将命令与各参数的分割，分别存储到字符串指针数组中

void CutCommand(char *cmd, char *cmdArr[]) {
    char *p = strtok(cmd, " ");
    int index = 0;
    while (p != NULL && index < NUM) {
        cmdArr[index++] = p;
        p = strtok(NULL, " ");
    }
    cmdArr[index]=NULL;
}

simpleProc getProcByPid(int pid) {
    char fileName[128];
    char *line = NULL;
    char key[32];
    char val0[128], val1[128], val2[128], val3[128];
    size_t len = 0;
    FILE *file;
    int i, count = 0;
    int ret;
    simpleProc proc;
    proc.pid = 0;
    sprintf(fileName, "/proc/%d/status", pid);
    file = fopen(fileName, "r");
    if (file != NULL) {
        while (getline(&line, &len, file) != -1) {

            ret = sscanf(line, "%31s %127s %127s %127s %127s ", key, val0, val1, val2, val3);
            if (strcmp(key, "Pid:") == 0) {
                proc.pid = atoi(val0);
                count++;
            } else if (strcmp(key, "PPid:") == 0) {
                proc.ppid = atoi(val0);
                count++;
            } else if (strcmp(key, "Name:") == 0) {
                strcpy(proc.name, val0);
                count++;
            } else if (strcmp(key, "VmRSS:") == 0) {
                proc.vmRSS = atoi(val0);
                count++;
            }
            else if (strcmp(key, "State:") == 0) {
                proc.state = val0[0];
                count++;
            } else if (strcmp(key, "Uid:") == 0) {
                int uid = atoi(val0);
                struct passwd *pws;
                pws = getpwuid(uid);
                strcpy(proc.userName, pws->pw_name);
                count++;
            } else if (count >= 6)
                break;
        }
        fclose(file);
        file = NULL;
    } else
        return proc;
    sprintf(fileName, "/proc/%d/cmdline", pid);
    file = fopen(fileName, "r");
    line = NULL;
    len = 0;
    if (file != NULL) {
        getline(&line, &len, file);
        for (int i = 0; i < len - 1; i++)
            if (line[i] == 0 && line[i + 1] != 0)
                line[i] = ' ';
        strcpy(proc.cmd, line);
    }


    return proc;

}

void printfProc(simpleProc proc) {
    cpu_set_t cpu_mask;
    int flag = 0;
    char cpuAffinity[10];
    sched_getaffinity(proc.pid, sizeof (cpu_mask), &cpu_mask);
    for (unsigned int i = 0; i < sizeof (cpu_set_t); i++) {
        if (CPU_ISSET(i, &cpu_mask)) {
            if (flag == 0) {
                flag = 1;
                sprintf(cpuAffinity, "%d", (int) i);
            } else {
                sprintf(cpuAffinity, "%s,%d", cpuAffinity, (int) i);
            }
        }
    }

    printf("%s\t%d\t%d\t%.7s\t%c\t%d\t%s\t%s\n", proc.userName, proc.pid, proc.ppid, proc.name, proc.state, proc.vmRSS,cpuAffinity, proc.cmd);

}

void runWithSpecifyCPU(int cpuId, char* cmdArr[]) {
    if (cpuId >= get_nprocs()) {
        printf("指定的CPU无效，应为 0 至 %d\n", get_nprocs() - 1);
        return;
    }

    int i = 0;
    for (; i < NUM - 2; i++) {
        cmdArr[i] = cmdArr[i + 2];
    }

    pid_t pid = fork();
    assert(pid != -1);
    if (pid == 0) {
        cpu_set_t mask;
        CPU_ZERO(&mask); //初始化set集合，将其置空
        CPU_SET(cpuId, &mask);
        //设置CPU亲和性
        if (sched_setaffinity(0, sizeof (mask), &mask) == -1) {
            printf("指定CPU亲和性失败！");
            return;
        }
        char file[CMDLEN] = {0};
        strcpy(file, cmdArr[0]);
        execv(file, cmdArr); //如果命令不在，则execv会失败
        perror("execv");
        exit(0); //如果替换失败，子进程要能正常结束
    } else {
        addPid(pid);
    }
}

void run(char* cmdArr[]) {
    int i = 0;
    for (; i < NUM - 1; i++) {
        cmdArr[i] = cmdArr[i + 1];
    }
    pid_t pid = fork();
    assert(pid != -1);
    if (pid == 0) {
        char file[CMDLEN] = {0};
        strcpy(file, cmdArr[0]);
        execv(file, cmdArr); //如果命令不在，则execv会失败
        perror("execv");
        exit(0); //如果替换失败，子进程要能正常结束
    } else {
        addPid(pid);
    }
}

void sig_handler(int signo) {
    int status;
    for (int i = 0; i < pidCount; i++) {
        simpleProc proc = getProcByPid(pids[i]);
        if (proc.state=='Z'&&waitpid(pids[i],&status,WNOHANG)>0){
            rmPid(i);
            i--;
        }
    }
}

int main(int argc, char** argv) {

    regex_t minusNumReg, numReg;
    regmatch_t pmatch[1];
    int retval = 2;
    retval = regcomp(&minusNumReg, MINUSNUM_PATTERN, REG_EXTENDED | REG_NEWLINE);
//    retval = 3;
//    retval = regcomp(&numReg, NUM_PATTERN, REG_EXTENDED | REG_NEWLINE);

    struct sigaction act, oldact;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, &oldact);
    //    signal(SIGCHLD,sig_handler);

    while (1) {
        //1、打印提示信息，并获取用户输入的命令
        printf("\033[1;34mPlease Enter:\033[0m");
        fgets(cmd,CMDLEN,stdin);
        //空命令
        if (strlen(cmd)<=1) {
            continue;
        }
        cmd[strlen(cmd) - 1] = 0;
        char* cmdCopy = (char*) calloc(strlen(cmd) + 1, sizeof (char));
        strncpy(cmdCopy, cmd, strlen(cmd) + 1);
        //命令与各参数的分割，分别存入字符串
        char *cmdArr[NUM] = {0};
        CutCommand(cmd, cmdArr);
        int i = 0;
        char * p = cmdArr[i];

        //3、划分命令： run、list、 kill、 quit、其他（ls、cd等）
        //划分命令只与命令有关，与参数无关
        if (strlen(cmdArr[0]) == 3 && strncmp(cmdArr[0], "run", 3) == 0)//run命令
        {

            if (regexec(&minusNumReg, cmdArr[1], 1, pmatch, 0) == 0) {
                int cpuId = atoi(cmdArr[1] + 1);
                int cmdLength = strlen(cmdCopy);
                //在要执行的程序前加'./'
                int len = strlen(cmdArr[2]) + 3;
                char * file = (char*) calloc(len, sizeof (char));
                sprintf(file, "./");
                strncpy(file + 2, cmdArr[2], len - 2);
                cmdArr[2] = file;
                printf("要执行的程序：%s,指定的cpu:%d\n", cmdArr[2], cpuId);
                runWithSpecifyCPU(cpuId, cmdArr);
            } else {
                int cmdLength = strlen(cmdCopy);
                //+1:字符串后面的'\0'    +2:在要执行的程序前加'./'
                int len = strlen(cmdArr[1]) + 3;
                char * file = (char*) calloc(len, sizeof (char));
                sprintf(file, "./");
                strncpy(file + 2, cmdArr[1], len - 2);
                free(cmdCopy);
                cmdArr[1] = file;
                run(cmdArr);
            }
        } else if (strlen(cmdArr[0]) == 4 && strncmp(cmdArr[0], "list", 4) == 0)//list命令
        {
            printf("子进程个数:%d\n", pidCount);
            printf("UID\tPID\tPPID\tname\tstatus\tvmRSS\tcpus\tcmd\n");
            for (int i = 0; i < pidCount; i++) {
                simpleProc proc = getProcByPid(pids[i]);
                printfProc(proc);
            }
        }else if (strlen(cmdArr[0]) == 4 && strncmp(cmdArr[0], "kill", 4) == 0)//exit命令
        {
            int pid=atoi(cmdArr[1]);
            int i=0;
            for(;i<pidCount;i++){
                if(pid==pids[i])
                    break;
            }
            if(i==pidCount){
                printf("进程不存在。\n");
            }
            else{
                kill(pid,SIGTERM);
                pause();
                if(hasPid(pid)==1)
                    printf("进程:%d 已被杀死.\n",pid);
                else
                    printf("进程:%d 杀死失败.\n",pid);
            }
        }else if (strlen(cmdArr[0]) == 4 && strncmp(cmdArr[0], "quit", 4) == 0)//exit命令
        {
            exit(0); //结束进程
        } 
    }
}