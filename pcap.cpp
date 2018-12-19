#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include<regex>
#include<iostream>
#include<string>

using namespace std;

#include<pcap.h>

/* IP �ּ� (32 bit = 4 bytes) �� 1 byte �� �и��Ͽ� �б� ���� ����ü */
typedef struct ip_address {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header�� �� ��Ҹ� �и��Ͽ� �б� ���� ����ü */
typedef struct ip_header {
	u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
	u_char  tos;            // Type of service 
	u_short tlen;           // Total length 
	u_short identification; // Identification
	u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
	u_char  ttl;            // Time to live
	u_char  proto;          // Protocol
	u_short crc;            // Header checksum
	ip_address  saddr;      // Source address
	ip_address  daddr;      // Destination address
	u_int   op_pad;         // Option + Padding
}ip_header;

/* UDP header�� �� ��Ҹ� �и��Ͽ� �б� ���� ����ü */
typedef struct udp_header {
	u_short sport;          // Source port
	u_short dport;          // Destination port
	u_short len;            // Datagram length
	u_short crc;            // Checksum
}udp_header;

/* TCP header�� �� ��Ҹ� �и��Ͽ� �б� ���� ����ü */
typedef struct tcp_header {
	WORD sport;					// source port
	WORD dport;					// destination port
	DWORD seqno;				// sequenz number
	DWORD ackno;				// acknowledge number
	BYTE hlen;					// Header length
	BYTE flag;					// flags
	WORD window;				// window
	WORD chksum;				// checksum
	WORD urgptr;				// urgent pointer
}tcp_header;

/* �Լ� prototype ���� */
void ifprint(int index, pcap_if_t *d);
char *iptos(u_long in);
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen);
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
u_long retrieveIP(pcap_if_t* d);
int compareIP(struct ip_address a, struct ip_address b);
string ip24tos(ip_address in);


