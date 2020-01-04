#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void stream_to_stdout(int fd) {
  int chunk_size = 200; // arbitrary
  char buffer[chunk_size];

  // Put data in the buffer
  while (read(fd, buffer, sizeof(buffer)) > 0) {
    printf("%s", buffer);
    memset(buffer, 0, sizeof(buffer));
  }
}

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

  // We can't guarantee we can hold the entire response in memory (it's of indeterminate length), so
  // we stream it to STDOUT.
  stream_to_stdout(socket_fd);

  close(socket_fd);
  return EXIT_SUCCESS;
}
