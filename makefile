CC=gcc
OBJ= client.o server.o chat.o main.o

all : stnc

stnc: $(OBJ)
	$(CC) -Wall -g $(OBJ) -o stnc -lssl -lcrypto

main.o: main.c Client.h Server.h Chat.h
	$(CC) -Wall -g -c $< -o main.o

server.o: Srver.c Server.h
	$(CC) -Wall -g -c $< -o server.o -lssl -lcrypto

client.o: Client.c Client.h
	$(CC) -Wall -c $< -o client.o -lssl -lcrypto



chat.o: Chat.c Chat.h
	$(CC) -Wall -g -c $< -o chat.o 

.PHONY = clean all 

clean:
	rm -f *.o *.a stnc *.txt echo_socket received_file Rec_File