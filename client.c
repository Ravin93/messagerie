//LIGNE DE COMMANDE 

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define SERVER_IP "172.20.10.2"  // Mettez l'adresse IP du serveur ici
// #define PORT 12345
// #define BUFFER_SIZE 1024

// void start_client() {
//     int client_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_socket == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);

//     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
//         perror("Invalid address/ Address not supported");
//         exit(EXIT_FAILURE);
//     }

//     if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error connecting to server");
//         exit(EXIT_FAILURE);
//     }

//     char message[BUFFER_SIZE];
//     while (1) {
//         printf("Enter message: ");
//         fgets(message, sizeof(message), stdin);

//         // Envoi du message au serveur
//         send(client_socket, message, strlen(message), 0);

//         if (strcmp(message, "exit\n") == 0) {
//             // Quitte la boucle si le message est "exit"
//             break;
//         }
//     }

//     close(client_socket);
// }

// int main() {
//     start_client();
//     return 0;
// }


//INTERFACE GRAPHIQUE 

// #include <gtk/gtk.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define SERVER_IP "172.20.10.2"  //Adreese IP du serveur 
// #define PORT 12345
// #define BUFFER_SIZE 1024

// GtkWidget *messages_view;
// GtkWidget *message_entry;

// int client_socket;

// void append_message(const char *message) {
//     GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
//     GtkTextIter iter;
//     gtk_text_buffer_get_end_iter(buffer, &iter);
//     gtk_text_buffer_insert(buffer, &iter, message, -1);
// }

// void send_message() {
//     const char *message = gtk_entry_get_text(GTK_ENTRY(message_entry));
//     if (send(client_socket, message, strlen(message), 0) == -1) {
//         perror("Error sending message");
//         // Gérer l'erreur ici, par exemple, fermer la connexion, afficher un message à l'utilisateur, etc.
//     } else {
//         append_message("Mac: ");
//         append_message(message);
//         append_message("\n");
//         gtk_entry_set_text(GTK_ENTRY(message_entry), "");  // Vider le champ de saisie
//     }
// }


// void *receive_messages(void *arg) {
//     while (1) {
//         char buffer[BUFFER_SIZE];
//         ssize_t recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
//         if (recv_size <= 0) {
//             // Gérer la déconnexion ou l'erreur de réception ici
//             break;
//         }
//         buffer[recv_size] = '\0';  // Assurez-vous que la chaîne est terminée par un null byte
//         append_message(buffer);
//     }
//     return NULL;
// }

// void start_client() {
//     client_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_socket == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);

//     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
//         perror("Invalid address/ Address not supported");
//         exit(EXIT_FAILURE);
//     }

//     if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error connecting to server");
//         exit(EXIT_FAILURE);
//     }

//     // Initialize GTK
//     gtk_init(NULL, NULL);

//     // Create the main window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Client");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     gtk_widget_set_size_request(window, 400, 300);

//     // Create messages view
//     messages_view = gtk_text_view_new();
//     gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
//     gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

//     pthread_t receive_thread;
//     pthread_create(&receive_thread, NULL, receive_messages, NULL);
//     pthread_detach(receive_thread);

//     // Create message entry
//     message_entry = gtk_entry_new();
//     g_signal_connect(message_entry, "activate", G_CALLBACK(send_message), NULL);

//     // Create vertical box
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_box_pack_start(GTK_BOX(vbox), messages_view, TRUE, TRUE, 0);
//     gtk_box_pack_start(GTK_BOX(vbox), message_entry, FALSE, FALSE, 0);

//     // Connect the main window's destroy signal to gtk_main_quit
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Add the vertical box to the main window
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Show all the widgets in the window
//     gtk_widget_show_all(window);

//     // Run the GTK main loop
//     gtk_main();
// }

// int main() {
//     start_client();
//     close(client_socket);
//     return 0;
// }



// Gestion d'ami 

#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_ADDRESS "ws://172.20.10.2:8080"

static int interrupted = 0;

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            printf("Connected to server\n");
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            printf("Received data: %.*s\n", (int)len, (char *)in);
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            printf("Connection closed\n");
            interrupted = 1;
            break;

        default:
            break;
    }

    return 0;
}

int main(void) {
    struct lws_context *context;
    struct lws *wsi;
    struct lws_protocols protocols[] = {
        {"", callback, 0, 0},
        {NULL, NULL, 0, 0}  // Dernier élément nul pour indiquer la fin du tableau
    };

    struct lws_client_connect_info connect_info = {
        .context = NULL,
        .address = SERVER_ADDRESS,
        .port = 80,
        .path = "/",
        .host = "localhost",
        .origin = "localhost",
        .protocol = protocols[0].name,
        .iface = NULL,
        .ssl_connection = 0,
        .pss = NULL,
        .headers = NULL,
        .hostname = NULL,
        .port_early_hangup = 1,
        .fd = -1
    };

    struct lws_context_creation_info context_info = {
        .options = 0,
        .port = CONTEXT_PORT_NO_LISTEN,
        .protocols = protocols,
        .extensions = NULL,
        .token_limits = NULL,
        .ssl_cert_filepath = NULL,
        .ssl_private_key_filepath = NULL,
        .ssl_ca_filepath = NULL,
        .ssl_cipher_list = NULL,
        .http_proxy_address = NULL,
        .http_proxy_port = 0,
        .http_proxy_credentials = NULL,
        .user = NULL,
        .ka_time = 0,
        .ka_probes = 0,
        .ka_interval = 0,
        .retry_and_idle_policy = {0, 0, 0, 0},
        .ws_ping_pong_interval = 0,
        .uid = 0,
        .gid = 0,
        .ifname = NULL,
        .max_http_header_pool = 0,
        .use_ssl = 0,
        .mux_policy = 0,
        .pcontext = &context,
        .extensions_disabled = 0,
        .forece_no_protocol_version = 0,
        .client_ssl_ctx = NULL,
        .client_ssl_cert = NULL,
        .client_ssl_private_key = NULL,
        .client_ssl_ca_filepath = NULL,
        .check_client_cert_peer = 0,
        .client_ssl_options = 0,
        .timeout_secs = 10
    };

    context = lws_create_context(&context_info);
    if (context == NULL) {
        fprintf(stderr, "Error creating libwebsockets context\n");
        return -1;
    }

    connect_info.context = context;

    wsi = lws_client_connect_via_info(&connect_info);
    if (wsi == NULL) {
        fprintf(stderr, "Error creating libwebsockets client connection\n");
        lws_context_destroy(context);
        return -1;
    }

    while (!interrupted) {
        lws_service(context, 0);
        // Ajoutez ici votre logique de traitement
    }

    lws_context_destroy(context);

    return 0;
}





