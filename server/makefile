
#====Variables====

EXTERNAL_DIR=../common_files
CC = gcc

HEADER = filelist_util.h

OBJS = 

LOBJS = server_tcp.o filelist_util.o

PROG = server_tcp






#Call "make" with any of these labels in front to execute (default will be all)

all:	${PROG}



#removes the program and the objects and files that are created from compiling and executing the program

clean:

		rm ${PROG} ${FILES} ${LOBJS}



#clears the console then runs the program

play:

		clear

		sudo ./${PROG}





#=====Compilation======

${PROG}:	${OBJS} ${LOBJS}

			${CC} -Wall   ${HEADER} ${OBJS} ${LOBJS} -o ${PROG}






server_tcp.o: server_tcp.c

	${CC} -Wall -c server_tcp.c


filelist_util.o: filelist_util.c

	${CC} -Wall -c filelist_util.c