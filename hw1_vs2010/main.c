#define _CRT_SECURE_NO_WARNINGS /* to suppress Visual Studio 2010 compiler warning */
#include <stdio.h>
#include "Hw1TextFileReader.h"
#include "submarine.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


void main(int argc, char **argv)
{
	GameLine *game_line_array;
	Submarine *dolphin;
	TextFileReader textFileReader;
	Decision* decisions_array;
	FILE *file;

	if (!InputValidation(argc, argv))
		exit(1);
		printf("\n");
	dolphin = CreateMySubmarine(argc,argv);
	if (dolphin == NULL)
		exit(1);
	
	file=fopen(argv[5],"w+");
	if (file == NULL)
		exit(1);

	textFileReader = CreateTextFileReader(argv[1]);
	if (textFileReader.IsInitialized == FALSE)
	{
		printf("Failed to create the Text File Reader");
		exit(1);
	}

	if (textFileReader.NumOfLines == 0 || !ValidateTextFileReader(textFileReader))
		exit(1);

	game_line_array =(GameLine*) malloc(textFileReader.NumOfLines * sizeof(*game_line_array));
	
	if (game_line_array == NULL)
		exit(1);

	if (!TextFileReaderToGameLineArray(textFileReader, game_line_array))
		exit(1);

	decisions_array = (Decision*) malloc(textFileReader.NumOfLines * sizeof(*decisions_array));

	if (decisions_array == NULL)
		exit(1);

	if (!ProccessGame(textFileReader.NumOfLines, game_line_array,dolphin,decisions_array))
		exit(1);
	

	DeleteTextFileReader(textFileReader);

	printf("Hello World");
	while (1);

}