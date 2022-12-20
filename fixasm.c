#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getLine(char *pNext, char *pLimit, char *pLine, int cbLine)
{
	int iPattern = 0;
	char szPattern[] = "jmp\tDWORD PTR $";
	cbLine--;
	while (pNext < pLimit && cbLine--) {
		char ch = *pNext++;
		if (ch == szPattern[iPattern]) {
			iPattern++;
			if (iPattern == sizeof(szPattern)-1) {
				if (cbLine > 3) {
					*pLine++ = 'C';
					*pLine++ = 'S';
					*pLine++ = ':';
					cbLine -= 3;
				}
				iPattern = 0;
			}
		} else {
			iPattern = 0;
		}
		*pLine++ = ch;
		if (ch == '\n') break;
	}
	*pLine = 0;
	return pNext;
}

main(int argc, char **argv)
{
	char abLine[255];
	FILE *hOutput;
	FILE *hInput = argc > 1? fopen(argv[1], "rb") : NULL;
	if (hInput) {
		unsigned int cbInput;
		fseek(hInput, 0L, SEEK_END);
		cbInput = ftell(hInput);
		fseek(hInput, 0, SEEK_SET);
		if (cbInput) {
			char *pBuffer = malloc(cbInput);
			if (pBuffer) {
				if (cbInput == fread(pBuffer, 1, cbInput, hInput)) {
					fclose(hInput);
					hInput = NULL;
					hOutput = fopen(argv[1], "wb");
					if (hOutput) {
						unsigned int cbLine;
						char *pNext = pBuffer;
						char *pLimit = pBuffer + cbInput;
						while (pNext = getLine(pNext, pLimit, abLine, sizeof(abLine))) {
							cbLine = strlen(abLine);
							if (!cbLine) break;
							if (cbLine != fwrite(abLine, 1, cbLine, hOutput)) {
								printf("error writing file\n");
								break;
							}
						}
						fclose(hOutput);
					}
				} else {
					printf("error reading file\n");
				}
				free(pBuffer);
			}
		}
	} else {
		printf("error opening file\n");
	}
	if (hInput) fclose(hInput);
}
