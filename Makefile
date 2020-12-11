all: KKJserver

KKJserver: Asst3.c
	gcc Asst3.c -o KKJserver
clean:
	rm -f KKJserver
