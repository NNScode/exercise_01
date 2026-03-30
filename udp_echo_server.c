#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Sử dụng: %s <cổng>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        printf("Lỗi tạo socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Lỗi bind.\n");
        close(server_socket);
        return 1;
    }

    printf("UDP Echo Server đang lắng nghe ở cổng %d...\n", server_port);

    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Đã nhận và phản hồi: %s", buffer);
            
            sendto(server_socket, buffer, bytes_received, 0, (struct sockaddr *)&client_addr, client_len);
        }
    }

    close(server_socket);
    return 0;
}
