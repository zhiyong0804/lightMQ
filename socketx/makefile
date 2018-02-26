DIR_SRC = ./src
DIR_INC = ./include
DIR_TEST = ./tests
DIR_EXP = ./examples

SRC = $(wildcard ${DIR_SRC}/*.cpp)

VPATH = ./src
CFLAGS += -I$(DIR_INC) -std=c++11 -pthread

SRC_OBJ = $(patsubst %.cpp, %.o, ${SRC})

OBJ: $(SRC_OBJ)

${DIR_SRC}/%.o:$(DIR_SRC)/%.cpp
	g++ $(CFLAGS) -c $< -o $@

.PHONY: examples tests clean Lib 

lib: ./libs/libsocketx.a

./libs/libsocketx.a: $(SRC_OBJ)
	ar rcs $@ $^

examples: 
	for dir in $(DIR_EXP); do\
		(cd $$dir; ${MAKE});\
	done

tests: 
	for dir in $(DIR_TEST); do\
		(cd $$dir; ${MAKE});\
	done

clean:
	rm -f $(SRC_OBJ)
	for dir in $(DIR_EXP); do\
		(cd $$dir; ${MAKE} clean);\
	done
	for dir in $(DIR_TEST); do\
		(cd $$dir; ${MAKE} clean);\
	done
