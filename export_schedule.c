#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "schedule.h"

int main() {
    int shm_id = shmget(0x1234, sizeof(SharedMemory), 0644);
    if (shm_id == -1) {
        perror("shmget");
        return 1;
    }

    SharedMemory *shared_memory = (SharedMemory *)shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        return 1;
    }

    FILE *file = fopen("exported_schedule.txt", "w");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    for (int i = 0; i < shared_memory->count; i++) {
        fprintf(file, "Date: %d, Time: %04d, Event: %s, User: %s\n",
                shared_memory->schedules[i].date,
                shared_memory->schedules[i].time,
                shared_memory->schedules[i].event,
                shared_memory->schedules[i].user);
    }

    fclose(file);
    shmdt(shared_memory);
    printf("Schedule exported to exported_schedule.txt\n");

    return 0;
}
