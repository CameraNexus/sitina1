//
// Sitina JTAG Untility
//
// Copyright 2024 Wenting Zhang <zephray@outlook.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Contains code from:
// - https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-
//   from-serial-port-in-c
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <fcntl.h> 
#include <termios.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#define PORT            2542
#define MCU_MAX_SIZE    1024 // Max size the MCU can handle

#define ASSERT(x, s)   { if (!(x)) { perror(s); exit(1); } }
#define GET_TIME()   { time(&rawtime); timeinfo = localtime(&rawtime); \
    sprintf(timestr, "%02d:%02d:%02d", timeinfo->tm_hour, \
    timeinfo->tm_min, timeinfo->tm_sec); }

typedef enum {
    TOK_NONE, TOK_GETINFO, TOK_SETTCK, TOK_SHIFT
} tok_t;

static int sockfd, connfd, serialfd;
static int max_packet_size = 0;

static bool fd_getbuf(int connfd, char *buf, int bufsize, char stopchar) {
    int i;
    for (i = 0; i < bufsize - 1; i++) {
        if (read(connfd, &buf[i], 1) != 1)
            return false;
        if (buf[i] == stopchar)
            break;
    }
    buf[i + 1] = '\0';
    return true;
}

static bool fd_trywrite(int connfd, char *buf, int requestdsize) {
    int remaining = requestdsize;
    int byteswritten = 0;
    while (remaining > 0) {
        ssize_t result = write(connfd, &buf[byteswritten], remaining);
        if (result >= 0) {
            byteswritten += result;
            remaining -= result;
        }
        else {
            return false;
        }
    }
    return true;
}


static bool fd_tryread(int connfd, char *buf, int requestdsize) {
    int remaining = requestdsize;
    int bytesread = 0;
    while (remaining > 0) {
        ssize_t result = read(connfd, &buf[bytesread], remaining);
        if (result >= 0) {
            bytesread += result;
            remaining -= result;
        }
        else {
            return false;
        }
    }
    return true;
}

static bool xvc_getcmd(int connfd, tok_t *token) {
    char cmd[12];
    if (!fd_getbuf(connfd, cmd, sizeof(cmd), ':'))
        return false;
    if (memcmp(cmd, "getinfo:", 8) == 0)
        *token = TOK_GETINFO;
    else if (memcmp(cmd, "settck:", 7) == 0)
        *token = TOK_SETTCK;
    else if (memcmp(cmd, "shift:", 6) == 0)
        *token = TOK_SHIFT;
    else
        *token = TOK_NONE;
    return true;
}

static uint32_t getuint32(uint8_t *buf) {
    return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static int set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    int result = tcgetattr(fd, &tty);
    ASSERT(result == 0, "Error during tcgetattr");

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    result = tcsetattr(fd, TCSANOW, &tty);
    ASSERT(result == 0, "Error during tcsetattr");

    return 0;
}

static void set_blocking(int fd, int should_block) {
    struct termios tty;
    int result;
    memset(&tty, 0, sizeof tty);
    result = tcgetattr(fd, &tty);
    ASSERT(result == 0, "Error during tcgetattr");

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 10;

    result = tcsetattr(fd, TCSANOW, &tty);
    ASSERT(result == 0, "Error during tcsetattr");
}


static void cleanup(void) {
    printf("Max packet size ever seen: %d bytes\n", max_packet_size);
    if (connfd)
        close(connfd);
    if (sockfd)
        close(sockfd);
    if (serialfd)
        close(serialfd);
}

void sigint_handler(int dummy) {
    cleanup();
    exit(0);
}

uint64_t time_in_ms(void) {
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    return (uint64_t)s * 1000 + ms;
}

