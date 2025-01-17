test:unit_test.cc
	g++ -o $@ $^ -std=c++17 -lz -lstdc++fs -ljsoncpp
.PHONY:clean
clean:
	rm -f test
	rm -f log/*
	# rm -f *.dat