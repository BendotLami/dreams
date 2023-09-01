CXX = g++
FLAGS = -std=c++20 -L -lboost_system -pthread -lboost_coroutine -fcoroutines

user: chat_client.cpp
	$(CXX) $(FLAGS) chat_client.cpp -o user

server: chat_server.cpp
	$(CXX) $(FLAGS) chat_server.cpp -o server