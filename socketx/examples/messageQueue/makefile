DIR_SRC = ../../src

SRC = $(wildcard ${DIR_SRC}/*.cpp)

VPATH = $(DIR_SRC)
CFLAGS += -I$(DIR_SRC) -std=c++11 -pthread -g

SRC_OBJ = $(patsubst %.cpp, %.o, ${SRC})

	
messageQueue: messageQueue.o ${SRC_OBJ} $(DIR_SRC)/utilx.hpp
	g++ $(CFLAGS) messageQueue.o ${SRC_OBJ} -o messageQueue


./%.o:./%.cpp
	g++ $(CFLAGS) -c $< -o $@

${DIR_SRC}/%.o:$(DIR_SRC)/%.cpp
	g++ $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_OBJ) ./*.o messageQueue