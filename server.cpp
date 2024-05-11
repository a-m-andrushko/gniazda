#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    // UTWORZENIE GNIAZDA
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);   // socket(x, SOCK_STREAM, x) -- SYGNALIZUJE TCP
    if (server_socket == -1)
    {
        std::cerr << "Error: Nie udało się utworzyć gniazda!\n";
        exit(0);
    }

    // BINDING GNIAZDA DO PORTU IP
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8880);   // DOWOLNY SWOBODNY PORT
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Error: Nie udało się powiązać gniazda do portu IP!\n";
        exit(0);
    }

    // NASŁUCHIWANIE
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        std::cerr << "Error: Nasłuchiwanie nie powiodło się!\n";
        exit(0);
    }

    // POŁĄCZENIE
    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);
    int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
    if (client_socket == -1)
    {
        std::cerr << "Error: Połączenie nie zostało nawiązane!\n";
        exit(0);
    }

    // KOMUNIKACJA
    char buffer[4096];
    while (true)
    {
        // ODBIERANIE OD KLIENTA
        memset(buffer, 0, 4096);
        int bytes_received = recv(client_socket, buffer, 4096, 0);
        if (bytes_received == -1)
        {
            std::cerr << "Error: Błąd podczas odbierania!\n";
            break;
        }

        if (bytes_received == 0)
        {
            std::cout << "Klient został odłączony!\n";
        }
        std::cout << "Client: " << std::string(buffer, bytes_received) << "\n";

        // SPRAWDZENIE, CZY KLIENT SIĘ ROZŁĄCZA
        if (std::string(buffer) == "exit_client")
        {
            std::cout << "Klient się rozłączył!\n";
            close(client_socket);
        }


        // NADAWANIE DO KLIENTA
        std::string user_input;
        std::cout << "> ";
        std::getline(std::cin, user_input);
        if (user_input == "exit_server")
        {
            std::cout << "Serwer kończy połączenie!" << "\n";
            send(server_socket, user_input.c_str(), user_input.size() + 1, 0);
            close(client_socket);
            close(server_socket);
            break;
        }
        
        int send_result = send(client_socket, user_input.c_str(), user_input.size() + 1, 0);
        if (send_result == -1)
        {
            std::cerr << "Error: Błąd podczas nadawania!\n";
            break;
        }
    }

    // ZAMKNIĘCIE GNIAZDA KLIENTA
    if (client_socket != -1)
    {
        close(client_socket);
    }

    return 0;
}