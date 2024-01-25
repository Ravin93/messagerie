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


// client fusioner :

// #include <gtk/gtk.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <pthread.h>

// #define LENGTH 2048

// // Global variables
// volatile sig_atomic_t flag = 0;
// int sockfd = 0;
// char name[32];

// GtkWidget *messages_view;
// GtkWidget *message_entry;

// void append_message(const char *message) {
//     GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
//     GtkTextIter iter;
//     gtk_text_buffer_get_end_iter(buffer, &iter);
//     gtk_text_buffer_insert(buffer, &iter, message, -1);
// }

// void str_trim_lf(char *arr, int length) {
//     int i;
//     for (i = 0; i < length; i++) { // trim \n
//         if (arr[i] == '\n') {
//             arr[i] = '\0';
//             break;
//         }
//     }
// }

// void catch_ctrl_c_and_exit(int sig) {
//     flag = 1;
// }

// void send_msg_handler() {
//     char message[LENGTH] = {};
//     char buffer[LENGTH + 32] = {};

//     while (1) {
//         fgets(message, LENGTH, stdin);
//         str_trim_lf(message, LENGTH);

//         if (strcmp(message, "exit") == 0) {
//             break;
//         } else {
//             sprintf(buffer, "%s: %s\n", name, message);
//             send(sockfd, buffer, strlen(buffer), 0);
//         }

//         bzero(message, LENGTH);
//         bzero(buffer, LENGTH + 32);
//     }
//     catch_ctrl_c_and_exit(2);
// }

// void recv_msg_handler() {
//     char message[LENGTH] = {};
//     while (1) {
//         int receive = recv(sockfd, message, LENGTH, 0);
//         if (receive > 0) {
//             append_message(message);
//         } else if (receive == 0) {
//             break;
//         } else {
//             // -1
//         }
//         memset(message, 0, sizeof(message));
//     }
// }

// void *network_thread(void *arg) {
//     // Socket settings
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(8001);

//     if (inet_pton(AF_INET, "172.20.10.2", &server_addr.sin_addr) <= 0) {
//         perror("Invalid address/ Address not supported");
//         exit(EXIT_FAILURE);
//     }

//     // Connect to Server
//     int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
//     if (err == -1) {
//         perror("ERROR: connect");
//         exit(EXIT_FAILURE);
//     }

//     // Send name
//     send(sockfd, name, 32, 0);

//     // Receive welcome message
//     char welcome_message[LENGTH];
//     recv(sockfd, welcome_message, LENGTH, 0);
//     append_message(welcome_message);

//     pthread_t send_msg_thread;
//     if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0) {
//         perror("ERROR: pthread");
//         exit(EXIT_FAILURE);
//     }

//     pthread_t recv_msg_thread;
//     if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0) {
//         perror("ERROR: pthread");
//         exit(EXIT_FAILURE);
//     }

//     // Wait for the send and receive threads to finish
//     pthread_join(send_msg_thread, NULL);
//     pthread_join(recv_msg_thread, NULL);

//     close(sockfd);

//     // Terminate the GTK main loop
//     gtk_main_quit();

//     return NULL;
// }

// void send_message(GtkWidget *widget, gpointer data) {
//     const char *message = gtk_entry_get_text(GTK_ENTRY(message_entry));
//     if (send(sockfd, message, strlen(message), 0) == -1) {
//         perror("Error sending message");
//         // Gérer l'erreur ici, par exemple, fermer la connexion, afficher un message à l'utilisateur, etc.
//     } else {
//         append_message("Mac: ");
//         append_message(message);
//         append_message("\n");
//         gtk_entry_set_text(GTK_ENTRY(message_entry), ""); // Vider le champ de saisie
//     }
// }

// void start_client() {
//     // Initialize GTK
//     gtk_init(NULL, NULL);

//     // Create the main window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Chat Client");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     gtk_widget_set_size_request(window, 400, 300);

//     // Create messages view
//     messages_view = gtk_text_view_new();
//     gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
//     gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

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

//     // Create a separate thread for network operations
//     pthread_t network_thread_id;
//     pthread_create(&network_thread_id, NULL, network_thread, NULL);

//     // Run the GTK main loop
//     gtk_main();

//     // Wait for the network thread to finish
//     pthread_join(network_thread_id, NULL);
// }

// int main() {
//     signal(SIGINT, catch_ctrl_c_and_exit);

//     printf("Please enter your name: ");
//     fgets(name, 32, stdin);
//     str_trim_lf(name, strlen(name));

//     if (strlen(name) > 32 || strlen(name) < 2) {
//         printf("Name must be less than 30 and more than 2 characters.\n");
//         return EXIT_FAILURE;
//     }

//     start_client();

//     return EXIT_SUCCESS;
// }


