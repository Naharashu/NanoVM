CXX=g++
CXX_FLAGS=-O2 -g1 -pipe -flto

all: main

main:
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit  -o nanovm

clean:
	rm -f ./nanovm
