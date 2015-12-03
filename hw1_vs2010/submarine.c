#define _CRT_SECURE_NO_WARNINGS /* to suppress Visual Studio 2010 compiler warning */

#include "submarine.h"
#include "Hw1TextFileReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#define MAX_DISTANCE 100000000
#define MAX_CHARS_IN_MESSAGE 200
#define MAX_DISTANCE_FOR_DANGER 1000
#define _CRT_SECURE_NO_WARNINGS /* to suppress Visual Studio 2010 compiler warning */
#define PI 3.14159265

int Makedecision(Submarine* dolphin, Decision* decision ,int is_dolphin_in_danger, int is_friend_in_danger);

int ProccessTimeZone(Submarine* dolphin, GameLine* time_zone, int number_of_lines,Decision* decisions);
int GetDistance (int location_a, int location_b);

int InputValidation(char argc, char **argv){
	if (argc != 6)
		return 0;
}

int IsReaderInputValid(TextFileReader textFileReader){
	return textFileReader.IsInitialized;
}

//------------------------
// Create functions
//------------------------

struct Submarine *CreateMySubmarine(char argc, char **argv) {
	// Try to allocate vector structure.

	struct Submarine *newSub =(Submarine*) malloc(sizeof(struct Submarine));
	if (newSub == NULL)
		return NULL;


	newSub->direction = atoi(argv[2]);
	newSub->depth = atoi(argv[3]);
	newSub->ammo = atoi(argv[4]);

	return newSub;
}

int CreateGameLine(GameLine* gameLine)
{
	gameLine = (GameLine*) malloc(sizeof(*gameLine));
	if (gameLine == NULL)
		return 0;
	gameLine->Azimut = 0;
	gameLine->Distance = 0;
	gameLine->IsLineEmpty = TRUE;
	gameLine->SubOpponent = FOE;
	gameLine->visited = 0;
	gameLine->in_danger = 0;
	return 1;
}

int CreateDecision(Decision* decision)
{
	decision = (Decision*) malloc(sizeof *decision);
	if (decision == NULL)
		return 0;
	decision->IsLineEmpty=TRUE;
	decision->direction = 0;
	decision->depth = 0;
	decision->ammo = 0;
	decision->gun_status = CEASE;

	return 1;
}

//----------------------------------
// arrange inputs to game parameters
//----------------------------------

int SaveGameLineProperty(int WordInd, GameLine* game_line_array, ConstLine file_line)
{
	switch (WordInd)
	{
	case 0: game_line_array->Name = (char*) file_line[0]; break;
	case 1:
		if (strcmp("FRIEND", file_line[1]) == 0)
		{
			game_line_array->SubOpponent = FRIEND;
		}
		else
		{
			game_line_array->SubOpponent = FOE;
		}
		break;
	case 2:
		game_line_array->Azimut = atoi(file_line[2]);
		break;
	case 3:
		game_line_array->Distance = atoi(file_line[3]);
		break;
	default:
		printf("Bad input, quitting!\n");
		return 0;
		break;
	}
	return 1;
}


int TextFileReaderToGameLineArray(TextFileReader text_file_reader, GameLine* game_line_array)
{
	int LineInd;
	int WordInd;
	for (LineInd = 0; LineInd < text_file_reader.NumOfLines; LineInd++)
	{
		if (!CreateGameLine(&game_line_array[LineInd]))
			return 0;
		//printf("<%d><%d>\n",LineInd,text_file_reader.WordsInLine[LineInd]);
		if (text_file_reader.WordsInLine[LineInd] > 0)
		{
			for (WordInd = 0; WordInd < text_file_reader.WordsInLine[LineInd]; WordInd++)
			{
				//printf("<%s>", text_file_reader.WordsArr[LineInd][WordInd]);
				if (!SaveGameLineProperty(WordInd, &game_line_array[LineInd], text_file_reader.WordsArr[LineInd]))
				{
					return 0;
				}
			}
			//printf("\n");
			game_line_array[LineInd].IsLineEmpty = FALSE;
			//printf("\nname - %s\nazimut - %d\n", game_line_array[LineInd].Name, game_line_array[LineInd].Azimut);
			//printf("distance - %d\nsubopponent - %d\n", game_line_array[LineInd].Distance, game_line_array[LineInd].SubOpponent);
		}
		else
		{
			//printf("\n");
			game_line_array[LineInd].IsLineEmpty = TRUE;
		}
	}
	return 1;
}

