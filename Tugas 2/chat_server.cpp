#include "header/main.h"
//  -lws2_32
int main()
{
    CONNECTION con;
    // Jika return -1 maka error
    if (con.startServerSocket(12345, INADDR_ANY) == -1)
        return 0;
    if (con.startServer() == -1)
        return 0;
    SOCKET serverSocket = con.getServerSocket();
    SOCKET clientSocket = con.getClientSocket();

    DES_Encryption DES;
    int msgLenInt = 0;
    int bytesReceived = 0;
    char msgLenStr[5];
    char rec_msg[8193];
    char msg[8193];
    CHAT chat;

    while (true)
    {
        bytesReceived = recv(clientSocket, msg, 1024, 0);
        if (bytesReceived <= 0)
        {
            printf("Error receiving message length.\n");
            // delete[] buffer;
            break;
        }

        chat.messageDecryption(msg);
        printf("[Decrypted] msg: %s\n", msg);

        // Prompt pesan kepada user server
        printf("Masukkan Pesan: ");
        gets(msg);

        // DES Encryption
        chat.messageEncryption(msg);
        printf("[Encrypted] msg: %s\n", msg);

        // kirim pesan ke client
        send(clientSocket, msg, strlen(msg), 0);
    }
    con.closeSocket(clientSocket);
    con.closeSocket(serverSocket);
    con.cleanUp();
    return 0;
}
