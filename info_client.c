#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Sử dụng: %s <địa chỉ IP> <cổng>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Lỗi tạo socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Địa chỉ IP không hợp lệ!\n");
        close(client_socket);
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Kết nối đến server thất bại!\n");
        close(client_socket);
        return 1;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Lỗi lấy thư mục hiện tại.\n");
        close(client_socket);
        return 1;
    }

    int dir_len = strlen(cwd);
    send(client_socket, &dir_len, sizeof(int), 0);
    send(client_socket, cwd, dir_len, 0);

    DIR *dir = opendir(cwd);
    if (dir == NULL) {
        printf("Lỗi mở thư mục.\n");
        close(client_socket);
        return 1;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                int name_len = strlen(entry->d_name);
                long long file_size = file_stat.st_size;

                send(client_socket, &name_len, sizeof(int), 0);
                send(client_socket, entry->d_name, name_len, 0);
                send(client_socket, &file_size, sizeof(long long), 0);
            }
        }
    }

    int end_marker = 0;
    send(client_socket, &end_marker, sizeof(int), 0);

    printf("Đã gửi thông tin thư mục và danh sách tập tin thành công!\n");

    closedir(dir);
    close(client_socket);
    return 0;
}
