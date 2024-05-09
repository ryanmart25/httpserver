#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "httpserve.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
// #define BUFFER_SIZE 1024
const char *get_file_name( char *path) {
    int i = 0;
    char *lastslash = NULL;
    for(i = 0; path[i] != '\0'; i++) {
        if(path[i] == '/') {
            lastslash = path + i;
        }
    }
    return lastslash;
}
int readAndCompare(int fd) // this is a method i made. I was tired of writing.
{
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    strcpy(buffer2, "EXIT");
    int rc = read(fd, buffer, sizeof(buffer));
    if (rc < 0)
    { // error reading
        return -1;
    }
    rc = strcmp(buffer, buffer2);
    if (rc == 0) // string1 == string2
    {

        return 0;
    }
    else if (rc == 1)
    { // string1 > string 2
        return 1;
    }
    return 1;
}
int main(int argc, char *argv[])
{
    // TODO: Parse command line arguments to override default port if necessary
    if(argc == 2) {
        int x = atoi(argv[1]);
        if(x == 0) {
            fprintf(stdout, "port must be an integer");
            return 0;
        }
        start_server(x);
    }
    else if(argc > 2) {
        fprintf(stdout, "usage: httpserver custom_port_number\n");
        return 0;
    }
    else {
        start_server(SERVER_PORT);
    }

    return 0;
}

void start_server(int port)
{
    // TODO: Implement server initialization:
    // 1. Create a socket.(welcome socket)
    // 2. Bind the socket to a port.
    // 3. Listen on the socket.
    // 4. Enter a loop to accept and handle connections.
    // most of this taken from the Beeg's guide to network programming.
    struct sockaddr_in my_addr; // set up for bind()
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
    // set up for the various calls
    // making socket
    int sock = create_socket(port);
    // binding
    int rc = bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)); // bind the socket to an address
    ;
    if (rc == -1)
    { // error check
        fprintf(stderr, "error binding: %s\n", strerror(errno));
        exit(1);
    }
    printf("binded\n");
    int listenrc = listen(sock, 10); // set the socket to listen.
    if (listenrc == -1)
    { // error check
        fprintf(stderr, "error listening: %s\n", strerror(errno));
        exit(1);
    }
    printf("listening\n");
    printf( "Server started. ready to accept connections on port: %i\n", port);
    while (1 == 1)
    {

        handle_connections(sock);
    }
}

int create_socket(int port)
{             // make the welcome socket
              //  TODO: Create and configure a TCP socket
              //  Return the socket descriptor
    int sock; // set up for socket()

    // setup for accept()

    sock = socket(PF_INET, SOCK_STREAM, 0); // create file descriptor
    if (sock < 0)
    { // error check
        fprintf(stderr, "error creating socket: %s\n", strerror(errno));
        exit(1);
    }
    return sock;
}

void handle_connections(int server_sock)
{
    // TODO: Implement connection handling:
    // 1. Accept a new connection.
    // 2. Handle the request in a separate function.
    // 3. Close the connection.
    struct sockaddr_in theiraddr;
    socklen_t sin_size;
    sin_size = sizeof(theiraddr);

    int clientSocket = accept(server_sock, (struct sockaddr *)&theiraddr, &sin_size);
    // parse request
    process_request(clientSocket);
    // close the connection
    close(clientSocket);
}

