# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -g

# 객체 파일 목록
OBJECTS = main.o schedule.o shared_memory.o
EXPORT_SCHEDULE_OBJ = export_schedule.o

# 기본 목표
all: schedule_program export_schedule

# schedule_program 실행 파일 생성
schedule_program: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# export_schedule 실행 파일 생성
export_schedule: $(EXPORT_SCHEDULE_OBJ)
	$(CC) $(CFLAGS) -o $@ $(EXPORT_SCHEDULE_OBJ)

# main.c 컴파일 규칙
main.o: main.c schedule.h
	$(CC) $(CFLAGS) -c main.c

# schedule.c 컴파일 규칙
schedule.o: schedule.c schedule.h
	$(CC) $(CFLAGS) -c schedule.c

# shared_memory.c 컴파일 규칙
shared_memory.o: shared_memory.c schedule.h
	$(CC) $(CFLAGS) -c shared_memory.c

# export_schedule.c 컴파일 규칙
export_schedule.o: export_schedule.c schedule.h
	$(CC) $(CFLAGS) -c export_schedule.c

# 깨끗하게 정리하는 규칙
clean:
	rm -f $(OBJECTS) $(EXPORT_SCHEDULE_OBJ) schedule_program export_schedule
