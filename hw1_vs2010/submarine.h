#ifndef SUBMARINE_H
#define SUBMARINE_H
#include "Hw1TextFileReader.h"

#define MAX_CHARS_IN_STRING 100

#define LOW 100
#define DEEP 200

//input validation
typedef enum { FRIEND = 0, FOE = 1 } SUBOPPONENT;
typedef enum { CEASE = 0, FIRE = 1 } GUNSTATUS;
/*
	basic field of struct Submarine
*/
typedef struct Submarine{
	int depth;
	int direction;
	int ammo;	//the amount of torpedos 
}Submarine;

typedef struct Decision{
	BOOL IsLineEmpty;
	int depth;
	int direction;
	GUNSTATUS gun_status;
	int ammo;	//the amount of torpedos 
	char* message;
}Decision;

typedef struct GameLine {
	char* Name;
	SUBOPPONENT SubOpponent;
	int Azimut;
	int Distance;
	BOOL IsLineEmpty;
	int visited;
	int in_danger;
} GameLine;

int InputValidation(char argc, char **argv);
int IsReaderInputValid(TextFileReader textFileReader);
struct Submarine *CreateMySubmarine(char argc, char **argv);
int TextFileReaderToGameLineArray(TextFileReader text_file_reader, GameLine* game_line_array);
int ProccessGame(int number_of_lines, GameLine* game_line_array, Submarine* dolphin,Decision* decisions_array);
int ValidateTextFileReader(TextFileReader textFileReader);
#endif