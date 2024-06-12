#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <time.h>

// 일정 구조체 정의
typedef struct {
    int date;          // 일정 날짜 (YYYYMMDD 형식)
    int time;          // 일정 시간 (HHMM 형식)
    char event[256];   // 일정 이벤트 설명
    char user[256];    // 사용자 이름
} Schedule;

// 공유 메모리 구조체 정의
typedef struct {
    int count;              // 저장된 일정 개수
    Schedule schedules[100]; // 일정 배열
    time_t last_added;      // 마지막 일정 추가 시간
    time_t last_deleted;    // 마지막 일정 삭제 시간
} SharedMemory;

// 공유 메모리 관련 함수 선언

// 공유 메모리를 초기화하는 함수
void initialize_shared_memory();

// 세마포어를 초기화하는 함수
void initialize_semaphore();

// 공유 메모리에 일정을 추가하는 함수
void add_schedule_to_shared_memory();

// 공유 메모리에 저장된 일정을 출력하는 함수
void view_shared_memory_schedule();

// 공유 메모리에서 일정을 삭제하는 함수
void delete_schedule_from_shared_memory();

// 공유 메모리에서 분리하는 함수
void detach_shared_memory();

// 공유 메모리를 제거하는 함수
void remove_shared_memory();

// 공유 메모리에서 일정을 검색하는 함수
void search_schedule(const char *keyword);

// 파일에서 일정을 검색하는 함수
void search_schedule_file(const char *filename, const char *keyword);

// 세마포어를 잠그는 함수
void sem_lock(int sem_id);

// 세마포어를 여는 함수
void sem_unlock(int sem_id);

// 시스템 정보를 출력하는 함수 선언
void print_system_info();

// 외부 프로그램을 통해 일정을 파일로 내보낼 수 있음
// void export_schedule_to_file();

extern int sem_id;  // 세마포어 ID
extern SharedMemory *shared_memory;  // 공유 메모리 포인터

#endif
