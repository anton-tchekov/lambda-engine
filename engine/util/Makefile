all: shader_packer shader_checker

shader_packer:
	gcc shader_packer.c -I../src ../src/readfile.c -o shader_packer -Wall -Wextra -Wconversion -g

shader_checker:
	gcc shader_checker.c \
	-I../src/ \
	-I../src/io \
	-DSHADER_COMPILE_FILE_USED \
	../src/readfile.c \
	../src/shader.c \
	../src/lib/glad.c \
	../src/log.c \
	../src/debug.c \
	../src/internal.c \
	../src/thread.c \
	-DCOLOR_TERMINAL \
	-o shader_checker -ldl -pthread -lglfw -lGL -lopenal -Wall -Wextra -g -DDEBUG
