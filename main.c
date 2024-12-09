#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_UDP_DATA_LEN 128

int tcp_socket;
struct sockaddr_in tcp_server_addr;
char prefix[5];
FILE *log_file;

void log_event(const char *message)
{
    fprintf(log_file, "%s\n", message);
    fflush(log_file);
}

void *tcp_connection_handler(void *arg)
{
    while (1)
    {
        if (connect(tcp_socket, (struct sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr)) == -1)
        {
            log_event("TCP connection failed. Retrying...");
        }
        else
        {
            log_event("TCP connection established.");
            break;
        }
    }
    return NULL;
}

void send_data(const char *data, size_t len)
{
    if (send(tcp_socket, data, len, 0) == -1)
    {
        log_event("Failed to send data over TCP.");
    }
}

void udp_listener(int udp_socket)
{
    char buffer[MAX_UDP_DATA_LEN];
    struct sockaddr_in udp_client_addr;
    socklen_t addr_len = sizeof(udp_client_addr);

    while (1)
    {
        ssize_t received_len = recvfrom(udp_socket, buffer, MAX_UDP_DATA_LEN, 0,
                                        (struct sockaddr *)&udp_client_addr, &addr_len);
        if (received_len == -1)
        {
            log_event("Error receiving UDP data.");
            continue;
        }

        if (received_len < 16 || received_len > 128)
        {
            log_event("Received UDP data length out of bounds.");
            continue;
        }

        if (tcp_socket < 0)
        {
            log_event("TCP connection not established. Dropping UDP packet.");
            continue;
        }

        char data_to_send[MAX_UDP_DATA_LEN + 4];
        snprintf(data_to_send, sizeof(data_to_send), "%s%.*s", prefix, (int)received_len, buffer);
        send_data(data_to_send, strlen(data_to_send));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <UDP ip:port> <TCP ip:port> <log_file> <prefix>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse command line arguments
    char *udp_ip = strtok(argv[1], ":");
    char *udp_port_str = strtok(NULL, ":");
    char *tcp_ip = strtok(argv[2], ":");
    char *tcp_port_str = strtok(NULL, ":");
    strncpy(prefix, argv[4], 4);
    prefix[4] = '0';

    // Open log file
    log_file = fopen(argv[3], "a");
    if (!log_file)
    {
        perror("Failed to open log file");
        return EXIT_FAILURE;
    }

    // Create UDP socket
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0)
    {
        perror("Failed to create UDP socket");
        fclose(log_file);
        return EXIT_FAILURE;
    }

    // Bind UDP socket
    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = inet_addr(udp_ip);
    udp_addr.sin_port = htons(atoi(udp_port_str));

    if (bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0)
    {
        perror("Failed to bind UDP socket");
        close(udp_socket);
        fclose(log_file);
        return EXIT_FAILURE;
    }

    // Create TCP socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        perror("Failed to create TCP socket");
        close(udp_socket);
        fclose(log_file);
        return EXIT_FAILURE;
    }

    // Set up TCP server address
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_addr.s_addr = inet_addr(tcp_ip);
    tcp_server_addr.sin_port = htons(atoi(tcp_port_str)); // Start TCP connection handler in a separate thread
    pthread_t tcp_thread;
    pthread_create(&tcp_thread, NULL, tcp_connection_handler, NULL);

    // Start listening for UDP packets
    udp_listener(udp_socket);

    // Clean up
    close(udp_socket);
    fclose(log_file);
    return EXIT_SUCCESS;
}
