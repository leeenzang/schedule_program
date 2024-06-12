#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <time.h>

// 일정 구조체 정의
typedef struct {
    int date;
    int time;
    char event[256];
    char user[256];
} Schedule;

// 공유 메모리 구조체 정의
typedef struct {
    int count;
    Schedule schedules[100];
    time_t last_added;    // 마지막 일정 추가 시간
    time_t last_deleted;  // 마지막 일정 삭제 시간
} SharedMemory;

// 공유 메모리 관련 함수 선언
void initialize_shared_memory();
void initialize_semaphore();
void add_schedule_to_shared_memory();
void view_shared_memory_schedule();
void delete_schedule_from_shared_memory();
void detach_shared_memory();
void remove_shared_memory();
void search_schedule(const char *keyword);
void search_schedule_file(const char *filename, const char *keyword);
void sem_lock(int sem_id);
void sem_unlock(int sem_id);

// 시스템 정보 출력 함수 선언
void print_system_info();

// 외부 프로그램을 통해 일정을 파일로 내보낼 수 있음
// void export_schedule_to_file();

extern int sem_id;
extern SharedMemory *shared_memory;

#endif

