#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// result
#define FAIL	-1
#define SUCCESS	0

// modes
#define FIND	1
#define RUN	0

int main(void)
{
	char buf_ipconfig[2048] = { 0 };
	char buf_ipv4[100] = { 0 };
	char remained[2048] = { 0 };
	char mode = FIND;
	FILE* fp_ipconfig = NULL;
	char dummy1[20] = { 0 };
	char dummy2[20] = { 0 };
	char ethernet[100] = { 0 };
	int ip[4] = { 0, 0, 0, 0 };
	char ip_addr[16] = { 0 };
	int count = 1;
	int ipid = 3;
	char command1[200] = { 0 };
	char command2[200] = { 0 };
	char command3[200] = { 0 };
	char target_ip[100] = { 0 };
	char temp_target_ip[100] = { 0 };

	do {
		printf("\n Enter the target ip you want\n");
		scanf_s("%s", target_ip);
		strcpy_s(temp_target_ip, sizeof(temp_target_ip), target_ip);
		if (strlen(target_ip) > 16)
		{
			printf("\n ERROR : Invalid target ip\n");
			memset(target_ip, 0, sizeof(target_ip));
			continue;
		}
		for (int i = 0; i < strlen(target_ip); i++)
		{
			strtok_s(target_ip, ".", remained);
			if (strlen(remained) == 0)
			{
				printf("\n ERROR : Invalid target ip\n");
				memset(target_ip, 0, sizeof(target_ip));
				break;
			}
			for (int j = 1; j < 3; j++)
			{
				strtok_s(NULL, ".", remained);
				if (strlen(remained) == 0)
				{
					printf("\n ERROR : Invalid target ip\n");
					memset(target_ip, 0, sizeof(target_ip));
					break;
				}
			}
			if (strlen(target_ip) == 0)
			{
				break;
			}
		}
	} while (strlen(target_ip) == 0);

	strcpy_s(target_ip, sizeof(target_ip), temp_target_ip);

	system("ipconfig /all > ipconfig.txt");
	fopen_s(&fp_ipconfig, "ipconfig.txt", "r");

	if (fp_ipconfig == NULL)
	{
		printf("파일이 존재하지 않습니다\n");
		return FAIL;
	}
	while(fgets(buf_ipconfig, sizeof(buf_ipconfig), fp_ipconfig) != NULL)
	{
		if (mode == RUN) {
			if (strstr(buf_ipconfig, "IPv4") != NULL)
			{
				strcpy_s(buf_ipv4, sizeof(buf_ipv4), buf_ipconfig);
				for (int i = strlen(buf_ipv4) - 1; i >= 0; i--) {
					if (buf_ipv4[i] >= 0x30 && buf_ipv4[i] <= 0x39) {
						ip[ipid] += (buf_ipv4[i] - 0x30) * count;
						count *= 10;
					}
					if (buf_ipv4[i] == '.') {
						count = 1;
						ipid--;
					}
					if (buf_ipv4[i] == ' ' && ip[0] > 0) {
						printf("\n your current ethernet ip : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
						fclose(fp_ipconfig);
						_unlink("ipconfig.txt");
						break;
					}
				}
				break;
			}
		}
		else if (strstr(buf_ipconfig, "Ethernet") != NULL)
		{
			mode = RUN;
		}
		else if (strstr(buf_ipconfig, "이더넷") != NULL) {
			strtok_s(buf_ipconfig, ":", remained);
			strcpy_s(ethernet, sizeof(ethernet), &buf_ipconfig[14]);
			printf("\n your ethernet interface name : %s\n", ethernet);
		}
	}

	_itoa_s(ip[0], ip_addr, sizeof(ip_addr), 10);
	ip_addr[strlen(ip_addr)] = '.';
	for (int j = 1; j <= 3; j++)
	{
		_itoa_s(ip[j], ip_addr, sizeof(ip_addr), 10);
		if (j < 3)
		{
			if (strstr(target_ip, ip_addr) == NULL) {
				printf("ERROR : Invalid target ip\n");
				return FAIL;
			}
			ip_addr[strlen(ip_addr)] = '.';
		}
	}
	sprintf_s(command1, sizeof(command1),
		"netsh interface ip set address \"%s\" static %s 255.255.255.0 %d.%d.%d.1 > nul",
		ethernet,  target_ip, ip[0], ip[1], ip[2]
	);
	sprintf_s(command2, sizeof(command2),
		"netsh interface ip set dnsservers \"%s\" static 203.248.252.2 primary > nul",
		ethernet
	);
	sprintf_s(command3, sizeof(command3),
		"netsh interface ip add dnsservers \"%s\" 164.124.101.2 index=2 > nul",
		ethernet
	);
	printf("\n Now, the commands below will be excuted\n %s\n", command1);
	printf(" %s\n", command2);
	printf(" %s\n", command3);
	system(command1);
	Sleep(1000);
	system(command2);
	Sleep(1000);
	system(command3);
	Sleep(3000);
	system("ipconfig /all");
	printf("\n Now your ethernet interface is set. Check the above\n");
	_getch();

	return SUCCESS;
}