/* main �Լ� ���� */
int main() {

	printf("��Ŷ ĸ�� ���α׷��� �����մϴ�.\n");

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* ĸ�� ������ ��� NIC �� ����Ʈ�� �о�� alldevs ����ü�� ���� */
	if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
		exit(1);
	}

	/* alldevs ����ü�� ����� �� NIC�� ���� ������ ���ʷ� ȭ�鿡 ��� */
	for (d = alldevs; d != NULL; d = d->next)
	{
		ifprint(++i, d);
	}

	if (i == 0)
	{
		/* ��Ŷ�� ĸ���� �� �ִ� NIC�� �������� �ʴ� ��� ���� ����ϰ� ���� */
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	/* Capture �� NIC �� index #�� ����ڷκ��� �Է����� ���� */
	printf("Enter the NIC # to capture packets (1-%d): ", i);
	int inum;
	scanf_s("%d", &inum);
	if (inum < 1 || inum > i)
	{
		/* �Է��� NIC�� index�� ������ ��� ���, ���� �޽��� ����ϰ� ���� */
		printf("\nNIC number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* ���� d �� ����ڰ� ������ NIC �� ���� �����͸� �־��ش� */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* ����ڰ� ������ NIC���� capture�� �ϱ� ���� capture�� �ʿ��� �⺻ �ɼǵ��� �����Ͽ� �غ� */
	pcap_t *adhandle;
	if ((adhandle = pcap_open(d->name,          // name of the device
		65536,            // portion of the packet to capture (in bytes)
		// 65536 guarantees that the entire packet will be captured over all layers
		PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
		1000,             // read timeout (1 second)
		NULL,             // authentication on the remote machine
		errbuf            // error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter - not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* ��ũ�������� Ethernet�� ����ϴ��� Ȯ��
	Ethernet �� ����ϴ� ���, Ethernet header�� 14 bytes �̹Ƿ�,
	capture�� ��Ŷ�� 14��° byte���� IP header�� �������� �� �� ����.

	Ethernet�� �ƴ� ��� IP header�� �����ϴ� �κ��� �ڵ尡 ����Ǿ�� ��.
	(��κ��� Ethernet�� ����ϹǷ�, �� ���ǿ� ��߳��� ���� ���� ���� ������ ����)
	*/
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* IPv4 & TCP �� ����ϴ� ��Ŷ�� ĸ���ϵ��� ���� ���� */

	// (���ͼ��� 1�ܰ�) netmask �о���� - netmask �� ���� filter ���� �Լ��� ���ڷ� ���ǹǷ�
	// ���ڷ� ���Ǵ� ������, IPv4 broadcast �ּҸ� filter ���ǿ� �ִ� ��� netmask ���� ���� broadcast �ּҰ� �����ǹǷ�
	u_int netmask;
	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface comes without any address, we suppose it to be in a class-C network */
		netmask = 0xffffff;

	// (���ͼ��� 2�ܰ�) �Ʒ��� �־��� ���͸� �������� 
	// ��, ����� ������ �� �ִ� 'ip and tcp'��� ��Ʈ���� NIC���� ����� �� �ִ� 2���� ������ ���ͷ� ��ȯ
	char packet_filter[] = "ip and tcp";
	struct bpf_program fcode;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	// (���ͼ��� 3�ܰ�) �������� ���͸� NIC�� ������. �����, ���� ���ǿ� �´� ��Ŷ�� ĸ���ϰ� ��.
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\n���� NIC���� ĸ�ĸ� �����մϴ�:\n");
	if (d->description)
		printf("%s\n", d->description);
	else
		printf("the selected NIC\n");

	/* ������ NIC�� ip�ּҸ� �о�� ���� */
	/* LSB - MSB ���ʷ� ����Ǿ����� */
	u_long NIC_IP_in_long = retrieveIP(d);
	//printf("NIC_IP: <%lu>\n", NIC_IP_in_long);
	/* packet�� ip header���� ip �ּҰ� ����Ǵ� ����ü�� ������ ����ü ���� */
	struct ip_address nic_ip;
	nic_ip.byte1 = NIC_IP_in_long & 0xFF;
	NIC_IP_in_long >>= 8;
	nic_ip.byte2 = NIC_IP_in_long & 0xFF;
	NIC_IP_in_long >>= 8;
	nic_ip.byte3 = NIC_IP_in_long & 0xFF;
	NIC_IP_in_long >>= 8;
	nic_ip.byte4 = NIC_IP_in_long & 0xFF;
	printf("%d.%d.%d.%d\n",
		nic_ip.byte1,
		nic_ip.byte2,
		nic_ip.byte3,
		nic_ip.byte4
	);
	printf("\n=================================\n");

	/* �޸� ��ȯ */
	pcap_freealldevs(alldevs);

	/*
	NIC �� �����ϴ� ��Ŷ ĸ���Ͽ� ���� �ð�, IP�ּ�, ��Ʈ��ȣ ���
	while loop �� ���� �����ϴ� ��Ŷ�� ���������� ������
	*/
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0) {
		struct tm ltime;
		char timestr[16];
		time_t local_tv_sec;
		ip_header *ih;
		u_int ip_len;
		tcp_header *th;
		u_short sport, dport;
		u_int th_len;			// tcp header �� ����
		char* payload;

		if (res == 0)
			/* Timeout elapsed */
			continue;

		/* Timestamp �� ���� �� �ִ� ���·� ��ȯ */
		local_tv_sec = header->ts.tv_sec;
		localtime_s(&ltime, &local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);

		/* IP header�� ���� pointer �о���� (packet header�� ó�� 14byte (Ethernet header)�� ���� ����) */
		ih = (ip_header *)(pkt_data + 14); //length of ethernet header

		/* TCP header �� ���� pointer �о���� */
		ip_len = (ih->ver_ihl & 0xf) * 4;			// ip header �� ���̰� �� �κп� 4�� ���� Bytes����
		th = (tcp_header *)((u_char*)ih + ip_len);	// ip header �� ���� �κ��� TCP header �� ���� �κ�

		/* convert from network byte order to host byte order (��Ʈ��ũ�� ȣ��ƮOS���� ����ϴ� byte ������ �ٸ� ��� ��ȯ�� ��)*/
		sport = ntohs(th->sport);
		dport = ntohs(th->dport);

		/* payload�� ���� pointer �о���� */
		//th->hlen �κп��� ó�� 4bit�� TCP header ���̸� ��Ÿ����, 4byte ������ ��Ÿ���Ƿ�, TCP header�� 4�� ���ϸ� ����Ʈ ���� ����.
		// http://www.ktword.co.kr/abbr_view.php?m_temp1=1889
		th_len = ((th->hlen & 0xf0) >> 4) * 4;
		payload = (char*)th + th_len;

		//string a;

		/*
		Payload�� ���� exact matching�ϴ� ��  (www.naver.com ���� ���۵� HTTP GET ��Ŷ ã��)
		*/
		if (strncmp(payload, "GET", 3) == 0) { // payload�� ù 3 bytes�� "GET"�� ���
			char *pch = strstr(payload, "Host: www.naver.com");
			if (pch != NULL) {	// �� string�� payload�� �����ϴ� ���, �� "Host: www.naver.com"�� payload�� ���Ե� ���
				// ��Ŷ�� �����ð�, ���� (in bytes) ���
				printf("%s.%.6d (%dB) ", timestr, header->ts.tv_usec, header->len);

				// source ip, source port#, destination ip, destination port# ��� 
				printf("%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d",
					ih->saddr.byte1,
					ih->saddr.byte2,
					ih->saddr.byte3,
					ih->saddr.byte4,
					sport,
					ih->daddr.byte1,
					ih->daddr.byte2,
					ih->daddr.byte3,
					ih->daddr.byte4,
					dport
				);

				if (compareIP(ih->saddr, nic_ip) == 0) {
					printf("  >>> outbound >>>\n");
				}
				else { printf("  <<< inbound <<<\n"); }
			}
		}


		/*
		Payload�� ���� regex matching�ϴ� ��	(�ֹι�ȣ ������ ��Ŷ ã��)
		*/
		smatch match_result;
		regex regex_ssn("\\d{6}.{0,15}(1|2|3|4)\\d{6}");	// pattern for entire SSN (1/2 2000�� ���� ��/��, 3/4 2000�� ���� ���� ��/��)
		regex regex_ssn_part_2("(1|2|3|4)\\d{6}");			// ������ 7�ڸ� pattern

		string payload_str = string(payload);		// string ���·� ��ȯ�� �� regex_search�� ����
		bool match = regex_search(payload_str, match_result, regex_ssn);

		if (match == true) {
			// ��Ŷ�� �����ð�, ���� (in bytes) ���
			printf("%s.%.6d (%dB) ", timestr, header->ts.tv_usec, header->len);

			// source ip, source port#, destination ip, destination port# ��� 
			printf("%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d",
				ih->saddr.byte1,
				ih->saddr.byte2,
				ih->saddr.byte3,
				ih->saddr.byte4,
				sport,
				ih->daddr.byte1,
				ih->daddr.byte2,
				ih->daddr.byte3,
				ih->daddr.byte4,
				dport
			);

			if (compareIP(ih->saddr, nic_ip) == 0) {
				printf("  >>> outbound >>>\n");
			}
			else { printf("  <<< inbound <<<\n"); }

			//printf("%s\n", payload);
			//cout << "[" << match_result[0] << "]" << endl;
			// ù 6�ڸ� 
			string first_6_digits = ((string)match_result[0]).substr(0, 6);

			// ù 6�ڸ��� ������ ���� 7�ڸ� ã�� 
			string payload_middle = ((string)match_result[0]).substr(6, 22);
			regex_search(payload_middle, match_result, regex_ssn_part_2);
			string last_7_digits = ((string)match_result[0]).substr(0, 7);
			//cout << "[" << match_result[0] << "]" << endl;

			cout << "[" << first_6_digits << "-" << last_7_digits << "]: ";

			// �ֹε�Ϲ�ȣ ���Ŀ� ���� ����, http://haneulnoon.tistory.com/11 
			int N = atoi(first_6_digits.substr(0, 1).c_str()) * 2
				+ atoi(first_6_digits.substr(1, 1).c_str()) * 3
				+ atoi(first_6_digits.substr(2, 1).c_str()) * 4
				+ atoi(first_6_digits.substr(3, 1).c_str()) * 5
				+ atoi(first_6_digits.substr(4, 1).c_str()) * 6
				+ atoi(first_6_digits.substr(5, 1).c_str()) * 7

				+ atoi(last_7_digits.substr(0, 1).c_str()) * 8
				+ atoi(last_7_digits.substr(1, 1).c_str()) * 9
				+ atoi(last_7_digits.substr(2, 1).c_str()) * 2
				+ atoi(last_7_digits.substr(3, 1).c_str()) * 3
				+ atoi(last_7_digits.substr(4, 1).c_str()) * 4
				+ atoi(last_7_digits.substr(5, 1).c_str()) * 5
				;

			N = (11 - (N % 11)) % 10;
			int last_digit = atoi(last_7_digits.substr(6, 1).c_str());

			if (N == last_digit) cout << "Conform to the KSSN rule" << endl;
			else cout << "Not a KSSN" << endl;
		} // �ֹι�ȣ regex ���Ͽ� match �ϴ� ���			



	}	// end of while loop

	if (res == -1) {
		printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
		return -1;
	}

	return 0;
}

/* �� ip �ּ� a�� b�� ���Ͽ�,
������ ��� 0�� ��ȯ�ϰ�,
�ٸ� ��� 1�� ��ȯ */
int compareIP(struct ip_address a, struct ip_address b) {
	int rt_val = 1;

	if ((a.byte1 == b.byte1) &&
		(a.byte2 == b.byte2) &&
		(a.byte3 == b.byte3) &&
		(a.byte4 == b.byte4))
	{
		rt_val = 0;		//equal ip addresses
	}

	return rt_val;
}

/* �־��� device �κ��� IPv4 �ּ� ã�� return */
u_long retrieveIP(pcap_if_t* d) {
	pcap_addr_t *a;
	u_long ipv4_addr;

	for (a = d->addresses; a; a = a->next) {
		switch (a->addr->sa_family)
		{
		case AF_INET:	// IPv4 address			
			if (a->addr)
				ipv4_addr = ((struct sockaddr_in *)a->addr)->sin_addr.s_addr;
			break;
		}
	}

	return ipv4_addr;
}


/* �Է� ���ڷ� �־��� NIC�� ���� ���� ���
�Ҵ�� IP �ּ�, ������, �𵨸� ��
ipconfig ����� ��� ����� ����
*/
void ifprint(int index, pcap_if_t *d)
{
	pcap_addr_t *a;
	char ip6str[128];

	/* Index, Description(NIC�� ������/�𵨸� �� ����κ�) ��� */
	printf("%d. ", index);
	if (d->description)
		printf(" (%s)\n", d->description);
	else
		printf(" (No description available)\n");

	/* Loopback Address ������ ���� ���*/
	printf("\tLoopback: %s\n", (d->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");

	/* NIC�� �Ҵ�� IP addresses ��� */
	for (a = d->addresses; a; a = a->next) {
		switch (a->addr->sa_family)
		{
		case AF_INET:
			printf("\tIPv4 address: ");
			if (a->addr)
				printf("%s ", iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
			if (a->netmask)
				printf("%s ", iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
			printf("\n");
			break;

		case AF_INET6:
			printf("\tIPv6 address: ");
			if (a->addr)
				printf("%s", ip6tos(a->addr, ip6str, sizeof(ip6str)));
			printf("\n");
			break;

		default:
			printf("\tUnknown address type\n");
			break;
		}
	}
	printf("\n");
}



/* Convert a numeric IPv4 address to a string (ifprint �Լ� ������ ����) */
#define IPTOSBUFFERS    12		// # of IP string buffers
char *iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;
	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);	// move to the next string buffer, wrapping if necessary
	_snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}

/* numeric IP�� /24 prefix �κи� string���� ��ȯ */
string ip24tos(ip_address in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;

	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	_snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]), "%d.%d.%d.*", in.byte1, in.byte2, in.byte3);
	return string(output[which]);
}

/* Convert a IPv6 address to a string (ifprint �Լ� ������ ����) */
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen)
{
	socklen_t sockaddrlen;

	sockaddrlen = sizeof(struct sockaddr_in6);

	if (getnameinfo(sockaddr,
		sockaddrlen,
		address,
		addrlen,
		NULL,
		0,
		NI_NUMERICHOST) != 0) address = NULL;

	return address;
}
