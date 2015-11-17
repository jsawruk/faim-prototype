#include <stdio.h>

#import "audio.h"

int main(int argc, char *argv[]) {
	
	Audio *audio = new Audio();
	int initReturn = audio->init();

	if (initReturn != 0) {
		return -1;
	}

	audio->openStream();

	printf("Press enter to exit\n");
	getchar();

	audio->closeStream();

	return 0;
}