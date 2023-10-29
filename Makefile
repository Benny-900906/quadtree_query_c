# the first target:
dict4: dict4.o stage.o quadtree.o dict.o data.o
	gcc -o dict4 dict4.o stage.o quadtree.o dict.o data.o

dict3: dict3.o stage.o quadtree.o dict.o data.o
	gcc -o dict3 dict3.o stage.o quadtree.o dict.o data.o

# Other targets specify how to create .o files and what they rely on
dict4.o: dict4.c 
	gcc -c dict4.c

dict3.o: dict3.c
	gcc -c dict3.c

stage.o: stage.c stage.h
	gcc -c stage.c

quadtree.o: quadtree.c quadtree.h 
	gcc -c quadtree.c
	
dict.o: dict.c dict.h
	gcc -c dict.c

data.o: data.c data.h  
	gcc -c data.c 

clean:
	rm -f *.o dict3 dict4