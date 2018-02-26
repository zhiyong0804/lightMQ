DIR_SRC = ../../src

SRC = $(wildcard ${DIR_SRC}/*.cpp)

VPATH = $(DIR_SRC)
CFLAGS += -I$(DIR_SRC) -std=c++11 -pthread

SRC_OBJ = $(patsubst %.cpp, %.o, ${SRC})
	
EventLoop_test: Client_test.o ${SRC_OBJ}
	g++ $(CFLAGS) Client_test.o ${SRC_OBJ} -o Client_test

./%.o:./%.cpp
	g++ $(CFLAGS) -c $< -o $@

${DIR_SRC}/%.o:$(DIR_SRC)/%.cpp
	g++ $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_OBJ) ./*.o Client_test