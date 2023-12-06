#include "header/main.h"
//  -lws2_32
int main()
{
    CONNECTION con;
    // Jika return -1 maka error
    if (con.startClientSocket(12345, "127.0.0.1") == -1)
        return 0;
    if (con.startClient() == -1)
        return 0;
    SOCKET clientSocket = con.getClientSocket();

    DES_Encryption DES;
    int msgLenInt = 0;
    int bytesReceived = 0;
    char msgLenStr[5];
    char rec_msg[8193];
    char msg[1024];
    CHAT chat;

    while (true)
    {
        // for (int i = 0; i <)
        // Prompt pesan kepada user client
        printf("Masukkan Pesan: ");
        gets(msg);

        printf("[Before] msg: %s\n", msg);
        // DES Encryption
        chat.messageEncryption(msg);
        printf("[Encrypted] msg: %s\n", msg);

        // Kirim pesan ke server
        send(clientSocket, msg, strlen(msg), 0);

        bytesReceived = recv(clientSocket, msg, 1024, 0);
        if (bytesReceived <= 0)
        {
            printf("Error receiving message length.\n");
            // delete[] buffer;
            break;
        }

        chat.messageDecryption(msg);
        printf("[Decrypted] msg: %s\n", msg);
    }
    con.closeSocket(clientSocket);
    con.cleanUp();
    return 0;
}