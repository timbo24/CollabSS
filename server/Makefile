

server:
	g++ -o Server Server.cpp -lboost_thread-mt -lboost_system-mt
client:
	g++ -o Client test_client.cpp -lboost_thread-mt -lboost_system-mt

clean:
	rm -rf *.out Client Server

SS:
	g++ -o SSTesting StringSocket.cpp SS_Tester.cpp -lboost_system-mt

Simple:
	g++ -o SimpleServer Simple_Test_Server.cpp -lboost_system-mt -lboost_thread-mt
