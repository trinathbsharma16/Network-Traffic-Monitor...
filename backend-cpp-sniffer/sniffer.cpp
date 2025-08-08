#include <pcap.h>
#include <iostream>
#include <cstring>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live("en0", BUFSIZ, 1, 1000, errbuf); // use eth0 or en0
    if (handle == nullptr) {
        std::cerr << "Couldn't open device: " << errbuf << std::endl;
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    auto packet_handler = [&](u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
        const struct ip* ipHeader = (struct ip*)(packet + 14);
        char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipHeader->ip_src), src, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ipHeader->ip_dst), dst, INET_ADDRSTRLEN);
        std::string msg = std::string(src) + " -> " + std::string(dst);
        sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        std::cout << msg << std::endl;
    };

    pcap_loop(handle, 0, [](u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
        (*(decltype(packet_handler)*)args)(args, header, packet);
    }, (u_char*)&packet_handler);

    pcap_close(handle);
    close(sock);
    return 0;
}
