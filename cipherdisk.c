#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "cgic.h"
#define SIZ 25
#define LIMIT 655360 /* Who will ever need more than 640k? */

const char MAP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void print_top(const char *, const char *, const char *);
void print_bottom(const char *, const char *, clock_t);
void print_form(char *, char []);
void cipherdisk(char *, const char []);
void teletype(char *);

int cgiMain()
{
   int n;
   char keystr[SIZ];
   char *buf;
   clock_t t1, t2;

   t1 = clock();
   printf("Content-Type: text/html\n\n");
   /* input */
   if (cgiContentLength > LIMIT) {  /* Who will ever need more than 640k? */
      print_top("sputnik", "cipherdisk", "/cipherdisk.html");
      puts("<pre>");
      puts("I'm sorry Dave, I'm afraid I can't do that.");
      printf("cgiContentLength = %d LIMIT = %d\n", cgiContentLength, LIMIT);
      puts("</pre>");
      print_bottom("Copyright (C) 2018 Josh Roybal", "/cipherdisk.html", t1);
      return(1);
   }
   cgiFormString("key", keystr, sizeof(keystr));
   cgiFormStringSpaceNeeded("message", &n);
   buf = (char *) malloc((n + 5) * sizeof(char));
   cgiFormString("message", buf, n);
   /* processing */
   cipherdisk(buf, keystr);
   n = strlen(buf);
   /* output */
   print_top("sputnik", "cipherdisk", "/cipherdisk.html");
   print_form(buf, keystr);
   print_bottom("Copyright (C) 2018 Josh Roybal", "/cipherdisk.html", t1);
   return 0;
}

/* emit top of html page */
void print_top(const char *title, const char *header, const char *backlink)
{
   puts("<!DOCTYPE html>");
   puts("<html>");
   puts("\t<head>");
   puts("\t\t<meta charset='US-ASCII'>");
   printf("\t\t<title>%s</title>\n", title);
   puts("\t\t<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
   puts("\t\t<link rel='stylesheet' media='all' type='text/css' href='/includes/style.css'/>");
   puts("\t</head>");
   puts("\t<body>");
   printf("\t\t<header><p>%s</p></header>\n", header);
	puts("\t\t\t<p></p>");
	printf("\t\t\t<div><a href='/index.html'>Home</a> | <a href='%s'>Back</a></div>\n", backlink);
	puts("\t\t\t<p></p>");
}

/* emit bottom of html page */
void print_bottom(const char *footer, const char *backlink, clock_t t1)
{
   char tstr[80];
   clock_t t2 = clock();
   sprintf(tstr, "Elapsed time %.3f seconds!", (t2-t1)/(float)CLOCKS_PER_SEC);
	puts("\t\t\t<p></p>");
	printf("\t\t\t<div><a href='/index.html'>Home</a> | <a href='%s'>Back</a></div>\n", backlink);
	puts("\t\t\t<p></p>");   
   printf("\t\t<footer><p>%s %s</p></footer>\n", tstr, footer);
   puts("\t</body>");
   puts("</html>");
}

/* emit html form */
void print_form(char *buf, char keystr[])
{
   puts("\t\t<form action='/cgi-bin/cipherdisk.cgi' method='POST'>");
   printf("\t\t\t<div><label>key : </label>");
   printf("<input type='text' name='key' size='25' value='%s'></div>\n", keystr);
   puts("\t\t\t<div>");
   puts("\t\t\t\t<textarea name='message' rows='12' cols='80'>");
   teletype(buf);
   puts("\t\t\t\t</textarea>");
   puts("\t\t\t</div>");
   puts("\t\t\t<div><input type='submit' name='data_entry'></div>");
   puts("\t\t</form>");
}

/* (progressive) cipherdisk encryptment/decryptment */
void cipherdisk(char *buf, const char keystr[])
{
   const int C = strlen(MAP);
   int i, j = 0, k = strlen(keystr), n = strlen(buf);
   int key[k];

   /* validate & constuct the key */
   for (i = 0; i < k; ++i) {
      if (isalnum(keystr[i])) {
         if (isdigit(keystr[i])) {
            key[j++] = keystr[i] - 22;
         } else if (islower(keystr[i])) {
            key[j++] = keystr[i] - 97;
         } else {
            key[j++] = keystr[i] - 65;
         }
      }
   }
   k = j;   /* reset key length to number of valid key elements processed */
   j = 0;   /* reset to zero after previous use */
   for (i = 0; i < n; ++i) {
      if (isalnum(buf[i])) {
         if (isdigit(buf[i])) {
            buf[j] = MAP[ (C - 1) - ((buf[i]) - 22 + key[j % k]) % C ];
         } else if (islower(buf[i])) {
            buf[j] = MAP[ (C - 1) - ((buf[i]) - 97 + key[j % k]) % C ];
         } else {
            buf[j] = MAP[ (C - 1) - ((buf[i]) - 65 + key[j % k]) % C ];
         }
         ++(key[j % k]) % C;  /* key progression */
         ++j;
      }
   }
   /* pad out to message length congruent mod 5 if necessary */
   if (j % 5 != 0)
      srand(time(NULL));
   while (j % 5 != 0) {
      buf[j++] = MAP[ rand() % C ];
   }
   buf[j] = '\0'; /* above in-place works because j <= i ! */
}

/* output message in clasic groups of 5 telegram format */
void teletype(char *outbuf)
{
   int i, n = strlen(outbuf);
   
   for (i = 0; i < n; ++i) {
      putchar(outbuf[i]);
      if ((i+1)%5 == 0) {
         if ((i+1)%25 == 0) {
            putchar('\n');
         } else {
            putchar(' ');
         }
      }
   }
   if (n % 25) {
      putchar('\n');
   }
}
