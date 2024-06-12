#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "schedule.h"

int main() {
    // 공유 메모리 식별자 생성
    int shm_id = shmget(0x1234, sizeof(SharedMemory), 0644);
    if (shm_id == -1) {
        perror("shmget"); // shmget 실패 시 에러 메시지 출력
        return 1;
    }

    // 공유 메모리 첨부
    SharedMemory *shared_memory = (SharedMemory *)shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat"); // shmat 실패 시 에러 메시지 출력
        return 1;
    }

    // 출력 파일 생성
    FILE *file = fopen("exported_schedule.txt", "w");
    if (file == NULL) {
        perror("fopen"); // fopen 실패 시 에러 메시지 출력
        return 1;
    }

    // 공유 메모리에 저장된 일정 파일로 쓰기
    for (int i = 0; i < shared_memory->count; i++) {
        fprintf(file, "Date: %d, Time: %04d, Event: %s, User: %s\n",
                shared_memory->schedules[i].date,
                shared_memory->schedules[i].time,
                shared_memory->schedules[i].event,
                shared_memory->schedules[i].user);
    }

    fclose(file); // 파일 닫기
    shmdt(shared_memory); // 공유 메모리 분리
    printf("Schedule exported to exported_schedule.txt\n");

    return 0;
}
