//
// Created by Peter on 2026/7/5.
//
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


void *worker(void *args)
{
    int num = *(int *)args;
    printf("child thread %ld running, input para: %d\n", pthread_self(), num);
    int *ret = malloc(sizeof(int));
    *ret = num * 100;
    pthread_exit(ret);
}


int main(int argc, char *argv[])
{
    pthread_t tid;
    int param = 666;
    int *res;
    if (pthread_create(&tid, NULL, worker, &param) != 0) {
        perror("create error!\n");
        return -1;
    }

    pthread_join(tid, (void **)&res);
    printf("主线程收到返回：%d\n", *res);
    free(res);
    return 0;
    // TODO: 需要说明每个API的入参 并说清楚对应的内容， 给出一个实际的案例， demo 需要从 gcc 编译进行说明。
}


