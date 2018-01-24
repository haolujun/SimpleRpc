PROTO=/data/home/zhangxingbiao/usr/lib/protobuf
target: Test.o calculate.pb.o
	g++ -o target Test.o calculate.pb.o -L ${PROTO}/lib -lprotobuf
Test.o:
	g++ -g -c Test.cpp -I ${PROTO}/include -L ${PROTO}/lib -lprotobuf
calculate.pb.o:
	g++ -g -c calculate.pb.cc -L ${PROTO}/lib -I ${PROTO}/include -lprotobuf
clean:
	rm -rf *.o target
