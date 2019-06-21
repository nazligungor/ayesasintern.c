
  #include <stdio.h>          
  #include <stdlib.h>        
  #include <string.h>         
  #include <fcntl.h>          
  #include <process.h>      
  #include <stddef.h>      
  #include <windows.h>      
  #include <sys\stat.h>     
  #include <io.h>           
  #pragma comment(lib,"ws2_32.lib")
#define OK_IMAGE  "HTTP/1.0 200 OK\r\nContent-Type:image/gif\r\n\r\n"
#define OK_TEXT   "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n"
#define NOTOK_404 "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define MESS_404  "<html><body><h1>FILE NOT FOUND</h1></body></html>"
#define  PORT_NUM            80     // Port number for Web server
#define  BUF_SIZE            4096     // Buffer size (big enough for a GET)
  void handle_get(void *in_arg);      // Windows thread function to handle GET

int main()
{
  WORD wVersionRequested = MAKEWORD(1,1);    
  WSADATA wsaData;                           
  int                  server_s;             // Server socket descriptor
  struct sockaddr_in   server_addr;          // Server Internet address
  int                  client_s;             // Client socket descriptor
  struct sockaddr_in   client_addr;          // Client Internet address
  struct in_addr       client_ip_addr;       // Client IP address
  int                  addr_len;             // Internet address length
  int                  retcode;              // Return code

  WSAStartup(wVersionRequested, &wsaData);
  server_s = socket(AF_INET, SOCK_STREAM, 0);
  if (server_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT_NUM);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  retcode = bind(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  listen(server_s, 100);

  printf(">>> weblite is running on port %d <<< \n", PORT_NUM);
  while(1)
  {
    addr_len = sizeof(client_addr);
    client_s = accept(server_s, (struct sockaddr *)&client_addr, &addr_len);
    if (client_s == -1)
    {
      printf("ERROR - Unable to create a socket \n");
      exit(1);
    }

    if (_beginthread(handle_get, 4096, (void *)client_s) < 0)

    {
      printf("ERROR - Unable to create a thread to handle the GET \n");
      exit(1);
    }
  }

  return(0);
}


void handle_get(void *in_arg)

{
  int            client_s;             // Client socket descriptor
  char           in_buf[BUF_SIZE];     // Input buffer for GET request
  char           out_buf[BUF_SIZE];    // Output buffer for HTML response
  int            fh;                   // File handle
  int            buf_len;              // Buffer length for file reads
  char           command[BUF_SIZE];    // Command buffer
  char           file_name[BUF_SIZE];  // File name buffer
  int            retcode;              // Return code

  client_s = (int) in_arg;

  retcode = recv(client_s, in_buf, BUF_SIZE, 0);

  if (retcode <= 0)
  {
    printf("ERROR - Receive failed --- probably due to dropped connection \n");

    closesocket(client_s);
    _endthread();
  }

  sscanf(in_buf, "%s %s \n", command, file_name);

  if (strcmp(command, "GET") != 0)
  {
    printf("ERROR - Not a GET --- received command = '%s' \n", command);
    closesocket(client_s);
    _endthread();

  }

  fh = open(&file_name[1], O_RDONLY | O_BINARY, S_IREAD | S_IWRITE);

  if (fh == -1)
  {
    printf("File '%s' not found --- sending an HTTP 404 \n", &file_name[1]);
    strcpy(out_buf, NOTOK_404);
    send(client_s, out_buf, strlen(out_buf), 0);
    strcpy(out_buf, MESS_404);
    send(client_s, out_buf, strlen(out_buf), 0);

    closesocket(client_s);
    _endthread();
  }

  printf("Sending file '%s' \n", &file_name[1]);
  if (strstr(file_name, ".gif") != NULL)
    strcpy(out_buf, OK_IMAGE);
  else
    strcpy(out_buf, OK_TEXT);
  send(client_s, out_buf, strlen(out_buf), 0);
  while(1)
  {
    buf_len = read(fh, out_buf, BUF_SIZE);
    if (buf_len == 0) break;
    send(client_s, out_buf, buf_len, 0);
  }

  close(fh);

    closesocket(client_s);
    _endthread();

}