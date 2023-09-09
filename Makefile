CXX = g++-10
CHAT_FLAGS = -std=c++20 -L -lboost_system -pthread -lboost_coroutine -fcoroutines

DREAMS_SRCS = ./src/Cards.cpp ./src/Deck.cpp ./src/Game.cpp ./src/overload.cpp ./src/Player.cpp ./src/Queen.cpp dreams_server.cpp ./src/IOhandler.cpp ./src/Turn.cpp
DREAMS_FLAGS = --std=c++20 -g

TARGET_EXEC := sdreams

BUILD_DIR := .
SRC_DIR := ./src

OBJS := $(DREAMS_SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=-.d)

all: sdreams user clean_obj

user: ./src/chat_client.cpp
	$(CXX) $(CHAT_FLAGS) ./src/chat_client.cpp -o user

server: chat_server.cpp
	$(CXX) $(CHAT_FLAGS) ./src/chat_server.cpp -o server

# sdreams: $(DREAMS_HEADERS)
# 	$(CXX) $(DREAMS_FLAGS) $(CHAT_FLAGS) dreams_server.cpp -o sdreams

# sdreams server
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(DREAMS_FLAGS) $(CHAT_FLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(DREAMS_FLAGS) $(CHAT_FLAGS) -c $< -o $@

clean_obj: 
	rm -f $(OBJS)

clean: clean_obj
	rm -f $(BUILD_DIR)/$(TARGET_EXEC)