//compte //

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>  //  utiliser le type de données bool

#define LENGTH 2048
#define USERNAME_LENGTH 32
#define BUFFER_SIZE 1024 // ou la taille appropriée selon vos besoins
#define PASSWORD_LENGTH 64  // Vous pouvez ajuster la longueur du mot de passe selon vos besoins

typedef struct {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];  
} UserInfo;

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
GtkWidget *messages_view;
GtkWidget *message_entry;

bool exit_requested = false;

void str_trim_lf(char *arr, int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}


void create_account(int sockfd) {
    UserInfo new_user;

    printf("Enter your credentials (username password): ");
    char input[64];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading credentials\n");
        return;
    }

    sscanf(input, "%s %s", new_user.username, new_user.password);
    str_trim_lf(new_user.username, USERNAME_LENGTH);
    str_trim_lf(new_user.password, PASSWORD_LENGTH);  // Assurez-vous d'avoir PASSWORD_LENGTH défini

    // Demander d'autres informations comme le prénom, etc., et les stocker dans new_user.

    send(sockfd, &new_user, sizeof(UserInfo), 0);

    char server_response[BUFFER_SIZE];
    recv(sockfd, server_response, BUFFER_SIZE, 0);
    printf("%s\n", server_response);
}

int check_account_existence(int sockfd, const char *username) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "CHECK_EXISTENCE %s", username);
    send(sockfd, buffer, strlen(buffer), 0);

    // Attendre la réponse du serveur (compte existe ou non)
    recv(sockfd, buffer, sizeof(buffer), 0);

    // Analyser la réponse
    if (strcmp(buffer, "ACCOUNT_EXISTS") == 0) {
        printf("Account already exists. Please choose a different username.\n");
        return 1;  // Le compte existe déjà
    } else {
        return 0;  // Le compte n'existe pas
    }
}

int login(int sockfd) {
    // Client authentication process
    const char *auth_request_message = "Enter your credentials (name password): ";
    send(sockfd, auth_request_message, strlen(auth_request_message), 0);

    UserInfo user_info;

    // Receive user input for credentials
    printf("Enter your credentials (username password): ");
    scanf("%s %s", user_info.username, user_info.password);

    // Debug information
    printf("Sending credentials to server: Username: %s, Password: %s\n", user_info.username, user_info.password);

    // Send the entered credentials to the server as a UserInfo structure
    send(sockfd, &user_info, sizeof(UserInfo), 0);

    // Debug information
    printf("Credentials sent. Waiting for authentication result...\n");

    // Add logic to receive authentication result from the server
    char auth_result[BUFFER_SIZE];
    ssize_t received_bytes = recv(sockfd, auth_result, sizeof(auth_result) - 1, 0);
    printf("After receiving authentication result\n");

    if (received_bytes > 0) {
        // Null-terminate the received data
        auth_result[received_bytes] = '\0';
        printf("Received authentication result from server: \"%s\"\n", auth_result);

        if (strcmp(auth_result, "Authentication successful. Welcome to the chat server!") == 0) {
            printf("Authentication successful!\n");
            // Traiter l'authentification réussie ici
            return 1;
        } else {
            printf("Authentication failed. Please try again.\n");
            // Traiter l'authentification échouée ici
            return 0;
        }
    } else if (received_bytes == 0) {
        printf("Connection closed by the server.\n");
        // Ajoutez une logique pour gérer la fermeture de connexion si nécessaire
    } else {
        perror("Error receiving authentication result");
        // Ajoutez une logique pour gérer l'erreur de réception si nécessaire
    }

    // En cas d'erreur, retournez une valeur appropriée (par exemple, -1)
    return -1;
}





void append_message(const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messages_view));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, message, -1);
}

void send_message(GtkWidget *widget, gpointer data);


void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
    exit_requested = true;  
}

