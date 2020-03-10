// A simple server in the internet domain using TCP
// The port number is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

struct salaries
{
	char major[64];
	int earlyPay;
	int midPay;
	struct salaries* next;
};

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, clilen, n;
	struct sockaddr_in serv_addr, cli_addr;
	struct salaries* head = (struct salaries*)malloc(sizeof(struct salaries));
	struct salaries* current = head;
	char buffer[64];
	char reply[32];
	char fileName[32];
	FILE *fp;

	printf("Enter file name: ");
	scanf("%s", fileName);
	fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "ERROR, file does not exist\n");
		exit(1);
	}

	while (1)
	{
		struct salaries* node = (struct salaries*)malloc(sizeof(struct salaries));
		bzero(node->major, 64);
		node->next = NULL;
		current->next = node;
		current = node;

		char c;
		int i = 0;
		while ((c = getc(fp)) != '\t' && (c != EOF) && (i != 64))
		{
			node->major[i] = c;
			i++;
		}
		if (c == EOF)
			break;
		fscanf(fp, "%d \t%d", &node->earlyPay, &node->midPay);
		while ((c = getc(fp)) != '\n' && (c != EOF));
		if (c == EOF)
			break;
	}
	fclose(fp);

	/*current = head;
	while ((current = current->next) != NULL)
	{
		printf("%s %d %d\n", current->major, current->earlyPay, current->midPay);
		//current = current->next;
	}
	return 0;///////////////////*/

	printf("Enter server port number: ");
	scanf("%d", &portno);
	if ((portno < 2000) || (portno > 65535))
	{
		fprintf(stderr, "ERROR, invalid port number provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		printf("Connection accepted.\n");

		bzero(buffer, 64);
		n = read(newsockfd, buffer, 63);
		if (n < 0)
			error("ERROR reading from socket");
		printf("Data request for '%s'.\n", buffer);

		bzero(reply, 32);
		sprintf(reply, "-1 -1");
		current = head;
		while ((current = current->next) != NULL)
		{
			if (strcmp(buffer, current->major) == 0)
			{
				bzero(reply, 32);
				sprintf(reply, "%d %d", current->earlyPay, current->midPay);
				break;
			}
		}
		n = write(newsockfd, reply, strlen(reply));
		if (n < 0)
			error("ERROR writing to socket");

		close(newsockfd);
		printf("Connection terminated.\n");
	}
	return 0;
}