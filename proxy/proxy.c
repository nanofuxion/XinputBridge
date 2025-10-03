#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>

// --- Configuration ---
#define PROXY_HOST "127.0.0.1"
#define PORT 7947
#define BUFFER_SIZE 1024
#define IP_CHECK_INTERVAL 30 // seconds

// --- Globals ---
// Discovered IP of the remote Android device
char remote_server_ip[INET_ADDRSTRLEN];
// Address of the local client application (Xinput DLL)
struct sockaddr_in local_client_addr;
int has_local_client = 0;
// Mutex for thread-safe access to globals
pthread_mutex_t lock;

// --- Logging ---
void log_message(const char *message) {
    time_t now;
    time(&now);
    char buf[sizeof("2011-10-08T07:07:09Z")];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    printf("[%s] %s\n", buf, message);
    fflush(stdout);
}

// --- IP Discovery ---
void *ip_discovery_thread(void *arg) {
    char log_buf[256];

    while (1) {
        log_message("Searching for Android device IP...");
        FILE *fp = popen("ip -4 neigh show", "r");
        if (fp == NULL) {
            log_message("Failed to run 'ip neigh' command.");
            sleep(IP_CHECK_INTERVAL);
            continue;
        }

        char line[256];
        int found = 0;
        while (fgets(line, sizeof(line), fp) != NULL) {
            char ip[INET_ADDRSTRLEN];
            // Find an IP that is marked as REACHABLE or STALE
            if (strstr(line, "REACHABLE") != NULL || strstr(line, "STALE") != NULL) {
                if (sscanf(line, "%15s", ip) == 1) {
                    // Basic validation that it looks like an IP
                    if (inet_addr(ip) != INADDR_NONE) {
                        pthread_mutex_lock(&lock);
                        if (strcmp(remote_server_ip, ip) != 0) {
                            strncpy(remote_server_ip, ip, INET_ADDRSTRLEN);
                            remote_server_ip[INET_ADDRSTRLEN - 1] = '\0';
                            sprintf(log_buf, "Found potential Android device IP: %s", remote_server_ip);
                            log_message(log_buf);
                        }
                        pthread_mutex_unlock(&lock);
                        found = 1;
                        break;
                    }
                }
            }
        }
        pclose(fp);

        if (!found) {
            log_message("Could not find Android device IP. Will retry.");
        }
        sleep(IP_CHECK_INTERVAL);
    }
    return NULL;
}

// --- Main Proxy Logic ---
int main() {
    int sock;
    struct sockaddr_in proxy_addr, remote_server_addr;
    char buffer[BUFFER_SIZE];
    char log_buf[256];

    // Initialize mutex and globals
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    remote_server_ip[0] = '\0';
    has_local_client = 0;
    pthread_mutex_unlock(&lock);

    // Create IP discovery thread
    pthread_t tid;
    if (pthread_create(&tid, NULL, ip_discovery_thread, NULL) != 0) {
        log_message("Failed to create IP discovery thread.");
        return 1;
    }

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_message("Socket creation failed.");
        return 1;
    }

    // Bind proxy to localhost
    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = inet_addr(PROXY_HOST);
    proxy_addr.sin_port = htons(PORT);

    if (bind(sock, (const struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        sprintf(log_buf, "Bind failed on %s:%d. Error: %s", PROXY_HOST, PORT, strerror(errno));
        log_message(log_buf);
        return 1;
    }

    sprintf(log_buf, "UDP Proxy started. Listening on %s:%d", PROXY_HOST, PORT);
    log_message(log_buf);

    while (1) {
        struct sockaddr_in current_sender_addr;
        socklen_t len = sizeof(current_sender_addr);
        int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&current_sender_addr, &len);

        if (n < 0) {
            log_message("recvfrom failed.");
            continue;
        }

        pthread_mutex_lock(&lock);
        char current_remote_ip[INET_ADDRSTRLEN];
        strcpy(current_remote_ip, remote_server_ip);
        pthread_mutex_unlock(&lock);

        // Packet is from local Xinput client
        if (current_sender_addr.sin_addr.s_addr == inet_addr(PROXY_HOST)) {
            if (!has_local_client) {
                memcpy(&local_client_addr, &current_sender_addr, sizeof(local_client_addr));
                has_local_client = 1;
                log_message("Registered local client.");
            }
            if (strlen(current_remote_ip) > 0) {
                memset(&remote_server_addr, 0, sizeof(remote_server_addr));
                remote_server_addr.sin_family = AF_INET;
                remote_server_addr.sin_port = htons(PORT);
                remote_server_addr.sin_addr.s_addr = inet_addr(current_remote_ip);
                sendto(sock, buffer, n, 0, (const struct sockaddr *)&remote_server_addr, sizeof(remote_server_addr));
            }
        }
        // Packet is from remote Android server
        else if (strlen(current_remote_ip) > 0 && current_sender_addr.sin_addr.s_addr == inet_addr(current_remote_ip)) {
            if (has_local_client) {
                sendto(sock, buffer, n, 0, (const struct sockaddr *)&local_client_addr, sizeof(local_client_addr));
            }
        }
    }

    close(sock);
    pthread_mutex_destroy(&lock);
    return 0;
}