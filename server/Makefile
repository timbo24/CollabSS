

server:
	g++ -o server ss_server.cpp -pthread

2server:
	g++ -o Server server.cpp /usr/local/stow/boost/boost_1_53_0/lib/libboost_*.a -I/usr/local/stow/boost/boost_1_53_0/include/ -lboost_thread-mt -lboost_system-mti -std=gnu++0x

3server:
	g++ -o Server server.cpp -lboost_thread-mt -lboost_system-mt

client:
	g++ -o Client test_client.cpp -lboost_thread-mt -lboost_system-mt

clean:
	rm -rf *.out Client Server

SS:
	g++ -o SSTesting StringSocket.cpp SS_Tester.cpp -lboost_system-mt

Simple:
	g++ -o SimpleServer Simple_Test_Server.cpp -lboost_system-mt -lboost_thread-mt
