
INCLUDE_DIR=include

CPPFLAGS=-I$(INCLUDE_DIR)


all: libt2fs.a

libt2fs.a: t2fs.o list.o
	@ar crs ./lib/libt2fs.a t2fs.o list.o ./lib/libapidisk.a
	@echo "\nlibt2fs compiled.\n"

t2fs.o: src/t2fs.c ./include/t2fs.h ./include/apidisk.h 
	@gcc -c ./src/t2fs.c $(CPPFLAGS)

list.o: ./src/list.c ./include/list.h
	@gcc -c ./src/list.c $(CPPFLAGS)

clean:
	@rm -rf *.o *~ ./include/*~ ./src/*~
