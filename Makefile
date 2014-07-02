DIR_HEADERS = ./include/
DIR_SRC = ./src/
DIR_TEST = ./teste/
DIR_LIB = ./lib/
DIR_BIN = ./bin/

FLAGS = -Wall

default: lib

lib: t2fs.o list.o
	ar crs $(DIR_LIB)libt2fs.a t2fs.o list.o $(DIR_LIB)libapidisk.a

test: dirt2 copy2t2 mkdirt2 rmdirt2

dirt2: $(DIR_TEST)dirt2.c
	gcc -o $(DIR_BIN)dirt2 $(DIR_TEST)dirt2.c -L$(DIR_LIB) -lt2fs -lapidisk $(FLAGS) -I$(DIR_HEADERS)

copy2t2: $(DIR_TEST)copy2t2.c
	gcc -o $(DIR_BIN)copy2t2 $(DIR_TEST)copy2t2.c -L$(DIR_LIB) -lt2fs -lapidisk $(FLAGS) -I$(DIR_HEADERS)

mkdirt2: $(DIR_TEST)mkdirt2.c
	gcc -o $(DIR_BIN)mkdirt2 $(DIR_TEST)mkdirt2.c -L$(DIR_LIB) -lt2fs -lapidisk $(FLAGS) -I$(DIR_HEADERS)

rmdirt2: $(DIR_TEST)rmdirt2.c
	gcc -o $(DIR_BIN)rmdirt2 $(DIR_TEST)rmdirt2.c -L$(DIR_LIB) -lt2fs -lapidisk $(FLAGS) -I$(DIR_HEADERS)

t2fs.o: $(DIR_SRC)t2fs.c
	gcc -c $(DIR_SRC)t2fs.c -o t2fs.o -I$(DIR_HEADERS) $(FLAGS)

list.o: $(DIR_SRC)list.c
	gcc -c $(DIR_SRC)list.c -o list.o -I$(DIR_HEADERS) $(FLAGS)

clean:
	rm *.o
	rm $(DIR_LIB)libt2fs.a
	rm $(DIR_BIN)*
