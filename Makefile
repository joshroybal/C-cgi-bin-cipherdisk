cipherdisk.cgi: cipherdisk.o
	gcc -lm libcgic.a -s -o cipherdisk.cgi cipherdisk.o

cipherdisk.o: cipherdisk.c
	gcc -O2 -c cipherdisk.c

install:
	sudo cp cipherdisk.cgi /srv/httpd/cgi-bin
	sudo chown apache:apache /srv/httpd/cgi-bin/cipherdisk.cgi
	sudo cp cipherdisk.html /srv/httpd/htdocs
	sudo chown apache:apache /srv/httpd/htdocs/cipherdisk.html

clean:
	rm cipherdisk.cgi && rm *.o
