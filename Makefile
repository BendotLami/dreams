CXX = g++-10
CHAT_FLAGS = -std=c++20 -L -lboost_system -pthread -lboost_coroutine -fcoroutines

DREAMS_HEADERS = Cards.hpp Deck.hpp Game.hpp overload.hpp Player.hpp Queen.hpp
DREAMS_FLAGS = --std=c++20

user: chat_client.cpp
	$(CXX) $(CHAT_FLAGS) chat_client.cpp -o user

server: chat_server.cpp
	$(CXX) $(CHAT_FLAGS) chat_server.cpp -o server

sdreams: $(DREAMS_HEADERS)
	$(CXX) $(DREAMS_FLAGS) dreams_server.cpp -o sdreams