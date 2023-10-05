.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c 
	gcc -o scheduler.out scheduler.c 

.PHONY: clean
clean: 
	rm -f scheduler.out

.PHONY: test , test2
test: scheduler.out
	./scheduler.out 0 FIFO fifo.in

test2: scheduler.out
	./scheduler.out 1 FIFO fifo.in

