CC = g++
CFLAGS = -std=c++11 -Wall -g
LDFLAGS = -lfl

SRCDIR = src
INCDIR = inc
MISCDIR = misc

ASSEMBLER_SRCS = $(wildcard $(SRCDIR)/assembler*.cpp $(SRCDIR)/common.cpp)
ASSEMBLER_OBJS = $(patsubst $(SRCDIR)/%.cpp, %.o, $(ASSEMBLER_SRCS))
ASSEMBLER_PARSER = $(MISCDIR)/parser.tab.c
ASSEMBLER_LEXER = $(MISCDIR)/lex.yy.c
ASSEMBLER_PROGRAM = assembler

LINKER_SRCS = $(wildcard $(SRCDIR)/linker*.cpp $(SRCDIR)/common.cpp)
LINKER_OBJS = $(patsubst $(SRCDIR)/%.cpp, %.o, $(LINKER_SRCS))

LINKER_PROGRAM = linker

EMULATOR_SRCS = $(wildcard $(SRCDIR)/emulator*.cpp $(SRCDIR)/common.cpp)
EMULATOR_OBJS = $(patsubst $(SRCDIR)/%.cpp, %.o, $(EMULATOR_SRCS))
EMULATOR_PROGRAM = emulator

INPUT_FILE = $(MISCDIR)/test.txt

all: $(ASSEMBLER_PROGRAM) $(LINKER_PROGRAM) $(EMULATOR_PROGRAM)

$(ASSEMBLER_PROGRAM): $(ASSEMBLER_OBJS) $(ASSEMBLER_PARSER) $(ASSEMBLER_LEXER)
	$(CC) $(CFLAGS) -I$(INCDIR) $^ -o $@ $(LDFLAGS)

$(LINKER_PROGRAM): $(LINKER_OBJS) $(LINKER_PARSER) $(LINKER_LEXER)
	$(CC) $(CFLAGS) -I$(INCDIR) $^ -o $@ $(LDFLAGS)

$(EMULATOR_PROGRAM): $(EMULATOR_OBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) $^ -o $@ $(LDFLAGS)

%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(ASSEMBLER_PARSER): $(MISCDIR)/parser.y
	bison -o $@ -d $<

$(ASSEMBLER_LEXER): $(MISCDIR)/lexer.l
	flex -o $@ $<

$(LINKER_PARSER): $(MISCDIR)/linker_parser.y
	bison -o $@ -d $<

$(LINKER_LEXER): $(MISCDIR)/linker_lexer.l
	flex -o $@ $<

clean:
	rm -f $(ASSEMBLER_OBJS) $(ASSEMBLER_PARSER) $(ASSEMBLER_LEXER) \
	      $(LINKER_OBJS) $(LINKER_PARSER) $(LINKER_LEXER) \
	      $(EMULATOR_OBJS) $(ASSEMBLER_PROGRAM) $(LINKER_PROGRAM) $(EMULATOR_PROGRAM) \
	      *.o

run_assembler: $(ASSEMBLER_PROGRAM)
	./$(ASSEMBLER_PROGRAM) -o output.o $(INPUT_FILE)

run_linker: $(LINKER_PROGRAM)
	./$(LINKER_PROGRAM) -o output.o $(INPUT_FILE)

run_emulator:
