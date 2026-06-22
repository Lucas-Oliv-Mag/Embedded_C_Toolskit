#===============================================================================
# Compiler
#===============================================================================

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas
INC     := -Iinc

#===============================================================================
# Project
#===============================================================================

# Corrigido: Agora aponta para um executável .exe
TARGET      := rb_test.exe

LIB_SRC     := src/ringbuffer.c
# Aqui você escolhe qual arquivo de teste quer compilar (ex: rb_push_pull_test.c)
TEST_SRC    := tests/rb_initialization_test.c

OBJ         := $(LIB_SRC:.c=.o) $(TEST_SRC:.c=.o)

#===============================================================================
# Targets
#===============================================================================

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# Ajustado o comando 'clean' para funcionar no Windows nativo (PowerShell/CMD)
clean:
	del /f $(subst /,\,$(OBJ)) $(TARGET) 2>nul || true