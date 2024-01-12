#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int server_socket, client_sockets[MAX_CLIENTS];
GtkWidget *chat_view, *input_entry;

void append_text(const char *text) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, text, -1);
}

void start_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Accept a new connection
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Add the client socket to the array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_socket;
                break;
            }
        }

        // Receive and display messages
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
            if (recv_size <= 0) {
                // Client disconnected or error
                close(client_socket);
                break;
            }

            // Display the received message
            append_text(buffer);
        }
    }
}

void send_message(const char *message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
}

void on_send_clicked(GtkWidget *widget, gpointer data) {
    const char *message = gtk_entry_get_text(GTK_ENTRY(input_entry));
    send_message(message);
    append_text(message);  // Display the sent message
    gtk_entry_set_text(GTK_ENTRY(input_entry), "");
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);

    // Create chat view
    chat_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD_CHAR);

    // Create input entry
    input_entry = gtk_entry_new();

    // Create send button
    GtkWidget *send_button = gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), NULL);

    // Create vertical box
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), chat_view, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), input_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Connect the main window's destroy signal to gtk_main_quit
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Add the vertical box to the main window
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Show all the widgets in the window
    gtk_widget_show_all(window);

    // Start the server in a separate thread
    g_thread_new(NULL, (GThreadFunc)start_server, NULL);

    // Run the GTK main loop
    gtk_main();

    return 0;
}
