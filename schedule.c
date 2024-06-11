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

extern int sem_id;
extern SharedMemory *shared_memory;

// 시스템 정보를 출력하는 함수
void print_system_info() {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        perror("sysinfo"); // 시스템 정보 가져오기 실패 시 에러 출력
        return;
    }

    // 시스템 정보 출력
    printf(CYAN "=== System Information ===\n" RESET);
    printf("Uptime:           %ld seconds\n", sys_info.uptime);       // 시스템 가동 시간
    printf("Total RAM:        %ld bytes\n", sys_info.totalram);       // 총 RAM 용량
    printf("Free RAM:         %ld bytes\n", sys_info.freeram);        // 사용 가능한 RAM 용량
    printf("Process count:    %d\n", sys_info.procs);                // 현재 실행 중인 프로세스 수
    printf(CYAN "=========================\n" RESET);
}

// 일정 파일을 백업하는 함수
void backup_schedule(const char *filename) {
    pid_t pid = fork();

    if (pid == 0) { // 자식 프로세스
        char backup_filename[256];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        
        // 백업 파일 이름 생성
        snprintf(backup_filename, sizeof(backup_filename), "backup_%d%02d%02d%02d%02d%02d.txt",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);

        // 원본 파일 열기
        FILE *src = fopen(filename, "r");
        if (src == NULL) {
            perror("Failed to open source file");
            exit(1);
        }

        // 백업 파일 열기
        FILE *dst = fopen(backup_filename, "w");
        if (dst == NULL) {
            perror("Failed to open backup file");
            fclose(src);
            exit(1);
        }

        // 파일 내용을 버퍼를 통해 복사
        char buffer[256];
        size_t n;
        while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            fwrite(buffer, 1, n, dst);
        }

        fclose(src);
        fclose(dst);

        // 백업 완료 메시지 출력
        printf(GREEN "Backup created: %s\n" RESET, backup_filename);
        exit(0);
    } else if (pid > 0) { // 부모 프로세스
        wait(NULL); // 자식 프로세스가 종료될 때까지 기다림
        printf(GREEN "Backup process completed.\n" RESET);
    } else {
        perror("fork"); // fork 실패 시 에러 출력
        exit(1);
    }
}



// 공유 메모리에서 일정 검색 함수
void search_schedule(const char *keyword) {
    sem_lock(sem_id); // 세마포어 잠금

    printf(GREEN "=== Search Results ===\n" RESET);
    for (int i = 0; i < shared_memory->count; i++) {
        // 일정 또는 사용자 이름에 키워드가 포함되어 있는지 확인
        if (strstr(shared_memory->schedules[i].event, keyword) != NULL ||
            strstr(shared_memory->schedules[i].user, keyword) != NULL) {
            int hours = shared_memory->schedules[i].time / 100;
            int minutes = shared_memory->schedules[i].time % 100;
            // 검색 결과 출력
            printf("Date: %d, Time: %02dH%02dM, Event: %s, User: %s\n",
                   shared_memory->schedules[i].date, hours, minutes,
                   shared_memory->schedules[i].event, shared_memory->schedules[i].user);
        }
    }
    printf(GREEN "=========================\n" RESET);

    sem_unlock(sem_id); // 세마포어 잠금 해제
}

// 일정 파일에서 특정 키워드를 검색하는 함수
void search_schedule_file(const char *filename, const char *keyword) {
    pid_t pid = fork();

    if (pid == 0) { // 자식 프로세스
        // grep 명령어를 사용하여 파일에서 키워드 검색
        execlp("grep", "grep", keyword, filename, NULL);
        perror("execlp"); // execlp 실패 시 에러 출력
        exit(1);
    } else if (pid > 0) { // 부모 프로세스
        wait(NULL); // 자식 프로세스가 종료될 때까지 기다림
        printf(GREEN "Search completed.\n" RESET);
    } else {
        perror("fork"); // fork 실패 시 에러 출력
        exit(1);
    }
}

void add_schedule_to_local_file() {
    int date, time;
    char event[256];
    char user[256];
    
    printf("Enter your name: ");
    scanf("%s", user);
    printf("Enter date (YYYYMMDD): ");
    scanf("%d", &date);
    printf("Enter time (HHMM): ");
    scanf("%d", &time);
    printf("Enter event: ");
    scanf("%s", event);
    
    FILE *file = fopen("schedule.txt", "a");
    if (file == NULL) {
        perror("Failed to open schedule file");
        return;
    }
    
    fprintf(file, "%d %d %s %s\n", date, time, event, user);
    fclose(file);
    
    printf(GREEN "Schedule added to local file successfully!\n" RESET);
}
