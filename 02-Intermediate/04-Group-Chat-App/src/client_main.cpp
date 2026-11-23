#include "ChatClient.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 8080;
    std::string username = "Guest";

    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = std::stoi(argv[2]);
    if (argc >= 4) username = argv[3];

    std::cout << "=================================================================\n";
    std::cout << "                   TCP GROUP CHAT CLIENT                         \n";
    std::cout << "=================================================================\n";
    std::cout << "Commands:\n";
    std::cout << "  /w <user> <message>  : Send private message\n";
    std::cout << "  /users               : View active members\n";
    std::cout << "  /quit                : Exit chat room\n";
    std::cout << "-----------------------------------------------------------------\n";

    chat::ChatClient client;
    client.setMessageCallback([](const chat_protocol::ChatMessage& msg) {
        if (msg.type == chat_protocol::MessageType::SYSTEM) {
            std::cout << "\n[!] " << msg.text << "\n> " << std::flush;
        } else if (msg.type == chat_protocol::MessageType::WHISPER) {
            std::cout << "\n[Whisper from @" << msg.sender << "]: " << msg.text << "\n> " << std::flush;
        } else if (msg.type == chat_protocol::MessageType::USERS) {
            std::cout << "\n[Online Members]: " << msg.text << "\n> " << std::flush;
        } else {
            std::cout << "\n[@" << msg.sender << "]: " << msg.text << "\n> " << std::flush;
        }
    });

    if (!client.connectToServer(host, port, username)) {
        std::cerr << "Failed to connect to " << host << ":" << port << "\n";
        return 1;
    }

    std::cout << "Connected as '" << username << "'. Start chatting!\n> ";

    std::string input;
    while (client.isConnected() && std::getline(std::cin, input)) {
        if (input == "/quit") {
            break;
        } else if (input == "/users") {
            client.requestUserList();
        } else if (input.rfind("/w ", 0) == 0) {
            size_t space2 = input.find(' ', 3);
            if (space2 != std::string::npos) {
                std::string target = input.substr(3, space2 - 3);
                std::string body = input.substr(space2 + 1);
                client.sendWhisper(target, body);
            } else {
                std::cout << "Usage: /w <username> <message>\n";
            }
        } else if (!input.empty()) {
            client.sendMessage(input);
        }
        std::cout << "> " << std::flush;
    }

    client.disconnect();
    std::cout << "Disconnected.\n";
    return 0;
}
