#include <stdio.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

#include "csapp.h"
#include "sbuf.h"
#include "cache.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *hostname, char *port, char *proxyuri);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
void serve_as_client(int fd, char *hostname, char *port, char *proxyuri, char *uri);

#define NTHREADS 4
#define SBUFSIZE 16
sbuf_t sbuf;

Cache *cache;

void *thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE);
    cache = cache_init(MAX_CACHE_SIZE);

    for (int i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);

    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
	sbuf_insert(&sbuf, connfd);
    }
}
/* $end tinymain */

void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);                                             //line:netp:tiny:doit
	    Close(connfd);                                            //line:netp:tiny:close
    }
}

/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd) 
{
    int valid;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], proxyuri[MAXLINE], port[MAXLINE];
    rio_t rio;
    Result *res;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))  //line:netp:doit:readrequest
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);       //line:netp:doit:parserequest
    if (strcasecmp(method, "GET")) {                     //line:netp:doit:beginrequesterr
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                    //line:netp:doit:endrequesterr
    read_requesthdrs(&rio);    
    if ((res = visit_line(cache, uri)) != NULL) {
        Rio_writen(fd, res->buf, res->size);
        Free(res->buf);
        Free(res);
        return;
    }
    /* Parse URI from GET request */
    valid = parse_uri(uri, hostname, port, proxyuri);       //line:netp:doit:staticcheck
    if (!valid) {
        printf("parse fail!\n");
    } else {
        serve_as_client(fd, hostname, port, proxyuri, uri);
    }
}
/* $end doit */

/*
 * read_requesthdrs - read HTTP request headers
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
    }
    return;
}
/* $end read_requesthdrs */

/*
 * parse_uri - parse URI into hostname, port and proxyuri
 *             return 0 if parse failure, 1 if success
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *hostname, char *port, char *proxyuri)
{
    char *ptr;
    int len;

    if ((ptr = strstr(uri, "http://")) != uri) return 0;
    uri += 7;
    if (!(ptr = strchr(uri, '/'))) return 0;
    len = ptr - uri;
    strncpy(hostname, uri, len);
    hostname[len] = '\0';
    uri += len;
    strcpy(proxyuri, uri);
    if ((ptr = strchr(hostname, ':'))) {
        *ptr = '\0';
        strcpy(port, ptr + 1);
    } else {
        strcpy(port, "80");
    }
    return 1;
}
/* $end parse_uri */

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */

void serve_as_client(int fd, char *hostname, char *port, char *proxyuri, char *uri) {
    rio_t rio;
    int clientfd;
    char buf[MAXLINE];
    char total_data[MAX_OBJECT_SIZE];
    char *ptr = total_data;
    
    clientfd = Open_clientfd(hostname, port);
    rio_readinitb(&rio, clientfd);
    sprintf(buf, "GET %s HTTP/1.0\r\n", proxyuri);
    Rio_writen(clientfd, buf, strlen(buf));
    sprintf(buf, "Host: %s\r\n", hostname);
    Rio_writen(clientfd, buf, strlen(buf));
    sprintf(buf, "User-Agent: %s\r\n", user_agent_hdr);
    Rio_writen(clientfd, buf, strlen(buf));
    sprintf(buf, "Connection: close\r\n");
    Rio_writen(clientfd, buf, strlen(buf));
    sprintf(buf, "Proxy-Connection: close\r\n\r\n");
    Rio_writen(clientfd, buf, strlen(buf));

    int total_len = 0;
    ssize_t len;
    while ((len = Rio_readnb(&rio, buf, MAXLINE))) {
        total_len += len;
        if (total_len <= MAX_OBJECT_SIZE) {
            memcpy(ptr, buf, len);
            ptr += len;
        }
        Rio_writen(fd, buf, len);
    }
    if (total_len <= MAX_OBJECT_SIZE) {
        while (cache->total_size + total_len > MAX_CACHE_SIZE)
            remove_line(cache); 
        add_line(cache, total_data, total_len, uri);
    }
    Close(clientfd);
}