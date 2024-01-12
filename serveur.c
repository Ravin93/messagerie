//LIGNE DE COMMANDE 

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 12345
// #define MAX_CLIENTS 10
// #define BUFFER_SIZE 1024
// server_addr.sin_addr.s_addr = inet_addr("172.20.10.2"); 

// int server_socket, client_sockets[MAX_CLIENTS];

// void start_server() {
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t addr_len = sizeof(client_addr);

//     // Create server socket
//     server_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_socket == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     // Configure server address
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     // Bind the socket
//     if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error binding socket");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections
//     if (listen(server_socket, MAX_CLIENTS) == -1) {
//         perror("Error listening for connections");
//         exit(EXIT_FAILURE);
//     }

//     while (1) {
//         // Accept a new connection
//         int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
//         if (client_socket == -1) {
//             perror("Error accepting connection");
//             continue;
//         }

//         // Add the client socket to the array
//         for (int i = 0; i < MAX_CLIENTS; i++) {
//             if (client_sockets[i] == 0) {
//                 client_sockets[i] = client_socket;
//                 break;
//             }
//         }

//         // Receive and display messages
//         char buffer[BUFFER_SIZE];
//         while (1) {
//             memset(buffer, 0, sizeof(buffer));
//             ssize_t recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
//             if (recv_size <= 0) {
//                 // Client disconnected or error
//                 close(client_socket);
//                 break;
//             }

//             // Display the received message
//             printf("Received message: %s", buffer);
//         }
//     }
// }

// int main() {
//     start_server();
//     return 0;
// }


//INTERFACE GRAPHIQUE 

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <pthread.h>
// #include <gtk/gtk.h>

// #define PORT 12345
// #define MAX_CLIENTS 10
// #define BUFFER_SIZE 1024

// typedef struct {
//     int index;
//     int socket;
// } ClientInfo;

// int server_socket, connected_clients = 0;
// ClientInfo client_sockets[MAX_CLIENTS];

// GtkWidget *messages_view;
// GtkWidget *connected_label;
// GtkWidget *memory_label;
// GtkWidget *ip_label;  // Nouveau label pour afficher l'IP
// GtkWidget *notebook;  // Nouveau notebook pour gérer les onglets

// void update_label_text(const char *text, GtkWidget *label) {
//     gtk_label_set_text(GTK_LABEL(label), text);
// }

// void update_connected_clients() {
//     char label_text[50];
//     sprintf(label_text, "Connected clients: %d", connected_clients);
//     update_label_text(label_text, connected_label);
// }

// void update_ip_label_text(const char *ip_text) {
//     update_label_text(ip_text, ip_label);
// }

// void append_message(const char *message) {
//     GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
//     GtkTextIter iter;
//     gtk_text_buffer_get_end_iter(buffer, &iter);
//     gtk_text_buffer_insert(buffer, &iter, message, -1);
// }

// void send_to_all_clients(const char *message, int sender_index) {
//     for (int i = 0; i < MAX_CLIENTS; i++) {
//         if (client_sockets[i].socket != 0 && i != sender_index) {
//             send(client_sockets[i].socket, message, strlen(message), 0);
//         }
//     }
// }

// gboolean append_message_idle(gpointer data) {
//     const char *message = (const char *)data;
//     append_message(message);
//     return FALSE;  
// }

// void *handle_client(void *arg) {
//     ClientInfo *client_info = (ClientInfo *)arg;

//     int client_index = client_info->index;
//     int client_socket = client_info->socket;

//     char buffer[BUFFER_SIZE];
//     ssize_t recv_size;

//     while ((recv_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
//         buffer[recv_size] = '\0';  // Assurez-vous que la chaîne est terminée par un null byte
//         printf("Received message from client %d: %s\n", client_index, buffer);

//         g_idle_add(append_message_idle, g_strdup(buffer));

//         send_to_all_clients(buffer, client_index);
//     }

