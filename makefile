CC=g++
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

all: pmt

pmt: $(OBJ)
	@mkdir -p bin
	g++ $(OBJ) -o bin/pmt

$(OBJ): $(SRC)
	@mkdir -p $(@D)
	g++ -c $(patsubst obj/%.o, src/%.cpp, $@) -o $@

clean:
	rm -rf obj/*
