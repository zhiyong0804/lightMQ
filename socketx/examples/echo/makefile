DIR_SRC = ../../src

SRC = $(wildcard ${DIR_SRC}/*.cpp)

VPATH = $(DIR_SRC)
CFLAGS += -I$(DIR_SRC) -std=c++11 -pthread -g

SRC_OBJ = $(patsubst %.cpp, %.o, ${SRC})

OBJ = EchoClient.o EchoServer.o 

echo: EchoClient EchoServer
	
EchoClient: EchoClient.o ${SRC_OBJ}
	g++ $(CFLAGS) EchoClient.o ${SRC_OBJ} -o EchoClient

EchoServer: EchoServer.o ${SRC_OBJ}
	g++ $(CFLAGS) EchoServer.o ${SRC_OBJ} -o EchoServer

./%.o:./%.cpp
	g++ $(CFLAGS) -c $< -o $@

${DIR_SRC}/%.o:$(DIR_SRC)/%.cpp
	g++ $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_OBJ) ./*.o EchoClient EchoServer