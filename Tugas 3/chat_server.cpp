#include "header/main.h"

CONNECTION con;
// map<int, SOCKET> clientSockets;
// map<int, string> client_des_keys;
int currentClient = 0;
char listClient[10][30];
SOCKET clientSockets[10];
CHAT chats;
char client_des_keys[10][17];
const long long int P = 13, Q = 17;

//  -lws2_32
DWORD WINAPI listenThread(LPVOID lpParam)
{
    SOCKET clientSocket = *static_cast<SOCKET *>(lpParam);
    DES_Encryption DES;
    // CHAT chats;
    char msg[256];
    int bytesReceived = 0;

    while (true)
    {
        // Menerima panjang pesan dari client
        bytesReceived = recv(clientSocket, msg, 256, 0);
        if (bytesReceived <= 0)
        {
            cerr << "Error receiving message length.\n";
            break;
        }

        // DES Decryption
        chats.messageDecryption(msg);

        printf("message decypted: %s\n", msg);
        strcpy(msg, "");
    }

    return 0;
}

DWORD WINAPI sendThread(LPVOID lpParam)
{
    SOCKET clientSocket = *static_cast<SOCKET *>(lpParam);
    DES_Encryption DES;
    // CHAT message;
    char msg[256];
    int messageLength = 0;
    int des_iteration = 0;
    int bytesReceived = 0;

    while (true)
    {
        strcpy(msg, "");
        // for (int i = 0; i <)
        // Prompt pesan kepada user client
        printf("Masukkan Pesan: ");
        gets(msg);

        printf("[Before] msg: %s\n", msg);
        // DES Encryption
        chats.messageEncryption(msg);
        printf("[Encrypted] msg: %s\n", msg);

        // Kirim pesan ke server
        send(clientSocket, msg, strlen(msg), 0);
    }
    return 0;
}

void itoc(char *c, int i)
{
    int index = -1;
    char temp[10];
    while (i != 0)
    {
        // printf("%d\n", i);
        // printf("%d\n", i % 10);
        temp[++index] = i % 10 + '0';
        i /= 10;
    }
    c[index + 1] = '\0';
    while (index >= 0)
        c[i++] = temp[index--];
}

int ctoi(char *c)
{
    int tempInt = 0;
    int lent = strlen(c);
    for (int index = 0; index < lent; index++)
        tempInt = (tempInt * 10) + (int)(c[index] - '0');

    return tempInt;
}

int recv_int_msg(SOCKET clientSocket, string msg)
{
    int bytesReceived;
    char tempChar[1024];
    bytesReceived = recv(clientSocket, tempChar, 1024, 0);
    if (bytesReceived <= 0)
    {
        cerr << msg << "\n";
        con.closeSocket(clientSocket);
        return -1;
    }

    return ctoi(tempChar);
}

