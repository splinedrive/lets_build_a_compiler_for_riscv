IN=main.c cradle.c
OUT=main
FLAGS=-g -Wall -Werror

all:
	gcc -o $(OUT) $(IN) $(FLAGS)

run:
	./$(OUT)

.PHONY: clean
clean:
	rm $(OUT)
