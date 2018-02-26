DIR_SRC = ./src
DIR_LIB = ./socketx
DIR_INC = ./socketx/include

SRC = $(wildcard ${DIR_SRC}/*.cpp)
LIB = $(wildcard ${DIR_LIB}/*.cpp)

VPATH = $(DIR_LIB)/src
#CFLAGS += -I$(DIR_INC) -std=c++11 -pthread -L$(DIR_LIB)/libs $(DIR_LIB)/libs/libsocketx.a -lglog -lzookeeper_mt -g 
CFLAGS += -I$(DIR_INC) -std=c++11 -pthread -L$(DIR_LIB)/libs $(DIR_LIB)/libs/libsocketx.a -lglog -g

SRC_OBJ = $(patsubst %.cpp, %.o, ${SRC})

OBJ = ${DIR_SRC}/PubSub.o ${DIR_SRC}/Publisher.o ${DIR_SRC}/Subscriber.o ${DIR_SRC}/Protocol.o

#OBJ = ${DIR_SRC}/PubSub.o ${DIR_SRC}/Publisher.o ${DIR_SRC}/Subscriber.o ${DIR_SRC}/Protocol.o $(DIR_SRC)/ZooKeeperService.o

all: PubSubSys PublisherClient SubscriberClient

PubSubSys: $(SRC_OBJ)
	g++ ${DIR_SRC}/PubSubSys.o $(OBJ) $(CFLAGS) -o ${DIR_SRC}/PubSubSys

PublisherClient: $(SRC_OBJ)
	g++ ${DIR_SRC}/PublisherClient.o $(OBJ) $(CFLAGS) -o ${DIR_SRC}/PublisherClient

SubscriberClient: $(SRC_OBJ)
	g++ ${DIR_SRC}/SubscriberClient.o $(OBJ) $(CFLAGS) -o ${DIR_SRC}/SubscriberClient

${DIR_SRC}/%.o:$(DIR_SRC)/%.cpp
	g++ $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_OBJ) $(DIR_SRC)/PubSubSys $(DIR_SRC)/PublisherClient $(DIR_SRC)/SubscriberClient