int main()  {
    int cli_len, result;
    struct sockaddr_in serv_addr, cli_addr;
    time_t rawtime;
    struct tm * timeinfo;
    char timestr[10];
    uint8_t buf[1024]; // Generic buffer

    char *serialport = "/dev/ttyACM0";

    printf("Sitina JTAG Debug Bridge\n");

    signal(SIGINT, sigint_handler);

    serialfd = open(serialport, O_RDWR | O_NOCTTY | O_SYNC);
    ASSERT(serialfd >= 0, "Failed to open serial port");

    set_interface_attribs(serialfd, B921600, 0);
    set_blocking(serialfd, 1); // blocking

    bzero(&serv_addr, sizeof(serv_addr));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(sockfd != -1, "Socket creation failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    result = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    ASSERT(result == 0, "Socket bind failed");

    result = listen(sockfd, 5);
    ASSERT(result == 0, "Listen failed");

    cli_len = sizeof(cli_addr);

    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
        ASSERT(connfd >= 0, "Accept failed");

        GET_TIME();
        printf("[%s] Accepted connection\n", timestr);

        while (1) {
            tok_t token;
            if (!xvc_getcmd(connfd, &token))
                break;
            if (token == TOK_GETINFO) {
                GET_TIME();
                printf("[%s] Received Getinfo\n", timestr);
                // Send to MCU
                if (!fd_trywrite(serialfd, "getinfo:", 8))
                    goto serialfail;
                if (!fd_getbuf(serialfd, buf, sizeof(buf), '\n'))
                    goto serialfail;
                GET_TIME();
                printf("[%s] MCU returned info %s", timestr, buf);

                if (write(connfd, buf, strlen(buf)) != strlen(buf))
                    break;
            }
            else if (token == TOK_SETTCK) {
                uint32_t period;
                if (read(connfd, buf, 4) != 4)
                    break;
                period = getuint32(buf);
                GET_TIME();
                printf("[%s] Set TCK frequency to %d kHz\n",
                    timestr, 1000000 / period);
                if (!fd_trywrite(serialfd, "settck:", 7))
                    goto serialfail;
                if (!fd_trywrite(serialfd, buf, 4))
                    goto serialfail;
                if (!fd_tryread(serialfd, buf, 4))
                    goto serialfail;
                period = getuint32(buf);
                GET_TIME();
                printf("[%s] Actual TCK frequency %d kHz\n",
                    timestr, 1000000 / period);
                if (write(connfd, buf, 4) != 4)
                    break;
            }
            else if (token == TOK_SHIFT) {
                uint32_t num_bits;
                if (read(connfd, buf, 4) != 4)
                    break;
                num_bits = getuint32(buf);
                uint32_t num_bytes = (num_bits + 7) / 8;
                GET_TIME();
                printf("[%s] Shift %d bits (%d bytes)\n",
                    timestr, num_bits, num_bytes);
                uint8_t *tms_buf = malloc(num_bytes);
                uint8_t *tdi_buf = malloc(num_bytes);
                uint8_t *tdo_buf = malloc(num_bytes);
                ASSERT(tms_buf, "Allocation failed");
                ASSERT(tdi_buf, "Allocation failed");
                ASSERT(tdo_buf, "Allocation failed");
                if (read(connfd, tms_buf, num_bytes) != num_bytes)
                    break;
                if (read(connfd, tdi_buf, num_bytes) != num_bytes)
                    break;

                int remaining = num_bytes;
                if (num_bytes > max_packet_size)
                    max_packet_size = num_bytes;
                int pos = 0;
                while (remaining > 0) {
                    int size = (remaining > MCU_MAX_SIZE) ?
                        MCU_MAX_SIZE : remaining;
                    uint64_t start_ms = time_in_ms();
                    if (!fd_trywrite(serialfd, "shift:", 6))
                        goto serialfail;
                    if (!fd_trywrite(serialfd, buf, 4))
                        goto serialfail;
                    uint64_t ms1 = time_in_ms();
                    if (!fd_trywrite(serialfd, &tms_buf[pos], size))
                        goto serialfail;
                    if (!fd_trywrite(serialfd, &tdi_buf[pos], size))
                        goto serialfail;
                    uint64_t ms2 = time_in_ms();
                    if (!fd_tryread(serialfd, &tdo_buf[pos], size))
                        goto serialfail;
                    uint64_t ms3 = time_in_ms();
                    printf("%d ms + %d ms + %d ms = %d ms to send %d bytes (%.1f KB/s)\n",
                        (int)(ms1 - start_ms), (int)(ms2 - ms1), (int)(ms3 - ms2),
                        (int)(ms3 - start_ms), size, (float)size / (ms3 - start_ms));
                    pos += size;
                    remaining -= size;
                }

                if (write(connfd, tdo_buf, num_bytes) != num_bytes)
                    break;
                // Potential memory leak here, not a big issue hopefully
                free(tms_buf);
                free(tdi_buf);
                free(tdo_buf);
            }
        }

        close(connfd);
        printf("Closed connection\n");
    }
    goto exit;

serialfail:
    fprintf(stderr, "Failed reading from /writing to serial port\n");

exit:
    cleanup();

    // Not reachable
    //close(sockfd);

}