int AddGameLineToTimeZoneArray(GameLine* time_zone, GameLine* game_line, int line_ind)
{
	if (!CreateGameLine(&time_zone[line_ind]))
		return 0;
	time_zone[line_ind].Azimut = game_line->Azimut;
	time_zone[line_ind].Distance = game_line->Distance;
	time_zone[line_ind].IsLineEmpty = game_line->IsLineEmpty;
	time_zone[line_ind].Name = game_line->Name;
	time_zone[line_ind].SubOpponent = game_line->SubOpponent;
	return 1;
}

//----------------------------------
// Game's Logic
//----------------------------------

int ProccessGame(int number_of_lines, GameLine* game_line_array, Submarine* dolphin,Decision* decisions_array)
{
	int LineInd;
	int time_zone_line_counter = 0;
	int decision_line_count = 0;
	int *ptr_decision_line_count = &decision_line_count;
	//bazbezani but in order not using realloc.
	GameLine *time_zone = (GameLine*) malloc(number_of_lines * sizeof *time_zone);
	if (time_zone == NULL)
	{
		return 0;
	}

	for (LineInd = 0; LineInd < number_of_lines; LineInd++)
	{
		if (game_line_array[LineInd].IsLineEmpty)
		{
			if (!ProccessTimeZone(dolphin, time_zone, time_zone_line_counter,decisions_array,ptr_decision_line_count))
				return 0;
			//printf("<%d> <%d> <%d> <%d> \n",decisions_array->direction,decisions_array->depth,decisions_array->gun_status,decisions_array->ammo);
			decisions_array = decisions_array + (*ptr_decision_line_count+1)*sizeof(Decision); // (*ptr_decision_line_count);
			*ptr_decision_line_count = 0;
			time_zone_line_counter = 0;
			//elapse time_zone array to zero
			free(time_zone);
			time_zone = (GameLine*) malloc(number_of_lines * sizeof *time_zone);
			if (time_zone == NULL)
				return 0;
		}
		else
		{
			if (!AddGameLineToTimeZoneArray(time_zone, &game_line_array[LineInd], time_zone_line_counter))
				return 0;
			time_zone_line_counter++;
		}
	}
	return 1;
}

//----------------------------------
// Time_Zone's Logic
//----------------------------------
int IsFriendInDanger(Submarine* dolphin, GameLine* time_zone, int number_of_lines)
{
	int friend_ind;
	int foe_ind;
	int foe_candidate_distance;
	for (friend_ind =0; friend_ind < number_of_lines ; friend_ind++)       //start svcan the time_zone
	{  
		if (time_zone[friend_ind].SubOpponent == FRIEND)               
		{
			for (foe_ind =0; foe_ind < number_of_lines ; foe_ind++)
			{         
				if (time_zone[foe_ind].SubOpponent == FOE)
				{ 
					foe_candidate_distance = GetDistance(time_zone[foe_ind].Distance,time_zone[foe_ind].Azimut,time_zone[friend_ind].Distance,time_zone[friend_ind].Azimut);
					if (foe_candidate_distance<1000)
						return 1;
				}
			}
		}
	}
}

int GetClosestFoeToFireFriendInDanger(Submarine* dolphin, GameLine* time_zone,int number_of_lines)
{
	int foe_direction;
	int max_distance = MAX_DISTANCE;
	int friend_ind;
	int foe_ind;
	int foe_candidate_distance;
	for (friend_ind =0; friend_ind < number_of_lines ; friend_ind++)    
	{  
		if (time_zone[friend_ind].SubOpponent == FRIEND)               
		{
			for (foe_ind =0; foe_ind < number_of_lines ; foe_ind++)
			{         
				if (time_zone[foe_ind].SubOpponent == FOE)
				{ 
					foe_candidate_distance = GetDistance(time_zone[foe_ind].Distance,time_zone[foe_ind].Azimut,time_zone[friend_ind].Distance,time_zone[friend_ind].Azimut);
					if (foe_candidate_distance<1000 && foe_candidate_distance<max_distance)
					{
						foe_direction = time_zone[foe_ind].Azimut;
						max_distance=foe_candidate_distance;
					}
				}
			}
		}
	}
	return foe_direction;
}

