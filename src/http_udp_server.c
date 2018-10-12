#include"header.h"
int main(int argc, char *argv[])
{

	int listensocket, file_fd, i, n;
	struct  sockaddr_in
		serveraddress,cliaddr;
	char *server_ip = SERVER_IP; 
	char * fstr = "NULL";
	int binderror;
	int index;
	ssize_t read_chars;
	socklen_t len;
	char buf[1024];
	char readbuf[BUFSIZE];
	listensocket = socket(AF_INET, SOCK_DGRAM, 0 );
	if (listensocket < 0 )
	{
		perror("socket" );
		exit(1 );
	}

	memset(&serveraddress, 0, sizeof(serveraddress) );
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(MYPORT);/*PORT NO*/
	serveraddress.sin_addr.s_addr = inet_addr(server_ip);/*ADDRESS*/
	binderror=bind(listensocket,(struct sockaddr*)&serveraddress,sizeof(serveraddress));
	if (-1 == binderror)
	{
		perror("BIND !!!!!!!");
		exit(1);
	}
	listen(listensocket,5);

	/*Beginning of the Main Server Processing Loop*/
	memset(buf,0,sizeof(buf));
	memset(readbuf,0,sizeof(readbuf));
	for (;;)
	{
		printf("Server:I am waiting-----Start of Main Loop\n");
		len=sizeof(cliaddr);
		index = 0;
		memset(buf,0,sizeof(buf));
		n = recvfrom(listensocket,readbuf,1000,0,(struct sockaddr *)&cliaddr,&len);


		for(i=0;i<n;i++)	/* remove CR and LF characters */
			if(readbuf[i] == '\r' || readbuf[i] == '\n')
				readbuf[i]='*';
		if( strncmp(readbuf,"GET ",4) && strncmp(readbuf,"get ",4) ) {
			printf("FORBIDDEN : Only simple GET operation supported");
		}
		for(i=4;i<BUFSIZE;i++) { /* null terminate after the second space to ignore extra stuff */
			if(readbuf[i] == ' ') { /* string is "GET URL " +lots of other stuff */
				readbuf[i] = 0;
				break;
			}
		}
		if( !strncmp(&readbuf[0],"GET /\0",6) || !strncmp(&readbuf[0],"get /\0",6) ) /* convert no filename to index file */
			(void)strcpy(readbuf,"GET /index.html");

	

		if(( file_fd = open(&readbuf[5],O_RDONLY)) == -1) {  /* open the file for reading */
			printf( "NOTFOUND: failed to open file");
		}

		len = (long)lseek(file_fd, (off_t)0, SEEK_END); /* lseek to the file end to find the length */
		(void)lseek(file_fd, (off_t)0, SEEK_SET); /* lseek back to the file start ready for reading */
		(void)sprintf(readbuf,"HTTP/1.1 200 OK\nServer: nweb/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n", VERSION, len, fstr); /* Header + a blank line */
		sendto(listensocket,readbuf,strlen(readbuf),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

		/* send file in 8KB block - last block may be smaller */
		while (	(read_chars = read(file_fd, readbuf, BUFSIZE)) > 0 ) {
			(void)write(listensocket,readbuf,read_chars);
			sendto(listensocket,readbuf,read_chars,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));



		}


		index =0;


	}
	return 0;
	}


