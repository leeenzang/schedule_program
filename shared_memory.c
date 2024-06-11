#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define MAX_SCHEDULES 100

// ANSI 색상 코드 정의
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"

int sem_id;              // 세마포어 ID
int shm_id;              // 공유 메모리 ID
SharedMemory *shared_memory; // 공유 메모리 포인터

// 세마포어 잠금 함수
void sem_lock(int sem_id) {
    struct sembuf sb = {0, -1, 0}; // 잠금 설정
    semop(sem_id, &sb, 1);
}

// 세마포어 잠금 해제 함수
void sem_unlock(int sem_id) {
    struct sembuf sb = {0, 1, 0}; // 잠금 해제 설정
    semop(sem_id, &sb, 1);
}

// 공유 메모리 초기화 함수
void initialize_shared_memory() {
    shm_id = shmget(SHM_KEY, sizeof(SharedMemory), 0644 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    shared_memory = (SharedMemory *)shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // 파일에서 데이터를 읽어와서 공유 메모리에 로드
    FILE *file = fopen("schedule_backup.dat", "rb");
    if (file != NULL) {
        fread(shared_memory, sizeof(SharedMemory), 1, file);
        fclose(file);
    } else {
        shared_memory->count = 0;
    }

    printf("Shared memory initialized with count: %d\n", shared_memory->count);
}

// 세마포어 초기화 함수
void initialize_semaphore() {
    sem_id = semget(SEM_KEY, 1, 0644 | IPC_CREAT);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }
    semctl(sem_id, 0, SETVAL, 1);
}

// 일정 추가 함수
void add_schedule_to_shared_memory() {
    if (shared_memory->count >= MAX_SCHEDULES) {
        printf("Schedule is full.\n");
        return;
    }

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

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) { // 자식 프로세스
        close(fd[0]); // 읽기 종단 닫기

        char confirmation[4];
        printf("Do you want to add this schedule? (yes/no): ");
        scanf("%3s", confirmation);

        write(fd[1], confirmation, strlen(confirmation) + 1);
        close(fd[1]); // 쓰기 종단 닫기
        exit(0);
    } else if (pid > 0) { // 부모 프로세스
        close(fd[1]); // 쓰기 종단 닫기
        char confirmation[4];
        read(fd[0], confirmation, sizeof(confirmation));
        close(fd[0]); // 읽기 종단 닫기

        wait(NULL); // 자식 프로세스가 종료될 때까지 기다림

        if (strcmp(confirmation, "yes") == 0) {
            sem_lock(sem_id); // 세마포어 잠금
            int count = shared_memory->count;
            shared_memory->schedules[count].date = date;
            shared_memory->schedules[count].time = time;
            strcpy(shared_memory->schedules[count].event, event);
            strcpy(shared_memory->schedules[count].user, user);
            shared_memory->count++;

            // 날짜와 시간 순서대로 정렬
            for (int i = 0; i < shared_memory->count - 1; i++) {
                for (int j = 0; j < shared_memory->count - i - 1; j++) {
                    if (shared_memory->schedules[j].date > shared_memory->schedules[j + 1].date ||
                        (shared_memory->schedules[j].date == shared_memory->schedules[j + 1].date && 
                         shared_memory->schedules[j].time > shared_memory->schedules[j + 1].time)) {
                        Schedule temp = shared_memory->schedules[j];
                        shared_memory->schedules[j] = shared_memory->schedules[j + 1];
                        shared_memory->schedules[j + 1] = temp;
                    }
                }
            }

            sem_unlock(sem_id); // 세마포어 잠금 해제

            printf(GREEN "Schedule added successfully!\n" RESET);
        } else {
            printf(RED "Schedule addition canceled.\n" RESET);
        }
    } else {
        perror("fork"); // fork 실패 시 에러 출력
        exit(1);
    }
}

// 일정 삭제 함수
void delete_schedule_from_shared_memory() {
    int date, time;
    printf("Enter date (YYYYMMDD) to delete: ");
    scanf("%d", &date);
    printf("Enter time (HHMM) to delete: ");
    scanf("%d", &time);

    sem_lock(sem_id); // 세마포어 잠금

    for (int i = 0; i < shared_memory->count; i++) {
        if (shared_memory->schedules[i].date == date && shared_memory->schedules[i].time == time) {
            for (int j = i; j < shared_memory->count - 1; j++) {
                shared_memory->schedules[j] = shared_memory->schedules[j + 1];
            }
            shared_memory->count--;
            printf("Schedule deleted successfully!\n");
            sem_unlock(sem_id); // 세마포어 잠금 해제
            return;
        }
    }

    sem_unlock(sem_id); // 세마포어 잠금 해제
    printf("Schedule not found.\n");
}

// 공유 메모리에 저장된 일정을 출력하는 함수
void view_shared_memory_schedule() {
    printf(CYAN "=== Shared Memory Schedule ===\n" RESET);

    sem_lock(sem_id); // 세마포어 잠금

    for (int i = 0; i < shared_memory->count; i++) {
        int hours = shared_memory->schedules[i].time / 100;
        int minutes = shared_memory->schedules[i].time % 100;
        printf("Date: %d, Time: %02dH%02dM, Event: %s, User: %s\n",
               shared_memory->schedules[i].date, hours, minutes,
               shared_memory->schedules[i].event, shared_memory->schedules[i].user);
    }

    sem_unlock(sem_id); // 세마포어 잠금 해제

    printf(CYAN "=============================\n" RESET);
}

// 공유 메모리를 분리하고 데이터를 파일에 저장하는 함수
void detach_shared_memory() {
    // 공유 메모리의 데이터를 파일에 저장
    FILE *file = fopen("schedule_backup.dat", "wb");
    if (file != NULL) {
        fwrite(shared_memory, sizeof(SharedMemory), 1, file);
        fclose(file);
    } else {
        perror("fopen");
    }

    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
    }
}

// 공유 메모리와 세마포어를 제거하는 함수
void remove_shared_memory() {
    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        perror("shmctl");
    }
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
    }
}
