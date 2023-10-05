.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c 
	gcc -o scheduler.out scheduler.c 

.PHONY: clean
clean: 
	rm -f scheduler.out

.PHONY: test , test2, test3, test4
test: scheduler.out
	./scheduler.out 0 FIFO fifo.in

test2: scheduler.out
	./scheduler.out 1 FIFO fifo.in

test3: scheduler.out
	./scheduler.out 0 SJF sjf.in

test4: scheduler.out
	./scheduler.out 1 SJF sjf.in

