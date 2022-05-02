CC = g++
src=$(wildcard *.cpp)
obj=$(patsubst %.cpp, %.o, $(src))
target=test
$(target):$(obj)
	$(CC) $^ -o $@ -pthread 
$(obj):$(src)
	$(CC) $^ -c

.PHONY:clean
clean:
	rm test $(obj)