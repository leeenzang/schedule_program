#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/sysinfo.h>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"

extern int sem_id;  // 세마포어 ID
extern SharedMemory *shared_memory;  // 공유 메모리 포인터

// 시스템 정보를 출력하는 함수
void print_system_info() {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        perror("sysinfo");  // 시스템 정보 조회 실패 시 에러 메시지 출력
        return;
    }

    printf(CYAN "=== System Information ===\n" RESET);
    printf("Uptime:           %ld seconds\n", sys_info.uptime);
    printf("Total RAM:        %ld bytes\n", sys_info.totalram);
    printf("Free RAM:         %ld bytes\n", sys_info.freeram);
    printf("Process count:    %d\n", sys_info.procs);
    printf(CYAN "=========================\n" RESET);
}

// 공유 메모리에서 일정 검색
void search_schedule(const char *keyword) {
    sem_lock(sem_id);  // 세마포어 잠금

    printf(GREEN "=== Search Results ===\n" RESET);
    for (int i = 0; i < shared_memory->count; i++) {
        if (strstr(shared_memory->schedules[i].event, keyword) != NULL ||
            strstr(shared_memory->schedules[i].user, keyword) != NULL) {
            int hours = shared_memory->schedules[i].time / 100;
            int minutes = shared_memory->schedules[i].time % 100;
            printf("Date: %d, Time: %02dH%02dM, Event: %s, User: %s\n",
                   shared_memory->schedules[i].date, hours, minutes,
                   shared_memory->schedules[i].event, shared_memory->schedules[i].user);
        }
    }
    printf(GREEN "=========================\n" RESET);

    sem_unlock(sem_id);  // 세마포어 잠금 해제
}

// 일정 파일에서 특정 키워드를 검색하는 함수
void search_schedule_file(const char *filename, const char *keyword) {
    pid_t pid = fork();

    if (pid == 0) { // 자식 프로세스
        execlp("grep", "grep", keyword, filename, NULL);  // grep 명령어 실행
        perror("execlp");  // execlp 실패 시 에러 메시지 출력
        exit(1);
    } else if (pid > 0) { // 부모 프로세스
        wait(NULL);  // 자식 프로세스가 종료될 때까지 기다림
        printf(GREEN "Search completed.\n" RESET);
    } else {
        perror("fork");  // fork 실패 시 에러 메시지 출력
        exit(1);
    }
}
