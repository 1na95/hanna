#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include<regex>
#include<iostream>
#include<string>

using namespace std;

#include<pcap.h>

/* IP 주소 (32 bit = 4 bytes) 를 1 byte 씩 분리하여 읽기 위한 구조체 */
typedef struct ip_address {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header의 각 요소를 분리하여 읽기 위한 구조체 */
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

/* UDP header의 각 요소를 분리하여 읽기 위한 구조체 */
typedef struct udp_header {
	u_short sport;          // Source port
	u_short dport;          // Destination port
	u_short len;            // Datagram length
	u_short crc;            // Checksum
}udp_header;

/* TCP header의 각 요소를 분리하여 읽기 위한 구조체 */
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

/* 함수 prototype 정의 */
void ifprint(int index, pcap_if_t *d);
char *iptos(u_long in);
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen);
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
u_long retrieveIP(pcap_if_t* d);
int compareIP(struct ip_address a, struct ip_address b);
string ip24tos(ip_address in);


/* main 함수 시작 */
int main() {

	printf("패킷 캡쳐 프로그램을 시작합니다.\n");

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* 캡쳐 가능한 모든 NIC 의 리스트를 읽어와 alldevs 구조체에 저장 */
	if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
		exit(1);
	}

	/* alldevs 구조체에 저장된 각 NIC에 대한 정보를 차례로 화면에 출력 */
	for (d = alldevs; d != NULL; d = d->next)
	{
		ifprint(++i, d);
	}

	if (i == 0)
	{
		/* 패킷을 캡쳐할 수 있는 NIC이 존재하지 않는 경우 에러 출력하고 종료 */
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	/* Capture 할 NIC 의 index #를 사용자로부터 입력으로 받음 */
	printf("Enter the NIC # to capture packets (1-%d): ", i);
	int inum;
	scanf_s("%d", &inum);
	if (inum < 1 || inum > i)
	{
		/* 입력한 NIC의 index가 범위를 벗어난 경우, 에러 메시지 출력하고 종료 */
		printf("\nNIC number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 변수 d 에 사용자가 선택한 NIC 에 대한 포인터를 넣어준다 */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* 사용자가 선택한 NIC에서 capture를 하기 위해 capture에 필요한 기본 옵션들을 설정하여 준비 */
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

	/* 링크계층에서 Ethernet을 사용하는지 확인
	Ethernet 을 사용하는 경우, Ethernet header가 14 bytes 이므로,
	capture된 패킷의 14번째 byte부터 IP header가 시작함을 알 수 있음.

	Ethernet이 아닌 경우 IP header를 추출하는 부분의 코드가 변경되어야 함.
	(대부분은 Ethernet을 사용하므로, 이 조건에 어긋나는 경우는 거의 없을 것으로 보임)
	*/
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* IPv4 & TCP 를 사용하는 패킷만 캡쳐하도록 필터 설정 */

	// (필터설정 1단계) netmask 읽어오기 - netmask 가 이후 filter 설정 함수의 인자로 사용되므로
	// 인자로 사용되는 이유는, IPv4 broadcast 주소를 filter 조건에 넣는 경우 netmask 값에 의해 broadcast 주소가 결정되므로
	u_int netmask;
	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface comes without any address, we suppose it to be in a class-C network */
		netmask = 0xffffff;

	// (필터설정 2단계) 아래의 주어진 필터를 컴파일함 
	// 즉, 사람이 이해할 수 있는 'ip and tcp'라는 스트링을 NIC에서 사용할 수 있는 2진수 형태의 필터로 변환
	char packet_filter[] = "ip and tcp";
	struct bpf_program fcode;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	// (필터설정 3단계) 컴파일한 필터를 NIC에 세팅함. 결과로, 필터 조건에 맞는 패킷만 캡쳐하게 됨.
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\n다음 NIC에서 캡쳐를 시작합니다:\n");
	if (d->description)
		printf("%s\n", d->description);
	else
		printf("the selected NIC\n");

	/* 선택한 NIC의 ip주소를 읽어와 저장 */
	/* LSB - MSB 차례로 저장되어있음 */
	u_long NIC_IP_in_long = retrieveIP(d);
	//printf("NIC_IP: <%lu>\n", NIC_IP_in_long);
	/* packet의 ip header에서 ip 주소가 저장되는 구조체와 동일한 구조체 넣음 */
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

	/* 메모리 반환 */
	pcap_freealldevs(alldevs);

	/*
	NIC 에 도달하는 패킷 캡쳐하여 도착 시간, IP주소, 포트번호 출력
	while loop 을 돌며 도달하는 패킷을 지속적으로 관찰함
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
		u_int th_len;			// tcp header 의 길이
		char* payload;

		if (res == 0)
			/* Timeout elapsed */
			continue;

		/* Timestamp 를 읽을 수 있는 형태로 변환 */
		local_tv_sec = header->ts.tv_sec;
		localtime_s(&ltime, &local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);

		/* IP header에 대한 pointer 읽어오기 (packet header의 처음 14byte (Ethernet header)를 지난 지점) */
		ih = (ip_header *)(pkt_data + 14); //length of ethernet header

		/* TCP header 에 대한 pointer 읽어오기 */
		ip_len = (ih->ver_ihl & 0xf) * 4;			// ip header 의 길이가 이 부분에 4를 곱한 Bytes수임
		th = (tcp_header *)((u_char*)ih + ip_len);	// ip header 를 지난 부분이 TCP header 의 시작 부분

		/* convert from network byte order to host byte order (네트워크와 호스트OS에서 사용하는 byte 순서가 다를 경우 변환해 줌)*/
		sport = ntohs(th->sport);
		dport = ntohs(th->dport);

		/* payload에 대한 pointer 읽어오기 */
		//th->hlen 부분에서 처음 4bit가 TCP header 길이를 나타내며, 4byte 단위로 나타내므로, TCP header의 4를 곱하면 바이트 수가 나옴.
		// http://www.ktword.co.kr/abbr_view.php?m_temp1=1889
		th_len = ((th->hlen & 0xf0) >> 4) * 4;
		payload = (char*)th + th_len;

		//string a;

		/*
		Payload에 대해 exact matching하는 예  (www.naver.com 으로 전송된 HTTP GET 패킷 찾기)
		*/
		if (strncmp(payload, "GET", 3) == 0) { // payload의 첫 3 bytes가 "GET"인 경우
			char *pch = strstr(payload, "Host: www.naver.com");
			if (pch != NULL) {	// 위 string을 payload에 포함하는 경우, 즉 "Host: www.naver.com"이 payload에 포함된 경우
				// 패킷의 도착시간, 길이 (in bytes) 출력
				printf("%s.%.6d (%dB) ", timestr, header->ts.tv_usec, header->len);

				// source ip, source port#, destination ip, destination port# 출력 
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
		Payload에 대해 regex matching하는 예	(주민번호 포함한 패킷 찾기)
		*/
		smatch match_result;
		regex regex_ssn("\\d{6}.{0,15}(1|2|3|4)\\d{6}");	// pattern for entire SSN (1/2 2000년 이전 남/녀, 3/4 2000년 포함 이후 남/녀)
		regex regex_ssn_part_2("(1|2|3|4)\\d{6}");			// 마지막 7자리 pattern

		string payload_str = string(payload);		// string 형태로 변환한 후 regex_search에 대입
		bool match = regex_search(payload_str, match_result, regex_ssn);

		if (match == true) {
			// 패킷의 도착시간, 길이 (in bytes) 출력
			printf("%s.%.6d (%dB) ", timestr, header->ts.tv_usec, header->len);

			// source ip, source port#, destination ip, destination port# 출력 
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
			// 첫 6자리 
			string first_6_digits = ((string)match_result[0]).substr(0, 6);

			// 첫 6자리를 지나서 다음 7자리 찾기 
			string payload_middle = ((string)match_result[0]).substr(6, 22);
			regex_search(payload_middle, match_result, regex_ssn_part_2);
			string last_7_digits = ((string)match_result[0]).substr(0, 7);
			//cout << "[" << match_result[0] << "]" << endl;

			cout << "[" << first_6_digits << "-" << last_7_digits << "]: ";

			// 주민등록번호 공식에 따른 검증, http://haneulnoon.tistory.com/11 
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
		} // 주민번호 regex 패턴에 match 하는 경우			



	}	// end of while loop

	if (res == -1) {
		printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
		return -1;
	}

	return 0;
}

/* 두 ip 주소 a와 b를 비교하여,
동일한 경우 0을 반환하고,
다른 경우 1을 반환 */
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

/* 주어진 device 로부터 IPv4 주소 찾아 return */
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


/* 입력 인자로 주어진 NIC에 대한 정보 출력
할당된 IP 주소, 제조사, 모델명 등
ipconfig 명령의 출력 결과와 유사
*/
void ifprint(int index, pcap_if_t *d)
{
	pcap_addr_t *a;
	char ip6str[128];

	/* Index, Description(NIC의 제조사/모델명 등 설명부분) 출력 */
	printf("%d. ", index);
	if (d->description)
		printf(" (%s)\n", d->description);
	else
		printf(" (No description available)\n");

	/* Loopback Address 인지의 여부 출력*/
	printf("\tLoopback: %s\n", (d->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");

	/* NIC에 할당된 IP addresses 출력 */
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



/* Convert a numeric IPv4 address to a string (ifprint 함수 내에서 사용됨) */
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

/* numeric IP의 /24 prefix 부분만 string으로 변환 */
string ip24tos(ip_address in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;

	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	_snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]), "%d.%d.%d.*", in.byte1, in.byte2, in.byte3);
	return string(output[which]);
}

/* Convert a IPv6 address to a string (ifprint 함수 내에서 사용됨) */
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