void process_request(int client_sock)
{
    // TODO: Implement request processing:
    // 1. Read the request from the client.
    // 2. Parse the HTTP method, path, and protocol version.
    // 3. Depending on the method, call handle_get_request, handle_head_request, or handle_post_request.
    // every line has a carriage return line feed at the end

    // read the request from the socket.
    char request[BUFFER_SIZE] = {0};                  // zero out the buffer
    int rc1 = read(client_sock, request, BUFFER_SIZE); // read client's request
    // check for the bad file strings
    if (strstr(request, "../") != NULL)
    {
        char *malformed = "HTTP/1.1 400 Bad Request\r\n";
         rc1 = send(client_sock, malformed, strlen(malformed), 0);
        if (rc1 == -1)
        {
            fprintf(stderr, "%s", strerror(errno)); // error check and close
            return;
        }
    }
    if (strstr(request, "//") != NULL)
    {
        char *malformed = "HTTP/1.1 400 Bad Request\r\n";
         rc1 = send(client_sock, malformed, strlen(malformed), 0);
        if (rc1 == -1)
        {
            fprintf(stderr, "%s", strerror(errno)); // error check and close
            return;
        }
    }
    if (rc1 == -1) // error check
    {
        fprintf(stderr, "error reading request. exiting\n");
        exit(1);
    }
    // print the request being served:
    fprintf(stdout, "received request: %s\n", request);
    char get[5];
    char post[6];
    char head[6];
    strcpy(head, "HEAD ");
    strcpy(get, "GET ");
    strcpy(post, "POST "); // setting up some buffers
    // parse the request.

    if (strncmp(request, get, 4) == 0) // if it is a get request
    {
        printf("succesfully parsed method\n");
        //TODO write a strstr to detect if there is a body. If yes, discard the body by placing a null terminating character at the start of the body.

        // carve a substring starting from the end of the method.
        // then, carve another substring out of that substring starting at the end of the path.
        // i have the start of the path, i can get the start of the protocol version. the end of the path is protocol version -1
        char path[BUFFER_SIZE];
        //char method[4];
        char protocol[BUFFER_SIZE];
         // i could not do this, it is roundabout, but to me its a little more readable.
        // searching for the first occurences of various characters or strings lets me carve up the request. I can figure out where to put null terminating characters from there, and then pass those pointers off to wherever I need them.
        printf("Line: 190. starting pointer allocation. Parsing start of path.\n");
        char *startofpath = strchr(request, '/');
        if (startofpath == NULL)
        { // send a malformed request response.
            char *malformed = "HTTP/1.1 400 Bad Request\r\n";
             rc1 = send(client_sock, malformed, strlen(malformed), 0);
            if (rc1 == -1)
            {
                fprintf(stderr, "%s", strerror(errno)); // error check and close
                close(client_sock);
            }
        }

        char *endofpath = strchr(startofpath, ' ');
        printf("Line: 204. Inserting null character for end of path\n");
        *endofpath = 0;
        char *version = endofpath + 1;
        //char *sp = request;
        char *endofmethod = strchr(request, ' ');
        printf("Line: 209. Inserting null character for end of method\n");
        *endofmethod = 0;
        // put the data into buffers. (fancy cool method i found on the interwebs: https://cplusplus.com/reference/cstdio/snprintf/)
        int n = snprintf(path, BUFFER_SIZE - 1, "%s", startofpath);
        //n = snprintf(method, 4, "%s", request);
        n = snprintf(protocol, BUFFER_SIZE - 1, "%s", version);
        if (n < 0)
        {
            fprintf(stderr, "Error writing request to buffers\n");
            exit(1);
        }
        printf("succesfully wrote to buffers. about to handle request\n");
        handle_get_request(client_sock, startofpath); // handle the request
        printf("succesfully handled get request\n");
        return;
    }
    /*else if (strncmp(request, post, 5) == 0) // TODO if it is a post request
    //{
    //    // set up some buffers, now that i've put null terminating characters in things, I'm wondering if this is necessary
    //    char path[BUFFER_SIZE];
    //    char method[5];
    //    char protocol[BUFFER_SIZE];
    //    char *startofmethod = request; // i could not do this, it is roundabout, but to me its a little more readable.
    //    // searching for the first occurences of various characters or strings lets me carve up the request. I can figure out where to put null terminating characters from there, and then pass those pointers off to wherever I need them.
    //    char *startofpath = strstr( startofmethod, "/");
    //    if (startofpath == NULL)
    //    { //  send a malformed request response.
    //        char *malformed = "HTTP/1.1 400 Bad Request\0";
    //         rc1 = send(client_sock, malformed, strlen(malformed), 0);
    //        if (rc1 == -1)
    //        {
    //            fprintf(stderr, "%s", strerror(errno)); // error check and close
    //            close(client_sock);
    //        }
    //    }
    //    char *endofmethod = startofpath - 1;
    //    *endofmethod = '\0';
    //    char *version = strstr( startofpath, "HTTP");
    //    char *endofpath = version - 1;
    //    *endofpath = '\0';
    //    char *body = strstr(request, "\r\n\r\n");
    //    if(body == NULL) {
    //        char *malformed = "HTTP/1.1 400 Bad Request\0";
    //        rc1 = send(client_sock, malformed, strlen(malformed), 0);
    //        if (rc1 == -1)
    //        {
    //            fprintf(stderr, "%s", strerror(errno)); // error check and close
    //            close(client_sock);
    //        }
    //    }
    //        // put the data into buffers. (fancy cool method i found on the interwebs: https://cplusplus.com/reference/cstdio/snprintf/)
    //        int n = snprintf(path, BUFFER_SIZE - 1, "%s", startofpath);
    //    n = snprintf(method, 4, "%s", startofmethod);
    //    n = snprintf(protocol, BUFFER_SIZE - 1, "%s", version);
    //    if (n < 0)
    //    {
    //        fprintf(stderr, "Error writing request to buffers\n");
    //        exit(1);
    //    }
    //    handle_post_request(client_sock, ps + 5);
    //    close(client_sock);
    //}
    //else if (strncmp(request, head, 5) == 0) // if it is a head request
    //{
    //    char path[BUFFER_SIZE];
    //    char method[4];
    //    char protocol[BUFFER_SIZE];
    //    char *startofmethod = request; // i could not do this, it is roundabout, but to me its a little more readable.
    //    // searching for the first occurences of various characters or strings lets me carve up the request. I can figure out where to put null terminating characters from there, and then pass those pointers off to wherever I need them.
    //    char *startofpath = strstr( startofmethod, "/");
    //    if (startofpath == NULL)
    //    { // TODO send a malformed request response.
    //        char *malformed = "HTTP/1.1 404 Not Found\0";
    //         rc1 = send(client_sock, malformed, strlen(malformed), 0);
    //        if (rc1 == -1)
    //        {
    //            fprintf(stderr, "%s", strerror(errno)); // error check and close
    //            close(client_sock);
    //        }
    //    }
    //    char *endofmethod = startofpath - 1;
    //    *endofmethod = '\0';
    //    char *version = strstr( startofpath, "HTTP");
    //    char *endofpath = version - 1;
    //    *endofpath = '\0';
    //
    //    // put the data into buffers. (fancy cool method i found on the interwebs: https://cplusplus.com/reference/cstdio/snprintf/)
    //    int n = snprintf(path, BUFFER_SIZE - 1, "%s", startofpath);
    //    n = snprintf(method, 4, "%s", startofmethod);
    //    n = snprintf(protocol, BUFFER_SIZE - 1, "%s", version);
    //    if (n < 0)
    //    {
    //        fprintf(stderr, "Error writing request to buffers\n");
    //        exit(1);
    //    }
    //    handle_head_request(client_sock, ps + 5); // send a bad request response;
    //    close(client_sock);
    *///}
    // TODO: gracefully decline malformed requests. Send a malformed request response with the send response method.
    char *malformed = "HTTP/1.1 400 Bad Request\r\n";
    rc1 = send(client_sock, malformed, strlen(malformed), 0);
    if (rc1 == -1)
    {
        fprintf(stderr, "%s", strerror(errno)); // error check and close
       return;
    }





    // close the socket, nothing more to do. (this will happen in main)
    // the idea for how to get the start/end of the method, path, protocol was taken from the following forum post: //EDIT: trying something else, since the method is at the front, I'm simply going to try using strncmp on the request. I didn't realize you could limit the window to a certain number of bytes
    // https://stackoverflow.com/questions/41286260/parse-http-request-line-in-c
    // get the addresses of the starts of the method, path, protocol
    // wait, if no parameters are given, does the '?' disapper? if so, what to do?will this fail? Are we even bothering with that?
    /*
    const char *startOfRequest = request; // this should decay into the address right?
    const char *endofRequest = strrchr(request, '\0') - 1;
    const char *startOfPath = strchr(request, ' ') + 1;
    const char *startOfQuery = strchr(startOfPath, '?');
    const char *startOfProtocol = strchr(startOfQuery, ' ') + 1;
    // compare method against these

    // allocate the memory
    char method[startOfPath - request];    // does this math work out?
    char path[startOfQuery - startOfPath]; // number of bytes to allocate for path = start of query - start of path
    /// Query: @1050, path @1010; 1050 - 1010 = 40 bytes
    char protocol[endofRequest - startOfProtocol];
    // put the info into the buffers
    strncpy(method, startOfRequest, startOfPath - request); // destination: method, source: start of request, number of bytes: start of path - start of request
    strncpy(path, startOfPath, startOfQuery - startOfPath);
    strncpy(protocol, startOfProtocol, endofRequest - startOfProtocol);

    method[sizeof(method)] = 0; // just learned 0 is the ascii value for null terminator. makes sense tbh, the more you know.
    path[sizeof(path)] = 0;
    protocol[sizeof(protocol)] = 0;
    if (strstr(path, "../") == NULL)
    { // reject the request
    }
    // call the appropriate method
    if (strcmp(method, get) == 0)
    {
        handle_get_request(client_sock, path);
        int rc = close(client_sock);
        if (rc < 0)
        {
            exit(1);
        }
    }
    if (strcmp(method, post) == 0)
    {
        handle_post_request(client_sock, path);
        int rc = close(client_sock);
        if (rc < 0)
        {
            exit(1);
        }
    }
    if (strcmp(method, head) == 0)
    {
        handle_head_request(client_sock, path);
        int rc = close(client_sock);
        if (rc < 0)
        {
            exit(1);
        }
    }
    */
}

