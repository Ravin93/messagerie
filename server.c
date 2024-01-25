#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <openssl/aes.h> //pour chiffre avec AES a voir 
#include <stdint.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    char server_ip[16];  
    int port;
    char messages_file[256];  
} Config;

typedef struct {
    int index;
    int socket;
} ClientInfo;

int server_socket, connected_clients = 0;
ClientInfo client_sockets[MAX_CLIENTS];

GtkWidget *messages_view;
GtkWidget *connected_label;
GtkWidget *memory_label;
GtkWidget *ip_label;
GtkWidget *notebook;

FILE *message_file;

Config read_config(const char *filename) {
    Config config;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    // Lecture des paramètres du fichier
    int read_count = fscanf(file, "SERVER_IP=%15s\n", config.server_ip);  // Lire au plus 15 caractères pour l'adresse IP
    if (read_count != 1) {
        fprintf(stderr, "Error reading SERVER_IP from configuration file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "PORT=%d\n", &config.port);
    fscanf(file, "MESSAGES_FILE=%s\n", config.messages_file);

    fclose(file);
    return config;
}


void update_label_text(const char *text, GtkWidget *label) {
    gtk_label_set_text(GTK_LABEL(label), text);
}

void update_connected_clients() {
    char label_text[50];
    sprintf(label_text, "Connected clients: %d", connected_clients);
    update_label_text(label_text, connected_label);
}

void update_ip_label_text(const char *ip_text) {
    update_label_text(ip_text, ip_label);
}

void append_message(const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
    GtkTextIter iter;

    gtk_text_buffer_get_end_iter(buffer, &iter);

    char formatted_message[BUFFER_SIZE];
    snprintf(formatted_message, sizeof(formatted_message), "%s\n", message);

    gtk_text_buffer_insert(buffer, &iter, formatted_message, -1);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(messages_view),
                                  gtk_text_buffer_get_insert(buffer),
                                  0.0, FALSE, 0.0, 1.0);
    
    // Enregistrez également le message dans le fichier
    if (message_file != NULL) {
        fprintf(message_file, "%s\n", message);
        fflush(message_file);
    }
}


void send_to_all_clients(const char *message, int sender_index) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i].socket != 0 && i != sender_index) {
            send(client_sockets[i].socket, message, strlen(message), 0);
        }
    }
}

gboolean append_message_idle(gpointer data) {
    const char *message = (const char *)data;
    append_message(message);
    return FALSE;
}

void send_messages_to_client(int client_socket) {
    FILE *message_file = fopen("messages.txt", "r");
    if (message_file != NULL) {
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, sizeof(buffer), message_file) != NULL) {
            send(client_socket, buffer, strlen(buffer), 0);
        }
        fclose(message_file);
    } else {
        perror("Error opening messages.txt for reading");
    }
}

void *handle_client(void *arg) {
    ClientInfo *client_info = (ClientInfo *)arg;

    int client_index = client_info->index;
    int client_socket = client_info->socket;

    char buffer[BUFFER_SIZE];
    ssize_t recv_size;

    while ((recv_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recv_size] = '\0';
        printf("Received message from client %d: %s\n", client_index, buffer);

        g_idle_add(append_message_idle, g_strdup(buffer));

        send_to_all_clients(buffer, client_index);
    }

    close(client_socket);

    connected_clients--;
    update_connected_clients();

    client_sockets[client_index].socket = 0;
    client_sockets[client_index].index = 0;

    free(client_info);

    return NULL;
}

void update_ip_label(const char *ip_text) {
    update_label_text(ip_text, ip_label);
}

void load_messages_from_file() {
    message_file = fopen("messages.txt", "r");
    if (message_file != NULL) {
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, sizeof(buffer), message_file) != NULL) {
            append_message(buffer);
        }
        fclose(message_file);
    } else {
        perror("Error opening messages.txt for reading");
        exit(EXIT_FAILURE);
    }
}

void *start_server(void *arg) {
    (void)arg;

    Config config = read_config("config.ini");
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(config.server_ip);
    server_addr.sin_port = htons(config.port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    update_ip_label_text("Server IP: Not available");

    load_messages_from_file();

    message_file = fopen("messages.txt", "a");
    if (message_file == NULL) {
        perror("Error opening messages.txt for writing");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i].socket == 0) {
                client_sockets[i].socket = client_socket;
                client_sockets[i].index = i;

                connected_clients++;
                update_connected_clients();

                update_ip_label_text("Server IP: Connected");

                ClientInfo *client_info = malloc(sizeof(ClientInfo));
                memcpy(client_info, &client_sockets[i], sizeof(ClientInfo));

                pthread_t thread;
                pthread_create(&thread, NULL, handle_client, client_info);
                pthread_detach(thread);

                break;
            }
        }
    }

    fclose(message_file);

    return NULL;
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    const char *config_filename = (argc > 1) ? argv[1] : "config.ini";
    Config config = read_config(config_filename);

    printf("Server IP: %s\n", config.server_ip);
    printf("Port: %d\n", config.port);
    printf("Messages file: %s\n", config.messages_file);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Server Dashboard");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);

    notebook = gtk_notebook_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    messages_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

    gtk_container_add(GTK_CONTAINER(scrolled_window), messages_view);

    connected_label = gtk_label_new("Connected clients: 0");

    ip_label = gtk_label_new("Server IP: Not available");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), memory_label, FALSE, FALSE, 0);

    GtkWidget *messages_tab_label = gtk_label_new("Messages");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, messages_tab_label);

    GtkWidget *clients_tab_label = gtk_label_new("Connected Clients");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), connected_label, clients_tab_label);

    GtkWidget *ip_tab_label = gtk_label_new("Server IP");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), ip_label, ip_tab_label);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, start_server, NULL);

    gtk_main();

    return 0;
}
