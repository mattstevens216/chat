all: crc crsd

crc.o: crc.c interface.h
	g++ -c -pthread crc.c -o crc.o

crsd.o: crsd.c interface.h
	g++ -pthread -c crsd.c -o crsd.o

crc: crc.o
	g++ -pthread crc.o -o crc

crsd: crsd.o
	g++ -pthread crsd.o -o crsd

clean:
	-rm -rf *.o crc crsd

