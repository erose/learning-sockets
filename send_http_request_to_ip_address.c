#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
  int port_number = 80; // default HTTP port
  char* ip_address = "172.217.11.14"; // current result of `dig google.com`

  // Use the socket syscall to get a file descriptor.

  // AF_INET --> this means that the socket can communicate with IPv4 addresses. Julia Evans says
  // the main alternative is AF_UNIX, which would be a Unix domain socket for communicating with
  // other Unix processes. There is also AF_INET6 for IPv6 addresses.

  // SOCK_STREAM --> this means we're using TCP. Julia Evans says the alternatives are SOCK_DGRAM,
  // which means we're using UDP, and SOCK_RAW, which means "just let me send IP packets, I will
  // implement my own protocol."

  // IPPROTO_TCP --> also means "we're using TCP". Theoretically there might be other options here
  // but I don't know of any.
  int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // Create the data structure that will hold the ip address.
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof sa);

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port_number); // htons is used to convert to the correct byte order, which may be different on the network.
  inet_pton(AF_INET, ip_address, &sa.sin_addr); // Do the conversion between string and IP address structure.

  // Ask Linux to connect, on the socket we just created, to a given IP and port.

  // This assigns a local port number to a socket, and sends packets in order to establish a TCP
  // connection with the remove server.
  connect(socket_fd, (struct sockaddr *)&sa, sizeof sa);

  char* data = "GET / HTTP/1.1\r\nHost: google.com \r\nConnection: close\r\n\r\n"; // a minimal HTTP GET request
  write(socket_fd, data, strlen(data));

  // We can't guarantee we can hold the entire response in memory (it's of indeterminate length), so
  // we stream it to STDOUT.
  stream_to_stdout(socket_fd);

  close(socket_fd);
  return EXIT_SUCCESS;
}