//     // Client disconnected or error
//     close(client_socket);

//     connected_clients--;
//     update_connected_clients();

//     client_sockets[client_index].socket = 0;
//     client_sockets[client_index].index = 0;

//     free(client_info);

//     return NULL;
// }

// void update_ip_label(const char *ip_text) {
//     update_label_text(ip_text, ip_label);
// }

// void *start_server(void *arg) {
//     (void)arg;

//     struct sockaddr_in server_addr, client_addr;
//     socklen_t addr_len = sizeof(client_addr);

//     server_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_socket == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr("172.20.10.2");//INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error binding socket");
//         exit(EXIT_FAILURE);
//     }

//     if (listen(server_socket, MAX_CLIENTS) == -1) {
//         perror("Error listening for connections");
//         exit(EXIT_FAILURE);
//     }

//     update_ip_label_text("Server IP: Not available");  // Initialiser avec un texte par défaut

//     while (1) {
//         int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
//         if (client_socket == -1) {
//             perror("Error accepting connection");
//             continue;
//         }

//         int i;
//         for (i = 0; i < MAX_CLIENTS; i++) {
//             if (client_sockets[i].socket == 0) {
//                 client_sockets[i].socket = client_socket;
//                 client_sockets[i].index = i;

//                 connected_clients++;
//                 update_connected_clients();

//                 // Mettre à jour l'adresse IP chaque fois qu'un client se connecte
//                 update_ip_label_text("Server IP: Connected"); 

//                 ClientInfo *client_info = malloc(sizeof(ClientInfo));
//                 memcpy(client_info, &client_sockets[i], sizeof(ClientInfo));

//                 pthread_t thread;
//                 pthread_create(&thread, NULL, handle_client, client_info);
//                 pthread_detach(thread);

//                 break;
//             }
//         }
//     }

//     return NULL;
// }

// int main(int argc, char *argv[]) {
//     gtk_init(&argc, &argv);

//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Server Dashboard");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     gtk_widget_set_size_request(window, 400, 300);

//     // Créer un GtkNotebook pour gérer les onglets
//     notebook = gtk_notebook_new();

//     messages_view = gtk_text_view_new();
//     gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
//     gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

//     connected_label = gtk_label_new("Connected clients: 0");

//     // Créer le label pour l'IP
//     ip_label = gtk_label_new("Server IP: Not available");

//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);  // Ajouter le notebook
//     gtk_box_pack_start(GTK_BOX(vbox), memory_label, FALSE, FALSE, 0);

//     // Ajouter l'onglet Messages
//     GtkWidget *messages_tab_label = gtk_label_new("Messages");
//     gtk_notebook_append_page(GTK_NOTEBOOK(notebook), messages_view, messages_tab_label);

//     // Ajouter l'onglet Clients
//     GtkWidget *clients_tab_label = gtk_label_new("Connected Clients");
//     gtk_notebook_append_page(GTK_NOTEBOOK(notebook), connected_label, clients_tab_label);

//     // Ajouter l'onglet IP
//     GtkWidget *ip_tab_label = gtk_label_new("Server IP");
//     gtk_notebook_append_page(GTK_NOTEBOOK(notebook), ip_label, ip_tab_label);

//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     gtk_widget_show_all(window);

//     pthread_t server_thread;
//     pthread_create(&server_thread, NULL, start_server, NULL);

//     gtk_main();

//     return 0;
// }


//Gestion des amies 

#include <libwebsockets.h>

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            // Nouvelle connexion établie
            break;

        case LWS_CALLBACK_RECEIVE:
            // Message reçu du client
            break;

        default:
            break;
    }

    return 0;
}

int main(void) {
    struct lws_context_creation_info info;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = 12345;
    info.protocols = lws_protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Erreur lors de la création du contexte libwebsockets\n");
        return -1;
    }

    while (1) {
        lws_service(context, 50);
        // Votre logique ici
    }

    lws_context_destroy(context);

    return 0;
}
