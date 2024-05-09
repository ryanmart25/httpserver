#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "httpserve.h"
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

int findAString(void *buffer, char *msg, size_t n)
{

    if (strncmp(buffer, msg, n) != 0)
    {
        fprintf(stdout, "Receieved non-GET Request\n");
        return -1;
    }
    else
    {
        return 0;
    }
}
const char *get_file_name( char *path) {
        int i = 0;
        char *npath = path;
        char *lastslash = NULL;
        for(i = 0; path[i] != '\0'; i++) {
            if(path[i] == '/') {
                lastslash = npath + i;
            }
        }
        return lastslash;
    }
const char *get_mime_type(const char *filename)
{
    // TODO: Return the MIME type based on the file extension.
    if (strstr(filename, ".png") != NULL)
    {
        return "image/png\n\0";

    }
    if (strstr(filename, ".jpg")!= NULL)
    {
        return "image/jpeg\n\0";

    }
    if (strstr(filename, ".html")!= NULL)
    {
        return "text/html\n\0";

    }
    if (strstr(filename, ".js")!= NULL)
    {
        return "application/javascript\n\0";

    }
    if (strstr(filename, ".gif")!= NULL)
    {
   return "image/gif\n\0";

    }
    if (strstr(filename, ".txt")!= NULL)
    {

        return "text/plain\n\0";
    }
    if (strstr(filename, ".css")!= NULL)
    {

        return "text/css\n\0";
    }
    return "notsupported\n\0";
}
int main(int argc, char *argv[])
{
    printf("starting...\n");
    const char *mime_type = get_mime_type(argv[1]);
    printf("type: %s\n\n", mime_type);
     char line[BUFFER_SIZE] = {0};
     size_t count = BUFFER_SIZE - 1;
     int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        fprintf(stdout, "%s\n", strerror(errno));
        return -1;
    }
     ssize_t n = read(fd, line, count);
    if(n == -1){
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    if(close(fd) == -1){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    printf("type: %s\n\n%s\n\nExiting...", mime_type, line);
    return 0;

}
    //char *string = "GET / HTTP/1.1\0";
    //char *path = strchr(string, '/');
    //int i;
    //for(i=0; string[i] != '\0'; i++){
    //    printf("iterate: %i\n", i);
    //    if(string[i] == ' '){
    //        printf("What string looks like after insert: %s\n", path);
    //
    //        string[i] = '?';
    //        break;
    //    }
    //}

   //char sa[] = "GET /index.html HTTP/1.1\0";
   //
   //
   //    char *path = strchr(sa, '/');
   //    char *endofpath = strchr(path, ' ');
   //    *endofpath = 0;
   //    char *version = endofpath + 1;
   //    char *sp = sa;
   //    char *endofmethod = strchr(sa, ' ');
   //     *endofmethod = 0;
   //    printf("method: %s, path: %s, version: %s.\n", sa, path, version);
   //    return 0;


    //hit[(int) (strchr(hit, ' ') - hit)] = 0;

    //char *hit = strstr(string, "/");
    //char *endofpath = strstr(hit, " ");
    //endofpath = 0;   //now this doesn't actually cut off the last half of the string
    //endofpath--;
    //printf("the result: %s\n", hit);




    // readAndCompare(0);
    //char buffer[BUFFER_SIZE] = {0};
    //int valread = read(0, buffer, BUFFER_SIZE - 1);
    //if (valread <= 0)
    //{
    //    perror("Failed to read request");
    //    exit(1);
    //}
    //fprintf(stdout, "Result of find: %i\n", findAString(buffer, "GET ", 4));
    //exit(0);


/*
int readAndCompare(int fd)
{
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    strcpy(buffer2, "EXIT\n");
    int rc = read(fd, buffer, sizeof(buffer));
    if (rc < 0)
    {

        return -1
    }
    rc = strcmp(buffer, buffer2);
    if (rc == 0)
    {

        return 0;
    }
    else if (rc == 1)
    {

        return 1;
    }
}
*/