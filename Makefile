.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c 
	gcc -g -Wall -o $@ $<

.PHONY: clean
clean: 
	rm -f scheduler.out

.PHONY: test
test: scheduler.out
	./scheduler.out 0 FIFO fifo.in