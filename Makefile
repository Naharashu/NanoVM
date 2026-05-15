CXX=clang++
CXX_FLAGS=-O2 -g1 -pipe -flto=thin -std=c++20 -Wall

all: main

main:
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit  -o nanovm

clean:
	rm -f ./nanovm
