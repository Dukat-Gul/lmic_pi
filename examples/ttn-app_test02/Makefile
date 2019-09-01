CFLAGS=-I../../lmic
LDFLAGS=-lwiringPi

ttn-app_test01: ttn-app_test02.cpp
	cd ../../lmic && $(MAKE)
	$(CC) $(CFLAGS) -o ttn-app_test02 ttn-app_test02.cpp ../../lmic/*.o $(LDFLAGS)

all: ttn-app_test02

.PHONY: clean

clean:
	rm -f *.o ttn-app_test02
