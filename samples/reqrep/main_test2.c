/*
 * main_test2.c
 *
 *  Created on: 29.04.2012
 *      Author: YaroslavLitvinov
 *  REQREP example is simulates two network nodes transmitting data from each to other.
 *  Data packet size=1000000bytes, and it's sending 100times in loop from one node to another and vice versa.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "zrt.h"

int main(int argc, char **argv){
	WRITE_LOG("test2 started\n");

	int fda = open("/dev/out/testa", O_WRONLY);
	int fdb = open("/dev/in/testb", O_RDONLY);
	WRITE_FMT_LOG("test2: fda=%d, fdb=%d\n", fda, fdb);

	int testlen = 1000000;
	char *buf = malloc(testlen+1);
	buf[testlen] = '\0';
	for (int i=0; i < 10; i++){
		ssize_t bwrote = write(fda, buf, testlen);
		WRITE_FMT_LOG("#%d case1: write passed=%d, wrote=%d\n", i, testlen, (int)bwrote );
		assert( bwrote == testlen );
		ssize_t bread = read(fdb, buf, testlen);
		WRITE_FMT_LOG("#%d case2: read requested=%d, read=%d\n", i, testlen, (int)bread );
		assert( bread == testlen );
	}
	free(buf);

	close(fda);
	close(fdb);
	WRITE_LOG("exit\n");
	return 0;
}
