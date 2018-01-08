
#====Variables====

EXTERNAL_DIR=../common_files
CC = gcc

HEADER = 

OBJS = 

LOBJS = client_tcp.o 

PROG = client_tcp






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






client_tcp.o: client_tcp.c

	${CC} -Wall -c client_tcp.c


