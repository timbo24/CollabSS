

server:
	g++ -std=c++0x -o Server server.cpp session.cpp editor.cpp Circles/DependencyNode.cpp Circles/DependencyGraph.cpp Circles/CircleChecker.cpp /usr/local/stow/boost/boost_1_53_0/lib/libboost_*.a -I/usr/local/stow/boost/boost_1_53_0/include/ -lboost_thread-mt -lboost_system-mt -std=gnu++0x `mysql_config --cflags --libs` 

clean:
	rm -rf *.out Client Server

