#include "header/main.h"
//  -lws2_32

CONNECTION con;
HANDLE listen_thread, send_thread;
CHAT chats;
const long long int P = 11, Q = 19;
char client_id[20];

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
    if (con.startClientSocket(13444, "127.0.0.1") == -1)
        return 0;
    if (con.startClient() == -1)
        return 0;
    SOCKET clientSocket = con.getClientSocket();

    RSA rsa(P, Q);
    int bytesReceived = 0,
        server_N = 0,
        server_N_encrypted = 0,
        my_N = 0,
        my_N_encrypted = 0,
        my_N_from_server = 0,
        my_N_from_server_encrypted = 0,
        idLength = 0,
        des_key_len = 16,
        server_pk[2];
    char des_key[16];
    // DES_Encryption DES;
    // int msgLenInt = 0;
    // int bytesReceived = 0;
    // char msgLenStr[5];
    // char rec_msg[8193];
    char msg[1024];
    // CHAT chats;

    // ------------------------------ Exchange Public Key ------------------------------
    // Sending client's public key (e and n)
    printf("[CONNECTION]Sending public key...\n");
    char tempChar[10];
    int myPublicKey[2];
    myPublicKey[0] = rsa.getPublicKey_e();
    myPublicKey[1] = rsa.getPublicKey_n();
    itoc(tempChar, myPublicKey[0]);
    printf("%s\n", tempChar);
    send(clientSocket, tempChar, sizeof(tempChar), 0);
    itoc(tempChar, myPublicKey[1]);
    printf("%s\n", tempChar);
    send(clientSocket, tempChar, sizeof(tempChar), 0);

    printf("----- My public Key -----\n");
    printf("e: %d\nn: %d\n", myPublicKey[0], myPublicKey[1]);
    printf("-------------------------------\n");

    // Receiving server's public key (e and n)
    printf("[CONNECTION]Receiving server's public key...\n");
    if ((server_pk[0] = recv_int_msg(clientSocket, "Error receiving server's public key\n")) == -1)
        return 0;
    if ((server_pk[1] = recv_int_msg(clientSocket, "Error receiving server's public key\n")) == -1)
        return 0;

    printf("----- Received server's public Key -----\n");
    printf("e: %d\nn: %d\n", server_pk[0], server_pk[1]);
    printf("----------------------------------------\n\n");
    // -------------------------- End of Exchange Public Key --------------------------

    // ----------------------------- Exchange Client's N ------------------------------
    // sending N (encrypted using server's PK)
    printf("[CONNECTION]Sending N...\n");
    my_N = rsa.generateKeyDistribution(server_pk[0], server_pk[1]);
    my_N_encrypted = rsa.encrypt(my_N, server_pk[0], server_pk[1]);
    itoc(tempChar, my_N_encrypted);
    send(clientSocket, tempChar, sizeof(tempChar), 0);

    printf("------------- My N ------------\n");
    printf("original                  : %d\nenrypted using server's pk: %d\n", my_N, my_N_encrypted);
    printf("-------------------------------\n\n");

    // Receiving Server's Respon
    printf("[CONNECTION]Receiving server's message...\n");
    if ((my_N_from_server_encrypted = recv_int_msg(clientSocket, "Error receiving message\n")) == -1)
        return 0;
    my_N_from_server = rsa.decrypt(my_N_from_server_encrypted);
    printf("------- My N from server ------\n");
    printf("original   : %d\nfrom server: %d\n", my_N, my_N_from_server);
    printf("-------------------------------\n\n");
    if (my_N != my_N_from_server)
    {
        cerr << "Sender is not trusted!\nERROR: Wrong N\n";
        con.closeSocket(clientSocket);
        return 0;
    }
    // ------------------------ End of Exchange Client's N ----------------------------

    // ----------------------------- Exchange Server's N ------------------------------
    // Receiving server's N
    printf("[CONNECTION]Receiving server's N...\n");
    if ((server_N_encrypted = recv_int_msg(clientSocket, "Error receiving server's N\n")) == -1)
        return 0;
    server_N = rsa.decrypt(server_N_encrypted);
    printf("---------- Server's N ---------\n");
    printf("received (encrypted): %d\nreceived (decrypted): %d\n", server_N_encrypted, server_N);
    printf("-------------------------------\n\n");

    // Sending server's N (encrypted using client's PK)
    printf("[CONNECTION]Sending Server's N...\n");
    server_N_encrypted = rsa.encrypt(server_N, server_pk[0], server_pk[1]);
    itoc(tempChar, server_N_encrypted);
    send(clientSocket, tempChar, sizeof(tempChar), 0);
    printf("------------- Server's N ------------\n");
    printf("original                  : %d\nenrypted using server's pk: %d\n", server_N, server_N_encrypted);
    printf("-------------------------------\n\n");
    // ------------------------ End of Exchange Server's N ----------------------------

    // ------------------------------ Receiving DES KEY -------------------------------
    printf("[CONNECTION]Receiving DES KEY...\n");
    bytesReceived = recv(clientSocket, des_key, sizeof(des_key), 0);
    if (bytesReceived <= 0)
    {
        cerr << "Error receiving des key from server.\n";
        return 0;
    }
    printf("--------- Message Recv  --------\n");
    des_key[16] = '\0';
    printf("DES_KEY = %s\n", des_key);
    printf("-------------------------------\n\n");
    chats.setKey(des_key);
    // ------------------------------ End of Receiving DES KEY -------------------------------

    // Memulai percakapan
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
    con.cleanUp();
    return 0;
}