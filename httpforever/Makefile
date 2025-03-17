CXX = g++
OUTPUT = httpforever
SRC = httpforever.cpp
OBJ = httpforever.o

$(OUTPUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUTPUT)

$(OBJ): $(SRC)
	$(CXX) -c $(SRC)

clean:
	rm -f $(OBJ) $(OUTPUT)

