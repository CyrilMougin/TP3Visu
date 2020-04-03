all:main

main:
	g++ main.cpp glad/glad.c -lglfw -lGL -lX11 -lpthread -lXrandr -ldl -o TP3_exe
clean:
	rm TP3_exe
