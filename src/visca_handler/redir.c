#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int open_serial_raw(char * name)
{
  int fd;
  struct termios options;

  fd = open(name, O_RDWR | O_NDELAY | O_NOCTTY);

  if(fd == -1)
    {
      fprintf(stderr, "Could not open serial port %s\n", name);
      return fd;
    }

  fcntl(fd, F_SETFL, 0);
  tcgetattr(fd, &options);
  cfsetispeed(&options, B9600); //9600 bps
  options.c_cflag &= ~PARENB;   //no parity
  options.c_cflag &= ~CSTOPB;   //1 stop bit
  options.c_cflag &= ~CSIZE;    //clear size mask
  options.c_cflag |= CS8;       //set size mask to 8 bit
  options.c_cflag &= ~CRTSCTS;  //no hardware rts/cts

  //disable canonical mode, echo, erase characters and
  //just about all the other processing.
  //AKA disable all local processing so we get raw input
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  options.c_iflag = 0; //clear all input flags

  options.c_oflag &= ~OPOST; //clear all output flags
  tcsetattr(fd, TCSANOW, &options);

  return fd;
}

int main(int argc, char ** argv)
{
  int input_fd;
  int output_fd;
  
  input_fd = open_serial_raw(argv[1]);
  output_fd = open_serial_raw(argv[2]);

  if(input_fd == -1 || output_fd == -1) return 1;
  
  if(fork())
    {
      int bytes_read_in;
      while(1)
        {
          char buf;
          bytes_read_in = read(input_fd, &buf, 1);
          write(output_fd, &buf, 1);
          fprintf(stderr, "%s 0x%02x\n", argv[1], buf);
        }
    }
  else
    {
      while(1)
        {
          char buf;
          read(output_fd, &buf, 1);
          write(input_fd, &buf, 1);
          fprintf(stderr, "%s 0x%02x\n", argv[2], buf);
        }
    }

  return 0;
}
