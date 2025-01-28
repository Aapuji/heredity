CC := gcc
FILES := main.c person.c

heredity.exe: $(FILES)
	$(CC) -o heredity $(FILES)

person.c: person.h
