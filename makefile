all:
	g++ `pkg-config --cflags gtk+-3.0` main.c `pkg-config --libs gtk+-3.0` -lpthread