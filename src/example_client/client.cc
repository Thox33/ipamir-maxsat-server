#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

int setupConnection(const char* serverIP, int port) {
    // Erstellen Sie einen Socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Fehler beim Erstellen des Client-Sockets" << std::endl;
        exit(1);
    }

    // Adresse und Port des Servers
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Verbinden Sie sich mit dem Server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Fehler beim Herstellen der Verbindung zum Server" << std::endl;
        close(clientSocket);
        exit(1);
    }

    std::cout << "Verbunden mit Server" << std::endl;

    return clientSocket;
}

void sendMessageCMD(int clientSocket) {
    std::string message;
    std::cout << "Nachricht an Server eingeben: ";
    std::getline(std::cin, message);

    if (message == "exit") {
        close(clientSocket);
        exit(0);
    }
    message += "\r\n";
    if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Fehler beim Senden von Daten an den Server" << std::endl;
        close(clientSocket);
        exit(1);
    }
}

void sendMessage(int clientSocket, const std::string& message) {
     std::string messageWithDelimiter = message + "\r\n";
    
    if (message == "exit") {
        close(clientSocket);
        exit(0);
    }
    if (send(clientSocket, messageWithDelimiter.c_str(), messageWithDelimiter.size(), 0) == -1) {
        std::cerr << "Fehler beim Senden von Daten an den Server" << std::endl;
        exit(1);
    }
}

void receiveMessage(int clientSocket) {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        std::cerr << "Fehler beim Empfangen von Daten vom Server" << std::endl;
        close(clientSocket);
        exit(1);
    } else if (bytesReceived == 0) {
        std::cout << "Server hat die Verbindung geschlossen" << std::endl;
        close(clientSocket);
        exit(0);
    }

    std::cout << "Antwort vom Server: " << std::string(buffer, bytesReceived) << std::endl;
}

void runExampleMaxSAT(int clientSocket){
    sendMessage(clientSocket, "ADD_HARD_CLAUSE 1 2 0");
    sendMessage(clientSocket, "ADD_HARD_CLAUSE 1 0");
    sendMessage(clientSocket, "ASSUME -1");
    sendMessage(clientSocket, "SOLVE");
    sendMessage(clientSocket, "SOLVE");
    sendMessage(clientSocket, "RESTART_SOLVER");

    sendMessage(clientSocket, "ADD_HARD_CLAUSE 1 2 0");
    sendMessage(clientSocket, "ADD_SOFT_CLAUSE -1 0 10");
    sendMessage(clientSocket, "ADD_SOFT_CLAUSE 1 0 20");
    sendMessage(clientSocket, "SOLVE");
    sendMessage(clientSocket, "QUERY_LIT 1");
    sendMessage(clientSocket, "QUERY_LIT 2");
}

void sendWCNFOldToServer(int clientSocket, const std::string& filename) {
    std::ifstream wcnfFile(filename);
    if (!wcnfFile.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return; // Beendet die Funktion frühzeitig, wenn die Datei nicht geöffnet werden konnte
    }
    std::string line;
    uint64_t topWeight = 0;
    
    // Skip the header and extract the top weight
    if (std::getline(wcnfFile, line)) {
        std::istringstream iss(line);
        std::string tmp;
        iss >> tmp >> tmp >> tmp >> tmp >> topWeight;
    }

    // Process each clause
    while (std::getline(wcnfFile, line)) {
        std::istringstream iss(line);
        uint64_t weight;
        int lit;
        std::vector<int> literals;

        iss >> weight; // Extract the clause's weight
        while (iss >> lit && lit != 0) {
            literals.push_back(lit); // Collect all literals until the 0 terminator
        }

        // Construct and send the appropriate command
        std::string command;
        if (weight == topWeight) {
            // Hard clause
            command = "ADD_HARD_CLAUSE ";
        } else {
            // Soft clause with its weight
            command = "ADD_SOFT_CLAUSE ";
        }

        for (int literal : literals) {
            command += std::to_string(literal) + " ";
        }
        if (weight < topWeight) {
            // Append the weight for soft clauses
            command += "0 " + std::to_string(weight);
        } else {
            // End hard clauses with a 0
            command += "0";
        }
        //std::cout << command << std::endl;
        sendMessage(clientSocket, command);
    }
}

void sendWCNFNewToServer(int clientSocket, const std::string& filename) {
    std::ifstream wcnfFile(filename);
    if (!wcnfFile.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return; // Exit early if file can't be opened
    }

    std::string line;
    uint64_t topWeight = 0;

    // Skip the header and extract the top weight if available
    std::getline(wcnfFile, line); // Assume first line is the header and ignore it

    // Process each clause
    while (std::getline(wcnfFile, line)) {
        if (line.empty() || line[0] == 'c') continue; // Skip comments or empty lines
        
        std::istringstream iss(line);
        std::string weightIndicator;
        uint64_t weight;
        int lit;
        std::vector<int> literals;

        iss >> weightIndicator; // Could be a weight or 'h' for hard clauses
        
        weight = std::stoull(weightIndicator); // Convert string to unsigned long long int

        while (iss >> lit && lit != 0) {
            literals.push_back(lit); // Collect all literals until the 0 terminator
        }

        // Construct and send the appropriate command
        std::string command;
        if (weightIndicator == "h") {
            command = "ADD_HARD_CLAUSE ";
        } else {
            command = "ADD_SOFT_CLAUSE ";
        }

        for (int literal : literals) {
            command += std::to_string(literal) + " ";
        }

        // Soft clauses, append '0' then the weight; Hard clauses, just '0'
        command += "0";
        if (weightIndicator != "h") {
            command += " " + std::to_string(weight); // Append the weight for soft clauses
        }

        //std::cout << "Sending command to server: " << command << std::endl;
        sendMessage(clientSocket, command);
    }
}


int main() {
    const char* serverIP = "127.0.0.1"; // Beispiel-IP-Adresse des Servers
    int port = 12345; // Beispiel-Port

    // Verbindung zum Server herstellen
    int clientSocket = setupConnection(serverIP, port);

    //runExampleMaxSAT(clientSocket);
    //const std::string filename1= "dim.san400_0.7_3.clq.wcnf";
    //sendWCNFNewToServer(clientSocket, filename1);
    const std::string filename2 = "test.wcnf";
    sendWCNFOldToServer(clientSocket, filename2);
    // Haupt-Loop des Clients
    while (true) {
        sendMessageCMD(clientSocket);
        //receiveMessage(clientSocket);
    }

    // Schließen die Verbindung 
    close(clientSocket);

    return 0;
}