void handle_get_request(int client_sock, const char *path)
{
//char notfoundheader[] = "HTTP/1.1 400 Not Found\0";
  //  char foundheader[] = "HTTP/1.1 200 OK\0";
//char unsupportedheader[] = "HTTP/1.1 415 Unsupported Media Type\0";
    // TODO: Handle GET request:
    // 1. Map the path to a file.
    int file;
    printf("About to open file\n");
    if(strcmp(path, "/") == 0) {
        //filepath = fopen("index.html", "r");
        file = open("www/index.html", O_RDONLY);
    }else {
        printf("about to use path pointer variable:%s\n", path);
        //filepath = fopen(path, "r");
        file = open(path, O_RDONLY);
    }
    char header[BUFFER_SIZE] = {0};
    // 2. Check if the file exists.
    if (file == -1) // if the file does not exist, set the not found header.
    {
        fprintf(stderr, "error opening file: %s\n", strerror(errno));
        strcpy(header, "HTTP/1.1 400 Not Found\n\r");
        int rc = send(client_sock, header, sizeof(header), 0);
        if(rc ==-1) {
            return;
        }
        printf("confirming header copied succesffuly: %s\n", header);
    }
    else { // file was found
        strcpy(header, "HTTP/1.1 200 OK\r\n");
        printf("copied file found header: %s\n", header);
    }
    // 3. Read the file and send it with an appropriate response header.
    char line[BUFFER_SIZE] = {0};
    size_t count = BUFFER_SIZE - 1;
    ssize_t n = read(file, line, count); // i don't believe a loop is necessary.
    if(n == -1){ // error check: if read failed, send internal server error.
        fprintf(stderr, "%s\n", strerror(errno)); //log the error.
        //strcpy(header, "HTTP/1.1 500 Internal Server Error\n\r"); // prepare header and send to client.
        char internalerror[] = "HTTP/1.1 500 Internal Server Error\r\n";
        int rc = send(client_sock, internalerror, sizeof(internalerror), 0);
        {
            if(rc == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
        }
        return; // return, and close the socket
    }

    /*while ((nread = getline(&line, &len, filepath)) != -1)
    //{
    //    printf("Retrieved line: %s\n", line);
    //    //fwrite(line, nread, 1, stdout); //instead of here, in send_response
    }
    */

    //attempting to parse the mime type
     const char *mime_type = get_mime_type(path);
    printf("succesfully parsed mimetype: %s\n", mime_type);
    if(strcmp(mime_type, "notsupported") == 0) { // if we don't recognize the file extension
        strcpy(header,  "HTTP/1.1 415 Unsupported Media Type\r\n"); //self explanatory
            //header = unsupportedheader;q
    }
    strcat(header, "Access-Control-Allow-Origin: *\r\nConnection: Keep-Alive\r\n");
    // build response;
    send_response(client_sock, header, mime_type, line, n);

   //free(line); not needed anymore cus I'm not using getline
    if(close(file) != 0) {
        close(client_sock);
        fprintf(stderr, "error closing File: %s\n", strerror(errno));
        exit(1);
    }



}

void handle_head_request(int client_sock, const char *path)
{
    // TODO: Handle HEAD request similarly to GET but without sending the file content.
}

void handle_post_request(int client_sock, const char *path)
{
    // TODO: Handle POST request:
    // 1. If the path is to a CGI script, execute the script and send the output as the response.
    // 2. If not, send a 404 Not Found response.
}

void send_response(int client_sock, const char *header, const char *content_type, const char *body, int body_length)
{       //append mime_type to the end of the header, then append \r\n\r\n to the end of the header.
    //then copy the header to response. then append the body to the end of the response
    // TODO: Compile and send a full HTTP response.
    // Include the header, content type, body, and any other necessary HTTP headers.
    //compile response
    int total_length = sizeof(header) + body_length + strlen(content_type);
    printf("%i\n",total_length);
    char response[BUFFER_SIZE] = {0};
    //char *res = (char *)malloc(total_length);
    //if(res == NULL) {
      //  return;
    //}
    //printf("%sContent-Length: %i\r\nContent-Type: %s\r\n\r\n%s", header, total_length, content_type, body);
    //print the headers, body into a buffer. (I've been clawing at the walls trying to get this to work and figured maybe strcat is messing it up)
    //snprintf(res, total_length + 1, "%s\r\nContent-Length: %i\r\nContent-Type: %s\r\n\r\n%s", header, total_length, content_type, body);

    strcpy(response,header);
    //build content-length header,
   // char lengthheader[50] ={0};
   // snprintf(lengthheader, 49, "Content-Length: %i\r\n", total_length);
    //strcat(response, lengthheader); //insert content-length header.
    strcat(response, content_type);//insert content type header
    strcat(response, "\r\n\r\n");
    strcat(response, body);
    printf("final request:\n");
    printf("%s\n", response);

    ssize_t bytessent = write(client_sock, response, total_length + 1);
    printf("%lu\n", bytessent);
    if(bytessent < total_length + 1) {
        bytessent = write(client_sock, response + bytessent, total_length + 1);
        printf("%lu\n", bytessent);
    }
    if(bytessent == -1) { //error check
        fprintf(stderr, "an error occurred while sending a response to client with socket: %i\n", client_sock);
        char *errorresponse = "HTTP/1.1 500 Internal Server Error\r\n";
        bytessent = send(client_sock, errorresponse, sizeof(errorresponse), 0); //inform the user of error
        if(bytessent == -1) { //  critical error, shutdown. TODO(false belief of error could be triggered by client closing the connection before sending)
            fprintf(stderr, "an error occurred while trying to inform the client of an internal server error. Line: 414\n");
            close(client_sock);
            exit(1);
        }
    }
    //free(res);
    printf("sent successfully\n");
}


const char *get_mime_type(const char *filename)
{
    // TODO: Return the MIME type based on the file extension.
    if (strstr(filename, ".png") != NULL)
    {
        return "Content-Type: image/png\n\0";

    }
    if(strcmp(filename, "/") == 0) {
        return "Content-Type: text/html\r\n";
    }
    if (strstr(filename, ".jpg")!= NULL)
    {
        return "Content-Type: image/jpeg";

    }
    if (strstr(filename, ".html")!= NULL)
    {
        return "Content-Type: text/html\r\n";

    }
    if (strstr(filename, ".js")!= NULL)
    {
        return "Content-Type: application/javascript\n\0";

    }
    if (strstr(filename, ".gif")!= NULL)
    {
        return "Content-Type: image/gif\n\0";

    }
    if (strstr(filename, ".txt")!= NULL)
    {

        return "Content-Type: text/plain; charset=utf-8\n\0";
    }
    if (strstr(filename, ".css")!= NULL)
    {

        return "Content-Type: text/css\n\0";
    }
    return "notsupported\n\0";
}
