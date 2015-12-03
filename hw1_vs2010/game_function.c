#include "game_function.h"
#include "Hw1TextFileReader.h"

int InputValidation(char argc, char **argv){
	if (argc < 5)
		return 0;
}

int IsReaderInputValid(TextFileReader textFileReader){
	return textFileReader.IsInitialized;
}

