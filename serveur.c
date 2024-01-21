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



//server fusioner 

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <pthread.h>
// #include <gtk/gtk.h>

// #define PORT 8001
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


//compte :

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>

#define PORT 8001
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Ajout des tailles des champs pour le nom d'utilisateur, le mot de passe et le prénom
#define USERNAME_LENGTH 32
#define PASSWORD_LENGTH 32
#define FIRSTNAME_LENGTH 32

typedef struct {
    int index;
    int socket;
} ClientInfo;

typedef struct {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char firstname[FIRSTNAME_LENGTH];
} UserInfo;

int server_socket, connected_clients = 0;
ClientInfo client_sockets[MAX_CLIENTS];
UserInfo registered_users[MAX_CLIENTS];

GtkWidget *messages_view;
GtkWidget *connected_label;
GtkWidget *ip_label;
GtkWidget *notebook;

void save_user_info(UserInfo *user) {
    FILE *file = fopen("user_database.txt", "a");
    if (file == NULL) {
        perror("Error opening user database file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s %s %s\n", user->username, user->password, user->firstname);
    fclose(file);
}


int create_account(int client_socket) {
    UserInfo new_user;
    recv(client_socket, &new_user, sizeof(UserInfo), 0);

    // Vérifier si l'utilisateur existe déjà
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(registered_users[i].username, new_user.username) == 0) {
            // Informer le client que la création de compte a échoué
            const char *account_failed_message = "Account creation failed. Username already exists.\n";
            send(client_socket, account_failed_message, strlen(account_failed_message), 0);

            // Retourner -1 pour indiquer l'échec de la création de compte
            return -1;
        }
    }

    // Si l'utilisateur n'existe pas, enregistrer les informations de l'utilisateur
    save_user_info(&new_user);

    // Informer le client que la création de compte a réussi
    const char *account_success_message = "Account created successfully!\n";
    send(client_socket, account_success_message, strlen(account_success_message), 0);

    return 0;
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
    gtk_text_buffer_insert(buffer, &iter, message, -1);
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


void *handle_client(void *arg) {
    ClientInfo *client_info = (ClientInfo *)arg;

    int client_index = client_info->index;
    int client_socket = client_info->socket;

    char buffer[BUFFER_SIZE];
    ssize_t recv_size;

    // Send user authentication request
    const char *auth_request_message = "Enter your credentials (username password firstname): ";
    send(client_socket, auth_request_message, strlen(auth_request_message), 0);

    // Receive user credentials
    recv(client_socket, registered_users[client_index].username, sizeof(registered_users[client_index].username), 0);
    recv(client_socket, registered_users[client_index].password, sizeof(registered_users[client_index].password), 0);
    recv(client_socket, registered_users[client_index].firstname, sizeof(registered_users[client_index].firstname), 0);

    // ... (restez attentif aux autres modifications nécessaires)

    return NULL;
}

void update_ip_label(const char *ip_text) {
    update_label_text(ip_text, ip_label);
}

void *start_server(void *arg) {
    (void)arg;

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("172.20.10.2");
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    update_ip_label_text("Server IP: Not available");

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

                // Send user authentication request
                const char *auth_request_message = "Enter your credentials (name password): ";
                send(client_socket, auth_request_message, strlen(auth_request_message), 0);

                // Receive user credentials
                recv(client_socket, registered_users[i].username, sizeof(registered_users[i].username), 0);
                recv(client_socket, registered_users[i].password, sizeof(registered_users[i].password), 0);
    
                pthread_t thread;
                pthread_create(&thread, NULL, handle_client, client_info);
                pthread_detach(thread);

                break;
            }
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Server Dashboard");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);

    notebook = gtk_notebook_new();

    messages_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

    connected_label = gtk_label_new("Connected clients: 0");

    ip_label = gtk_label_new("Server IP: Not available");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), connected_label, FALSE, FALSE, 0);

    GtkWidget *messages_tab_label = gtk_label_new("Messages");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), messages_view, messages_tab_label);

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



//Gestion des amies 

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
//     char username[50];
//     int is_friend[MAX_CLIENTS];
//     int pending_friend_request; // Ajout de la variable
// } ClientInfo;


// int server_socket, connected_clients = 0;
// ClientInfo client_sockets[MAX_CLIENTS];

// GtkWidget *messages_view;
// GtkWidget *connected_label;
// GtkWidget *ip_label;  // Nouveau label pour afficher l'IP
// GtkWidget *notebook;  // Nouveau notebook pour gérer les onglets




// void initialize_client(ClientInfo *client) {
//     client->socket = 0;
//     client->index = -1;
//     memset(client->username, 0, sizeof(client->username));
//     memset(client->is_friend, 0, sizeof(client->is_friend));
//     client->pending_friend_request = -1;
// }

// void send_friend_request(int sender_index, int target_index) {
//     // Vérifier si les indices sont valides
//     if (sender_index < 0 || sender_index >= MAX_CLIENTS || target_index < 0 || target_index >= MAX_CLIENTS) {
//         // Indices invalides, ignorer la demande d'amitié
//         return;
//     }