void send_msg_handler() {
    char message[LENGTH] = {};
    char buffer[LENGTH + 32] = {};

    while (1) {
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        if (strcmp(message, "exit") == 0) {
            break;
        } else {
            sprintf(buffer, "%s: %s\n", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
            printf("Message sent to server: %s\n", buffer);
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }
    catch_ctrl_c_and_exit(2);
}

void recv_msg_handler() {
    char message[LENGTH] = {};
    while (1) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            printf("Received message from server: %s\n", message);
            append_message(message);
        } else if (receive == 0) {
            break;
        } else {
            // -1
        }
        memset(message, 0, sizeof(message));
    }
}
void *network_thread(void *arg) {
    int local_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (local_sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("Client socket created successfully.\n");
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8001);

    if (inet_pton(AF_INET, "172.20.10.2", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        perror("ERREUR : connect");
        exit(EXIT_FAILURE);
    }
    printf("Connecté au serveur.\n");

    int authentication_successful = login(sockfd);

    if (authentication_successful) {
        // Create GUI elements
        gtk_init(NULL, NULL);

        GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Chat Client");
        gtk_container_set_border_width(GTK_CONTAINER(window), 10);
        gtk_widget_set_size_request(window, 400, 300);

        messages_view = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

        message_entry = gtk_entry_new();
        g_signal_connect(message_entry, "activate", G_CALLBACK(send_message), NULL);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_box_pack_start(GTK_BOX(vbox), messages_view, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), message_entry, FALSE, FALSE, 0);

        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        gtk_container_add(GTK_CONTAINER(window), vbox);

        gtk_widget_show_all(window);

        pthread_t send_msg_thread;
        if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0) {
            perror("ERROR: pthread");
            exit(EXIT_FAILURE);
        }

        pthread_t recv_msg_thread;
        if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0) {
            perror("ERROR: pthread");
            exit(EXIT_FAILURE);
        }

        gtk_main();

        pthread_join(send_msg_thread, NULL);
        pthread_join(recv_msg_thread, NULL);
    } else {
        printf("L'authentification a échoué. Impossible de continuer.\n");
    }

    close(local_sockfd);

    exit_requested = true;
    gtk_main_quit();

    return NULL;
}


void send_message(GtkWidget *widget, gpointer data);


void start_client() {
    gtk_init(NULL, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Client");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);

    messages_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messages_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(messages_view), GTK_WRAP_WORD_CHAR);

    message_entry = gtk_entry_new();
    g_signal_connect(message_entry, "activate", G_CALLBACK(append_message), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), messages_view, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), message_entry, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    pthread_t network_thread_id;
    pthread_create(&network_thread_id, NULL, network_thread, NULL);

    gtk_main();

    pthread_join(network_thread_id, NULL);
}

void send_message(GtkWidget *widget, gpointer data) {
    const char *message = gtk_entry_get_text(GTK_ENTRY(message_entry));
    if (send(sockfd, message, strlen(message), 0) == -1) {
        perror("Error sending message");
    } else {
        append_message(name);
        append_message(": ");
        append_message(message);
        append_message("\n");
        gtk_entry_set_text(GTK_ENTRY(message_entry), "");
    }
}

int connect_to_server() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        return -1;
    }
    printf("Client socket created successfully.\n");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8001);

    if (inet_pton(AF_INET, "172.20.10.2", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sockfd);
        return -1;
    }

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        perror("ERROR: connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int main() {
    signal(SIGINT, catch_ctrl_c_and_exit);

    // Créer le socket une seule fois au début du programme
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return EXIT_FAILURE;
    }

    int authentication_successful = 0;
    while (1) {

        printf("1. Connexion\n2. Créer un compte\n3. Quitter\nChoisissez une option : ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Option invalide\n");
            break;
        }

        // Vider le caractère de nouvelle ligne du tampon
        while (getchar() != '\n');

        if (choice == 3) {
            // Quitter le programme
            printf("Au revoir !\n");
            break;
        }

        if (choice != 1 && choice != 2) {
            printf("Option invalide\n");
            continue;  // Revenir au début de la boucle
        }

        if (choice == 1) {
            // Login
            authentication_successful = login(sockfd);
        } else if (choice == 2) {
            // Create an account
            create_account(sockfd);
        }

        if (!authentication_successful) {
            printf("Authentication failed. Do you want to retry? (y/n): ");
            char retry_choice;
            if (scanf(" %c", &retry_choice) != 1) {
                printf("Invalid choice\n");
                break;
            }

            // Flush the newline character from the buffer
            while (getchar() != '\n');

            if (retry_choice != 'y' && retry_choice != 'Y') {
                break;
            }

            // Reconnecter le socket avant la nouvelle tentative
            close(sockfd);
            sockfd = connect_to_server();
            if (sockfd == -1) {
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}


// Gestion d'ami 

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

// void send_message(const char *message) {
//     // Retirez la déclaration suivante, car 'message' est déjà déclaré comme paramètre
//     // const char *message = gtk_entry_get_text(GTK_ENTRY(message_entry));

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



// void send_friend_request(int target_index) {
//     char request_message[BUFFER_SIZE];
//     snprintf(request_message, sizeof(request_message), "/addfriend %d", target_index);
//     send_message(request_message);
// }

// void accept_friend_request(int sender_index) {
//     char accept_message[BUFFER_SIZE];
//     snprintf(accept_message, sizeof(accept_message), "/acceptfriend %d", sender_index);
//     send_message(accept_message);
// }

// void reject_friend_request(int sender_index) {
//     char reject_message[BUFFER_SIZE];
//     snprintf(reject_message, sizeof(reject_message), "/rejectfriend %d", sender_index);
//     send_message(reject_message);
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





