CC = gcc
CFLAGS = -g -Wall -std=c99 -fopenmp -mavx -mfma -pthread
LDFLAGS = -fopenmp
CUNIT = -L/usr/lib/x86_64-linux-gnu -I/usr/include/CUnit -lcunit
PYTHON = -I/usr/include/python3.10 -lpython3.10

install:
	if [ ! -f files.txt ]; then touch files.txt; fi
	if [ ! -f dumb_files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt
	python3 setup.py install --record files.txt
	#python3 setup_dumbpy.py install --record dumb_files.txt

uninstall:
	if [ ! -f files.txt ]; then touch files.txt; fi
	if [ ! -f dumb_files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt
	xargs rm -rf < dumb_files.txt

clean:
	rm -f *.o
	rm -f test
	rm -rf build
	rm -rf __pycache__

test:
	rm -f test
	$(CC) $(CFLAGS) mat_test.c matrix.c -o test $(LDFLAGS) $(CUNIT) $(PYTHON)
	./test

.PHONY: test