//     // Récupérer les informations sur l'émetteur (sender) et la cible (target)
//     ClientInfo *sender = &client_sockets[sender_index];
//     ClientInfo *target = &client_sockets[target_index];

//     // Vérifier si la cible est connectée
//     if (target->socket == 0) {
//         // La cible n'est pas connectée, ignorer la demande d'amitié
//         return;
//     }

//     // Vérifier si les deux utilisateurs ne sont pas déjà amis
//     if (sender->is_friend[target_index] || target->is_friend[sender_index]) {
//         // Les utilisateurs sont déjà amis, ignorer la demande d'amitié
//         return;
//     }

//     // Construire le message de demande d'amitié
//     char request_message[BUFFER_SIZE];
//     snprintf(request_message, sizeof(request_message), "FriendRequest:%d", sender_index);

//     // Envoyer la demande d'amitié à la cible
//     send(target->socket, request_message, strlen(request_message), 0);
// }


// void accept_friend_request(int sender_index, int target_index) {
//     // Vérifier si les indices sont valides
//     if (sender_index < 0 || sender_index >= MAX_CLIENTS || target_index < 0 || target_index >= MAX_CLIENTS) {
//         // Indices invalides, ignorer la demande
//         return;
//     }

//     // Récupérer les informations sur l'émetteur (sender) et la cible (target)
//     ClientInfo *sender = &client_sockets[sender_index];
//     ClientInfo *target = &client_sockets[target_index];

//     // Vérifier si la cible a encore la demande en attente
//     if (target->pending_friend_request != sender_index) {
//         // La demande n'est plus en attente, ignorer la demande d'acceptation
//         return;
//     }

//     // Mettre à jour les listes d'amis des deux utilisateurs
//     sender->is_friend[target_index] = 1;
//     target->is_friend[sender_index] = 1;

//     // Réinitialiser la demande d'amitié en attente pour la cible
//     target->pending_friend_request = -1;

//     // Informer l'émetteur que sa demande d'amitié a été acceptée
//     char accept_message[BUFFER_SIZE];
//     snprintf(accept_message, sizeof(accept_message), "FriendRequestAccepted:%d", target_index);
//     send(sender->socket, accept_message, strlen(accept_message), 0);

//     // Informer la cible que sa demande d'amitié a été acceptée
//     char target_accept_message[BUFFER_SIZE];
//     snprintf(target_accept_message, sizeof(target_accept_message), "FriendRequestAccepted:%d", sender_index);
//     send(target->socket, target_accept_message, strlen(target_accept_message), 0);
// }

// void reject_friend_request(int sender_index, int target_index) {
//     // Vérifier si les indices sont valides
//     if (sender_index < 0 || sender_index >= MAX_CLIENTS || target_index < 0 || target_index >= MAX_CLIENTS) {
//         // Indices invalides, ignorer la demande
//         return;
//     }

//     // Récupérer les informations sur l'émetteur (sender) et la cible (target)
//     ClientInfo *sender = &client_sockets[sender_index];
//     ClientInfo *target = &client_sockets[target_index];

//     // Vérifier si la cible a encore la demande en attente
//     if (target->pending_friend_request != sender_index) {
//         // La demande n'est plus en attente, ignorer la demande de rejet
//         return;
//     }

//     // Réinitialiser la demande d'amitié en attente pour la cible
//     target->pending_friend_request = -1;

//     // Informer l'émetteur que sa demande d'amitié a été rejetée
//     char reject_message[BUFFER_SIZE];
//     snprintf(reject_message, sizeof(reject_message), "FriendRequestRejected:%d", target_index);
//     send(sender->socket, reject_message, strlen(reject_message), 0);
// }


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

//     while ((recv_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
//         buffer[recv_size] = '\0';

//         // Vérifie si le message est une demande d'amitié
//         if (strncmp(buffer, "/addfriend ", 12) == 0) {
//             int target_index = atoi(buffer + 12);
//             if (target_index >= 0 && target_index < MAX_CLIENTS && client_sockets[target_index].socket != 0) {
//                 send_friend_request(client_index, target_index);
//             }
//         }
//         // Vérifie si le message est une réponse à une demande d'amitié
//         else if (strncmp(buffer, "/acceptfriend ", 15) == 0) {
//             int sender_index = atoi(buffer + 15);
//             if (sender_index >= 0 && sender_index < MAX_CLIENTS && client_sockets[sender_index].socket != 0) {
//                 accept_friend_request(sender_index, client_info->index);
//             }
//         }
//         else if (strncmp(buffer, "/rejectfriend ", 15) == 0) {
//             int sender_index = atoi(buffer + 15);
//             if (sender_index >= 0 && sender_index < MAX_CLIENTS && client_sockets[sender_index].socket != 0) {
//                 reject_friend_request(sender_index, client_info->index);

//             }
//         }
//         // Autres actions ou traitements de message
//         else {
//             // Traitement des autres types de messages
//             // ...
//         }

//         // Envoi du message à tous les clients
//         send_to_all_clients(buffer, client_index);
//     }
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

//     GtkWidget *memory_label = gtk_label_new("");

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