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

        int dir_len = 0;
        if (recv(client_socket, &dir_len, sizeof(int), 0) <= 0) {
            close(client_socket);
            continue;
        }

        char cwd[1024] = {0};
        recv(client_socket, cwd, dir_len, 0);
        
        printf("%s\n", cwd);

        while (1) {
            int name_len = 0;
            if (recv(client_socket, &name_len, sizeof(int), 0) <= 0) {
                break;
            }

            if (name_len == 0) {
                break;
            }

            char file_name[1024] = {0};
            recv(client_socket, file_name, name_len, 0);

            long long file_size = 0;
            recv(client_socket, &file_size, sizeof(long long), 0);

            printf("%s - %lld bytes\n", file_name, file_size);
        }
        
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
