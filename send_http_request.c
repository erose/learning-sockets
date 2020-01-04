#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
  char* port = "80"; // default HTTP port

  struct addrinfo* res;
  getaddrinfo("google.com", port, NULL, &res);

  // Use the socket syscall to get a file descriptor.

  // SOCK_STREAM --> this means we're using TCP. Julia Evans says the alternatives are SOCK_DGRAM,
  // which means we're using UDP, and SOCK_RAW, which means "just let me send IP packets, I will
  // implement my own protocol."

  // IPPROTO_TCP --> also means "we're using TCP". Theoretically there might be other options here
  // but I don't know of any.
  int socket_fd = socket(res->ai_family, SOCK_STREAM, IPPROTO_TCP);

  // Ask Linux to connect, on the socket we just created, to a given IP and port.

  // This assigns a local port number to a socket, and sends packets in order to establish a TCP
  // connection with the remove server.
  connect(socket_fd, res->ai_addr, res->ai_addrlen);

  char* data = "GET / HTTP/1.1\r\nHost: google.com \r\nConnection: close\r\n\r\n"; // a minimal HTTP GET request
  write(socket_fd, data, strlen(data));

  // TODO: Can we read the whole response?
  int amount_of_response_to_read = 1000 * sizeof(char);  
  char* response = malloc(amount_of_response_to_read);
  read(socket_fd, response, amount_of_response_to_read);

  printf("%s", response);

  free(response);
  close(socket_fd);
  return EXIT_SUCCESS;
}
