EXE = BeagleCode
FLAGS = -m23 -o
HEADER = -I /home/David/include/
LIB = -L /home/David/lib/

LIBS = 

all:	$(EXE)

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $(OBJS) $(LIBS) -o $(EXE)

main.o:	main.c
	$(CC) $(FLAGS) $(HEADER) $(LIB) main.c

clean:
	rm -f $(OBJS)
