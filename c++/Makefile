CC = g++
CFLAGS = -g -O3
OBJ = main.o
#CNF.o random_set.o generator.o johnson.o main.o random_set.o unit_clause_with_majority.o unit_clause.o walksat.o

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	@rm $(OBJ)