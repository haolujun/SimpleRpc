depends=Network.o Reactor.o Thread.o EpollReactorImplementation.o SimpleRpc.o
libsimplerpc.so: ${depends}
	g++ ${depends} -shared -fPIC -o ./lib/libsimplerpc.so
Reactor.o:
	g++ -g -c -fPIC ./src/Reactor.cpp -I ./include
EpollReactorImplementation.o:
	g++ -g -c -fPIC ./src/EpollReactorImplementation.cpp -I ./include
Network.o:
	g++ -g -c -fPIC ./src/Network.cpp -I ./include
#Scarch.o:
#	g++ -g -c -fPIC ./src/Scarch.cpp -I ./include

SimpleRpc.o:
	g++ -g -c -fPIC ./src/SimpleRpc.cpp -I ./include
BlockQueue.o:
	g++ -g -c -fPIC ./src/BlockQueue.cpp -I ./include
Thread.o:
	g++ -g -c -fPIC ./src/Thread.cpp -I ./include
clean:
	rm -rf *.o ./lib/libsimplerpc.so
