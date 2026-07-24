mainname = main

all:
	# compile the shaders to unsigned char arrays for c
	xxd -i src/shaders/default.frag > src/shaders/default_frag.h
	xxd -i src/shaders/default.vert > src/shaders/default_vert.h

	gcc -O2 -o build/$(mainname).exe \
		src/$(mainname).c \
		src/glad.c \
		-Iinclude \
		-Llib \
		-lopengl32 \
		-lglfw3dll \
		-lgdi32

	./build/main.exe