#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/wait.h>    
#include "schedule.h"

// ANSI 색상 코드 정의
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define FILENAME "schedule.txt"

// 프로그램 제목을 출력하는 함수
void print_title() {
    printf(CYAN "================================================\n");
    printf("        Schedule Management Program (＞Д＜)ゝ \n");
    printf("================================================\n" RESET);
}

// 메뉴를 출력하는 함수
void print_menu() {
    printf(CYAN "=== Schedule Management Program ===\n" RESET);
    printf(BLUE "1. Add Schedule to Shared Memory\n" RESET);
    printf(BLUE "2. Delete Schedule from Shared Memory\n" RESET);
    printf(BLUE "3. View Shared Memory Schedule\n" RESET);
    printf(BLUE "4. Search Schedule in Shared Memory\n" RESET);
    printf(BLUE "5. Export Schedule to File\n" RESET);
    printf(BLUE "6. Exit\n" RESET);
    printf(CYAN "Select an option: " RESET);
}

// 상태 메시지를 출력하는 함수
void print_status(const char *message) {
    printf(GREEN "%s\n" RESET, message);
}

// 오류 메시지를 출력하는 함수
void print_error(const char *message) {
    printf(RED "%s\n" RESET, message);
}

// 일정을 파일로 내보내는 함수
// exec 함수를 사용하여 외부 프로그램을 실행
void export_schedule_to_file() {
    pid_t pid = fork();
    if (pid == 0) { // 자식 프로세스
        execl("./export_schedule", "export_schedule", NULL); // 외부 프로그램 실행
        perror("execl"); // execl 실패 시 에러 출력
        exit(1);
    } else if (pid > 0) { // 부모 프로세스
        wait(NULL); // 자식 프로세스가 종료될 때까지 기다림
        printf(GREEN "Export process completed.\n" RESET);
    } else {
        perror("fork");
        exit(1);
    }
}

int main() {
    int choice;
    char keyword[256];

    // 공유 메모리 및 세마포어 초기화
    initialize_shared_memory();
    initialize_semaphore();

    // 프로그램 제목 출력
    print_title();
    // 시스템 정보 출력
    print_system_info();

    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // 공유 메모리에 일정 추가
                add_schedule_to_shared_memory();
                break;
            case 2:
                // 공유 메모리에서 일정 삭제
                delete_schedule_from_shared_memory();
                break;
            case 3:
                // 공유 메모리에 저장된 일정 보기
                view_shared_memory_schedule();
                break;
            case 4:
                // 공유 메모리에서 일정 검색
                printf("Enter keyword to search in shared memory: ");
                scanf("%s", keyword);
                search_schedule(keyword);
                break;
            case 5:
                // 일정 파일로 내보내기
                export_schedule_to_file(); 
                break;
            case 6:
                // 공유 메모리 및 세마포어 정리 후 종료
                detach_shared_memory();
                remove_shared_memory();
                print_status("Shared memory and semaphore cleaned up. Exiting.");
                exit(0);
            default:
                print_error("Invalid choice. Try again.");
        }

        // 옵션 선택 후 추가적인 처리를 위해 버퍼를 비움
        while (getchar() != '\n'); // Enter 키를 기다림
    }

    return 0;
}
