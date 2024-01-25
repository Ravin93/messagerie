#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.1.26"  // Adresse IP du serveur 
#define PORT 12345
#define BUFFER_SIZE 1024

GtkWidget *messages_view;
GtkWidget *message_entry;

int client_socket;

void append_message(const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, message, -1);

    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(messages_view), &iter, 0.0, FALSE, 0.0, 1.0);
}

void send_message() {
    const char *message = gtk_entry_get_text(GTK_ENTRY(message_entry));
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Error sending message");
          } else {
        append_message("Mac: ");
        append_message(message);
        append_message("\n");
        gtk_entry_set_text(GTK_ENTRY(message_entry), "");  
    }
}

void *receive_messages(void *arg) {
    while (1) {
        char buffer[BUFFER_SIZE];
        ssize_t recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (recv_size <= 0) {
            break;
        }
        buffer[recv_size] = '\0';  
        append_message(buffer);
    }
    return NULL;
}

void start_client() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    gtk_init(NULL, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Client");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    messages_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_window), messages_view);

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);
    pthread_detach(receive_thread);

    message_entry = gtk_entry_new();
    g_signal_connect(message_entry, "activate", G_CALLBACK(send_message), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);  // Utiliser le conteneur de défilement
    gtk_box_pack_start(GTK_BOX(vbox), message_entry, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    gtk_main();
}

int main() {
    start_client();
    close(client_socket);
    return 0;
}

