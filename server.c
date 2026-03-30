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

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(server_socket, 5) < 0) {
        printf("Lỗi listen.\n");
        close(server_socket);
        return 1;
    }

    printf("Server đang lắng nghe ở cổng %d...\n", server_port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_socket < 0) {
            continue;
        }

        char leftover[10] = {0};
        int total_occurrences = 0;
        const char *target = "0123456789";
        int target_len = 10;

        char buffer[1024];
        int bytes_received;

        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';

            char temp[2048] = {0};
            strcpy(temp, leftover);
            strcat(temp, buffer);

            char *ptr = temp;
            while ((ptr = strstr(ptr, target)) != NULL) {
                total_occurrences++;
                ptr += target_len;
            }

            printf("Tổng số lần xuất hiện hiện tại: %d\n", total_occurrences);

            int temp_len = strlen(temp);
            if (temp_len < target_len - 1) {
                strcpy(leftover, temp);
            } else {
                strcpy(leftover, temp + temp_len - (target_len - 1));
            }
        }
        
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
