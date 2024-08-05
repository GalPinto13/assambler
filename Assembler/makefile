# Basic compilation macros
C = gcc # Compiler -> gcc
FLAGS = -g -ansi -Wall -pedantic # Flags
GLOBAL_DEP = global.h # Dependencies for everything
EXE_DEPS = assembler.o free_mem.o file_writer.o s_pass.o f_pass.o ins_anal.o gui_anal.o macro_reader.o text_parser.o utils.o #Dependencies for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEP)
	$(C) $(FLAGS) $(EXE_DEPS) -lm -o $@

## Main:
assembler.o: assembler.c assembler.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c assembler.c -o $@

## Free memory:
free_mem.o: free_memory.c free_memory.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c free_memory.c -o $@

## File writer:
file_writer.o: file_writer.c file_writer.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c file_writer.c -o $@

## Second pass:
s_pass.o: second_pass.c second_pass.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c second_pass.c -o $@

## First pass:
f_pass.o: first_pass.c first_pass.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c first_pass.c -o $@

## Ins Analyzer:
ins_anal.o: ins_analyzer.c ins_analyzer.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c ins_analyzer.c -o $@

## Gui Analyzer:
gui_anal.o: gui_analyzer.c gui_analyzer.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c gui_analyzer.c -o $@

## Macro Reader:
macro_reader.o: macro_reader.c macro_reader.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c macro_reader.c -o $@

## Text Parser:
text_parser.o: text_parser.c text_parser.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c text_parser.c -o $@

## Utilities:
utils.o: utils.c utils.h $(GLOBAL_DEP)
	$(C) $(FLAGS) -c utils.c -lm -o $@

## Clean:
clean:
##	del *.o  ## Windows clean command
	rm -rf *.o


## delete del *.o once finished! only used to clean on windows