int GetClosestFoeToFireDolphinInDanger(Submarine* dolphin, GameLine* time_zone,int number_of_lines)
{
	int foe_direction;
	int max_distance = MAX_DISTANCE;
	int line_ind;
	int foe_candidate_distance;
	for (line_ind =0; line_ind < number_of_lines ; line_ind++)    
	{  
		if (time_zone[line_ind].Distance <500 && dolphin[line_ind].depth<199 && time_zone[line_ind].SubOpponent == FOE && time_zone[line_ind].Distance<max_distance)
		{
			foe_candidate_distance = time_zone[line_ind].Distance;
			foe_direction = time_zone[line_ind].Azimut;
		}
	}
	return foe_direction;
}
int MoreThanOneFriendInDanger(GameLine* time_zone,int number_of_lines)
{
	int friend_in_danger_counter = 0;
	int friend_ind;
	int foe_ind;
	int foe_candidate_distance;
	for (friend_ind =0; friend_ind < number_of_lines ; friend_ind++)    
	{  
		if (time_zone[friend_ind].SubOpponent == FRIEND)               
		{
			for (foe_ind =0; foe_ind < number_of_lines ; foe_ind++)
			{         
				if (time_zone[foe_ind].SubOpponent == FOE)
				{ 
					foe_candidate_distance = GetDistance(time_zone[foe_ind].Distance,time_zone[foe_ind].Azimut,time_zone[friend_ind].Distance,time_zone[friend_ind].Azimut);
					if (foe_candidate_distance<1000)
					{
						friend_in_danger_counter++;
					}
				}
			}
		}
	}
	return (friend_in_danger_counter>1);
}
int Makedecision(Submarine* dolphin, GameLine* time_zone ,int is_dolphin_in_danger, int is_friend_in_danger,int number_of_lines)
{
	int foe_direction;
	if (is_dolphin_in_danger)
	{
		if (is_friend_in_danger)
		{
			if (MoreThanOneFriendInDanger(time_zone,number_of_lines))
			{
				foe_direction = GetClosestFoeToFireFriendInDanger(dolphin,time_zone,number_of_lines);
				printf("%d 200 FIRE %d\n",foe_direction,dolphin->ammo);
				dolphin->ammo = dolphin->ammo -1;
			}
			else
			{
				foe_direction = GetClosestFoeToFireFriendInDanger(dolphin,time_zone,number_of_lines);
				printf("%d 200 FIRE %d\n",foe_direction,dolphin->ammo);
				dolphin->ammo = dolphin->ammo -1;
			}
		}
		else
		{
			foe_direction = GetClosestFoeToFireDolphinInDanger(dolphin,time_zone,number_of_lines);
			printf("%d 200 FIRE %d\n",foe_direction,dolphin->ammo);
			dolphin->ammo = dolphin->ammo -1;
		}
	}
	else
	{
		if (is_friend_in_danger)
		{
			foe_direction = GetClosestFoeToFireFriendInDanger(dolphin,time_zone,number_of_lines);
			printf("%d 200 FIRE %d\n",foe_direction,dolphin->ammo);
			dolphin->ammo = dolphin->ammo -1;
		}
		else
		{
					printf("dolphin not in danger and friend is not\n");
		}
	}
}

int ProccessTimeZone(Submarine* dolphin, GameLine* time_zone, int number_of_lines,Decision* decisions,int* ptr_decision_line_count)
{
	
	int is_dolphin_in_danger = IsDolphinInDanger (dolphin,time_zone,number_of_lines); 
	int is_friend_in_danger = IsFriendInDanger (dolphin,time_zone,number_of_lines);
	Makedecision (dolphin,time_zone,is_dolphin_in_danger,is_friend_in_danger,number_of_lines);
	return 1;
}

// IsFriendInDanger run on all foes and check id onef them is closer than 500 && depth distance is less the 199
int IsDolphinInDanger(Submarine* dolphin, GameLine* time_zone, int number_of_lines) 
{
	int time_zone_ind = 0;
	int line_ind = 0;
	for (line_ind = 0; line_ind < number_of_lines; line_ind++)
	{
		if (time_zone[line_ind].Distance <500 && dolphin[line_ind].depth<199 && time_zone[line_ind].SubOpponent == FOE)
			return 1;
	}
	return 0;
}


