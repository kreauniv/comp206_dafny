
program: q1.c0 q2.c0 q3.c0 q4.c0 main.c0
	cc0 -o program -d --standard=c1 main.c0

run: program
	./program

submission:
	date -Iseconds > timestamp.txt
	chmod a-w timestamp.txt
	cd .. && tar zcvf $(shell whoami)-midterm.tar.gz midterm

sync:
	rsync * comp206:~/midterm/

help:
	less README.txt
