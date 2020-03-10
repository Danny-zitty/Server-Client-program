// Please note this is a C program
// It compiles without warnings with gcc

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
    char buffer[32];
	char major[64];

	printf("Enter server host name: ");
	char hostname[32];
	scanf("%s", hostname);
	server = gethostbyname(hostname);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	printf("Enter server port number: ");
	scanf("%d", &portno);
	if ((portno < 2000) || (portno > 65535))
	{
		fprintf(stderr, "ERROR, invalid port number provided\n");
		exit(1);
	}
	printf("\n");
	
	while (1)
	{
		bzero(major, 64);
		printf("Enter a college major: ");
		int c, earlyPay, midPay;
		while ((c = getchar()) != '\n' && (c != EOF));
		fgets(major, 64, stdin);
		if (major[0] == '\n')
			break;
		ungetc('\n', stdin);

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			error("ERROR opening socket");
		bzero((char *)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR connecting");

		major[strlen(major) - 1] = '\0';
		n = write(sockfd, major, strlen(major));
		if (n < 0)
			error("ERROR writing to socket");

		bzero(buffer, 32);
		n = read(sockfd, buffer, 31);
		if (n < 0)
			error("ERROR reading from socket");
		sscanf(buffer, "%d %d", &earlyPay, &midPay);
		if ((earlyPay == -1) && (midPay == -1))
			printf("That major is not in the table\n");
		else
			printf("The average early career pay for a %s major is $%d\nThe corresponding mid-career pay is $%d\n", major, earlyPay, midPay);

		close(sockfd);
		printf("\n");
	}
	return 0;
}