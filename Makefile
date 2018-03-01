CXXFLAGS = --std=c++14 -g

all: build

build: code

clean:
	rm -f code

.PHONY: clean
