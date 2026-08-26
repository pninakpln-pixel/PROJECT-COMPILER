assembler: assembler.o pre_assembler.o first_pass.o second_pass.o helpers.o table.o macro.o
	gcc -Wall -ansi -pedantic assembler.o pre_assembler.o first_pass.o second_pass.o helpers.o table.o macro.o -o assembler

assembler.o: assembler.c globals.h helpers.h macro.h pre_assembler.h second_pass.h table.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c macro.h pre_assembler.h helpers.h globals.h
	gcc -c -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o

first_pass.o: first_pass.c globals.h helpers.h table.h first_pass.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

second_pass.o: second_pass.c table.h second_pass.h helpers.h globals.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

helpers.o: helpers.c globals.h helpers.h
	gcc -c -Wall -ansi -pedantic helpers.c -o helpers.o

table.o: table.c table.h globals.h helpers.h
	gcc -c -Wall -ansi -pedantic table.c -o table.o

macro.o: macro.c globals.h macro.h
	gcc -c -Wall -ansi -pedantic macro.c -o macro.o

clean:
	rm -f *.o assembler *.am *.ob *.ent *.ext