DIR_HEADERS = ./include/
DIR_SRC = ./src/
DIR_TEST = ./teste/
DIR_LIB = ./lib/
DIR_BIN = ./bin/

FLAGS = -Wall

default: lib

lib: t2fs.o tools.o disk_tools.o read_only_tools.o
	ar crs $(DIR_LIB)libt2fs.a t2fs.o tools.o disk_tools.o read_only_tools.o $(DIR_LIB)libapidisk.a

test: print_reg_block print_reg_superblock dirt2

dirt2: $(DIR_TEST)dirt2.c
	gcc -o $(DIR_BIN)dirt2 $(DIR_TEST)dirt2.c -L$(DIR_LIB) -lt2fs -lapidisk $(FLAGS) -I$(DIR_HEADERS)

init_fd_table: init_fd_table.o tools.o disk_tools.o read_only_tools.o 
	gcc init_openfd_record.o tools.o disk_tools.o read_only_tools.o $(DIR_LIB)libapidisk.a -o $(DIR_BIN)init_opendfd_record 

init_fd_table.o: $(DIR_SRC)init_openfd_record.c
	gcc -c $(DIR_SRC)init_openfd_record.c -o init_openfd_record.o -I$(DIR_HEADERS) $(FLAGS)

t2fs.o: $(DIR_SRC)t2fs.c 
	gcc -c $(DIR_SRC)t2fs.c -o t2fs.o -I$(DIR_HEADERS) $(FLAGS)

tools.o: $(DIR_SRC)tools.c 
	gcc -c $(DIR_SRC)tools.c -o tools.o -I$(DIR_HEADERS) $(FLAGS)

disk_tools.o: $(DIR_SRC)disk_tools.c 
	gcc -c $(DIR_SRC)disk_tools.c -o disk_tools.o -I$(DIR_HEADERS) $(FLAGS)

read_only_tools.o: $(DIR_SRC)read_only_tools.c 
	gcc -c $(DIR_SRC)read_only_tools.c -o read_only_tools.o -I$(DIR_HEADERS) $(FLAGS)

clean:
	rm *.o
	rm $(DIR_LIB)libt2fs.a
	rm $(DIR_BIN)*

print_reg_block: $(DIR_TEST)print_register_from_block.c
	gcc -o $(DIR_BIN)print_register_from_block $(DIR_TEST)print_register_from_block.c $(DIR_LIB)libapidisk.a $(FLAGS) -I$(DIR_HEADERS)

print_reg_superblock: $(DIR_TEST)print_register_from_superblock.c
	gcc -o $(DIR_BIN)print_register_from_superblock $(DIR_TEST)print_register_from_superblock.c $(DIR_LIB)libapidisk.a $(FLAGS) -I$(DIR_HEADERS)  
