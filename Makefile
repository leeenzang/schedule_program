CC = gcc
CFLAGS = -Wall -g
OBJECTS = main.o schedule.o shared_memory.o
EXPORT_SCHEDULE_OBJ = export_schedule.o

all: schedule_program export_schedule

schedule_program: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

export_schedule: $(EXPORT_SCHEDULE_OBJ)
	$(CC) $(CFLAGS) -o $@ $(EXPORT_SCHEDULE_OBJ)

main.o: main.c schedule.h
	$(CC) $(CFLAGS) -c main.c

schedule.o: schedule.c schedule.h
	$(CC) $(CFLAGS) -c schedule.c

shared_memory.o: shared_memory.c schedule.h
	$(CC) $(CFLAGS) -c shared_memory.c

export_schedule.o: export_schedule.c schedule.h
	$(CC) $(CFLAGS) -c export_schedule.c

clean:
	rm -f $(OBJECTS) $(EXPORT_SCHEDULE_OBJ) schedule_program export_schedule