// IsFriendInDanger run on all freind and foes and check the most in dager friend, saving all friends in visited field
//int IsFriendInDanger(Submarine* dolphin, GameLine* time_zone, int number_of_lines, Decision* decisions,int* ptr_decision_line_count)
//
//{
//	int friend_ind = 0;
//	int foe_ind = 0;
//	int friend_candidate_distance = 0;
//	int foe_candidate_distance = 0;
//	int friend_shortest_distance =100000000;
//	int foe_shortest_distance = 100000000;
//	int chosen_azimut_of_foe = -1;
//	int chosen_azimut_of_friend = -1;
//	char* choosed_friend_to_protect = NULL;
//	int returned_value_friend_in_danger = 0;
//
//	for (friend_ind =0; friend_ind < number_of_lines ; friend_ind++)       //start svcan the time_zone
//	{  
//		if (time_zone[friend_ind].SubOpponent == FRIEND)                   //for every friend
//		{          
//			
//			if(time_zone[friend_ind].visited != 1)    
//			{
//				time_zone[friend_ind].visited = 1;
//			}
//			else 
//			{
//				//if allready visited, keep azimut for decision
//				friend_candidate_distance = GetDistance(0,0,time_zone[friend_ind].Distance,time_zone[friend_ind].Azimut);
//				if (friend_candidate_distance < friend_shortest_distance)
//				{
//				chosen_azimut_of_friend = time_zone[friend_ind].Azimut;
//				}
//			}
//			for (foe_ind =0; foe_ind < number_of_lines ; foe_ind++)
//			{         
//				// for every friend - run on all foes and update the most close foe
//				if (time_zone[foe_ind].SubOpponent == FOE)
//				{ 
//					foe_candidate_distance = GetDistance(time_zone[foe_ind].Distance,time_zone[foe_ind].Azimut,time_zone[friend_ind].Distance,time_zone[friend_ind].Azimut);
//					if (foe_candidate_distance<1000)
//						time_zone[friend_ind].in_danger = 1;
//					if (foe_candidate_distance < foe_shortest_distance)
//					{
//						//shortest_distance = GetDistance(time_zone[foe_ind].Distance,time_zone[friend_ind].Distance);
//						chosen_azimut_of_foe = time_zone[foe_ind].Azimut;							
//						choosed_friend_to_protect = time_zone[friend_ind].Name; // in case other friend in danger, we will know who was the protected
//					}
//				}
//			}
//		}
//	}
//
//	if (chosen_azimut_of_foe != -1 || foe_shortest_distance > MAX_DISTANCE_FOR_DANGER  )
//	{   
//		decisions[*ptr_decision_line_count].direction = chosen_azimut_of_friend;
//		decisions[*ptr_decision_line_count].gun_status = CEASE;
//		decisions[*ptr_decision_line_count].ammo = dolphin->ammo;	
//	}
//	else if (foe_shortest_distance < 1000 && dolphin->ammo > 0)
//	{
//		returned_value_friend_in_danger = 1;
//		decisions[*ptr_decision_line_count].direction = chosen_azimut_of_foe;
//		decisions[*ptr_decision_line_count].gun_status = FIRE;
//		decisions[*ptr_decision_line_count].ammo = dolphin->ammo -1;  
//		for  (friend_ind =0; friend_ind < number_of_lines ; friend_ind++)
//		{
//			if ((time_zone[friend_ind].SubOpponent == FRIEND) && (time_zone[friend_ind].in_danger == 1) && (strcmp(time_zone[friend_ind].Name,choosed_friend_to_protect)))
//			{
//				(*ptr_decision_line_count)++;
//				decisions[*ptr_decision_line_count].message = (char*) malloc(sizeof(char));
//				if (decisions[*ptr_decision_line_count].message == NULL)
//					exit(1);
//				strcpy(decisions[*ptr_decision_line_count].message,"Ship ");
//				strcpy(decisions[*ptr_decision_line_count].message,time_zone[friend_ind].Name);
//				strcpy(decisions[*ptr_decision_line_count].message," is in danger");
//			}
//		}
//	}
//	else if (foe_shortest_distance < 1000 & dolphin->ammo == 0){
//		//TODO printf   ship "choosed_friend_to_protect" is at danger		
//	}
//	return returned_value_friend_in_danger;
//}


	

int ValidateTextFileReader(TextFileReader textFileReader)
{
	int LineInd;
	int WordInd;
	for (LineInd = 0; LineInd < textFileReader.NumOfLines; LineInd++)
	{
		if (textFileReader.WordsInLine[LineInd] != 0)
		{
			if (textFileReader.WordsArr[LineInd][0] == '\0') return 0;
			if (strcmp(textFileReader.WordsArr[LineInd][1],"FOE") && strcmp(textFileReader.WordsArr[LineInd][1],"FRIEND"))	return 0;
			if (!isdigit(textFileReader.WordsArr[LineInd][2][0]) && !isdigit(textFileReader.WordsArr[LineInd][3][0])) return 0;
		}
	}
	return 1;
}



int GetDistance (int distance1, int azimut1,int distance2,int azimut2 )
{
	double x1 =distance1*cos((azimut1%360)*PI/180.0);
	double x2 =distance2*cos((azimut2%360)*PI/180.0);
	double y1 =distance1*sin((azimut1%360)*PI/180.0);
	double y2 =distance2*sin((azimut2%360)*PI/180.0);
	double distance = sqrt(pow((x2-x1),2) + pow((y2-y1),2));
	return (int) floor(distance);
}