all: Project2.o
	touch outputFile.log
	gcc Project2.o -o outputFile

Project2.0: Project2.c
	gcc -c -Ipthread Project2.c

run: 
	./outputFile $(seed)

report:
	cat outputFile.log

.PHOYNY: clean

clean:
	rm -f Project2.o outputFile outputFile.log