int main()
{
    // Jika return -1 maka error
    if (con.startServerSocket(13444, INADDR_ANY) == -1)
        return 0;
    if (con.startServer() == -1)
        return 0;
    SOCKET serverSocket = con.getServerSocket();
    SOCKET clientSocket = con.getClientSocket();
    printf("[STATUS]Inititate Server...\n");

    int bytesReceived = 0;
    int client_N = 0,
        my_N = 0,
        my_N_encrypted = 0,
        client_N_encrypted = 0,
        my_N_from_client = 0,
        my_N_from_client_encrypted = 0,
        idLength = 0,
        client_pk[2];

    DES_Encryption DES;
    char rec_msg[8193];
    char msg[8193];

    printf("[STATUS]Generating public and secret key using RSA...\n");
    RSA rsa(P, Q);
    // int self_ektemp = rsa.getPublicKey_e();
    // int self_nkey = rsa.getPublicKey_n();

    // ------------------------------ Exchange Public Key ------------------------------
    // Receiving client's public key (e and n)
    printf("[CONNECTION]Receiving client's public key...\n");
    bytesReceived = recv(clientSocket, msg, 1024, 0);
    if (bytesReceived <= 0)
    {
        printf("Error receiving message length.\n");
        return 0;
    }
    client_pk[0] = ctoi(msg);
    printf("Hello\n\n");
    bytesReceived = recv(clientSocket, msg, 1024, 0);
    if (bytesReceived <= 0)
    {
        printf("Error receiving message length.\n");
        // delete[] buffer;
        return 0;
    }
    printf("Hello\n");
    client_pk[1] = ctoi(msg);
    printf("----- Received client's public Key -----\n");
    printf("e: %d\nn: %d\n", client_pk[0], client_pk[1]);
    printf("----------------------------------------\n\n");

    // sending server's public key (e and n)
    printf("[CONNECTION]Sending public key...\n");
    char tempChar[10];
    int myPublicKey[2];
    myPublicKey[0] = rsa.getPublicKey_e();
    myPublicKey[1] = rsa.getPublicKey_n();
    itoc(tempChar, myPublicKey[0]);
    send(clientSocket, tempChar, sizeof(tempChar), 0);
    itoc(tempChar, myPublicKey[1]);
    send(clientSocket, tempChar, sizeof(tempChar), 0);

    printf("----- Server's public Key -----\n");
    printf("e: %d\nn: %d\n", myPublicKey[0], myPublicKey[1]);
    printf("-------------------------------\n\n");
    // -------------------------- End of Exchange Public Key --------------------------

    // ----------------------------- Exchange Client's N ------------------------------
    // Receiving client's N
    printf("[CONNECTION]Receiving client's N...\n");
    if ((client_N_encrypted = recv_int_msg(clientSocket, "Error receiving client's N\n")) == -1)
        return 0;
    client_N = rsa.decrypt(client_N_encrypted);
    printf("---------- Client's N ---------\n");
    printf("received (encrypted): %d\nreceived (decrypted): %d\n", client_N_encrypted, client_N);
    printf("-------------------------------\n\n");

    // Sending client's N (encrypted using client's PK)
    printf("[CONNECTION]Sending N...\n");
    client_N_encrypted = rsa.encrypt(client_N, client_pk[0], client_pk[1]);
    itoc(tempChar, client_N_encrypted);
    send(clientSocket, tempChar, sizeof(tempChar), 0);
    printf("------------- Client's N ------------\n");
    printf("original                  : %d\nenrypted using client's pk: %d\n", client_N, client_N_encrypted);
    printf("-------------------------------\n\n");
    // ------------------------ End of Exchange Client's N ----------------------------

    // ----------------------------- Exchange Server's N ------------------------------
    // sending N (encrypted using client's PK)
    printf("[CONNECTION]Sending N...\n");
    my_N = rsa.generateKeyDistribution(client_pk[0], client_pk[1]);
    my_N_encrypted = rsa.encrypt(my_N, client_pk[0], client_pk[1]);
    itoc(tempChar, my_N_encrypted);
    send(clientSocket, tempChar, sizeof(tempChar), 0);

    printf("------------- My N ------------\n");
    printf("original                  : %d\nenrypted using client's pk: %d\n", my_N, my_N_encrypted);
    printf("-------------------------------\n\n");

    // Receiving Client's Respon
    printf("[CONNECTION]Receiving client's message...\n");
    if ((my_N_from_client_encrypted = recv_int_msg(clientSocket, "Error receiving message\n")) == -1)
        return 0;
    my_N_from_client = rsa.decrypt(my_N_from_client_encrypted);
    printf("------- My N from client ------\n");
    printf("original   : %d\nfrom client: %d\n", my_N, my_N_from_client);
    printf("-------------------------------\n\n");
    if (my_N != my_N_from_client)
    {
        cerr << "Sender is not trusted!\nERROR: Wrong N\n";
        con.closeSocket(clientSocket);
        return 0;
    }
    // ------------------------ End of Exchange Server's N ----------------------------

    // ----------- Mengirim DES KEY ----------
    printf("[CONNECTION]Sending DES key...\n");
    char curr_des_key[20];
    chats.randomizeKey();
    chats.getKey(curr_des_key);
    send(clientSocket, curr_des_key, strlen(curr_des_key), 0);
    printf("--------- Message Sent --------\n");
    printf("DES_KEY = %s----\n", curr_des_key);
    printf("-------------------------------\n\n");
    // ----------- End of Mengirim DES KEY ----------

    HANDLE thread1, thread2;
    thread1 = CreateThread(NULL, 0, listenThread, &clientSocket, 0, NULL);
    thread2 = CreateThread(NULL, 0, sendThread, &clientSocket, 0, NULL);

    // Check if threads are created successfully
    if (thread1 == NULL
        //  || thread2 == NULL
    )
    {
        cerr << "Error creating threads" << endl;
        return 1;
    }

    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    CloseHandle(thread1);
    CloseHandle(thread2);
    con.closeSocket(clientSocket);
    con.closeSocket(serverSocket);
    con.cleanUp();

    return 0;
}
