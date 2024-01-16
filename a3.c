//------------------------------------------------------------------------------
// main.c
//
// A card game loosely inspired by Ohanami in which two players compete to score
// the most points by creating rows of cards. The game is played in two phases:
// In the first phase, players choose two cards from their hand cards and add
// them to their chosen cards. In the second phase, players choose a chosen card
// and add it to a row. The game ends when both players have no hand cards and
// no chosen cards left. The player with the most points wins the game.
//
// Group: Matthias_Bergman
//
// Author: 12320035
//------------------------------------------------------------------------------
//

#define _POSIX_C_SOURCE 200809L // Necessary to suppress a warning in getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define WRONG_ARGUMENT_COUNT 1
#define CANNOT_OPEN_FILE 2
#define INVALID_FILE 3
#define MEMORY_ALLOCATION_ERROR 4

const int CONFIG_CARDS_LINE_START = 3;
const int CONFIG_CARDS_LINE_END = 23;
const int MAX_CARD_ROWS = 3;


enum _Color_
{
  RED = 'r',
  GREEN = 'g',
  BLUE = 'b',
  WHITE = 'w',
};
typedef enum _Color_ Color;

struct _Card_
{
  Color color_;
  int value_;
  struct _Card_ *next_;
};
typedef struct _Card_ Card;


int checkMainArgumentsCount(int argc);
int getPlayersCount(char *config_file);
void printWelcomeMessage(int players_count);
void printCardChoosingPhase();
void printActionPhase();

// File functions
FILE *openFile(char *config_file);
int checkConfigFile(char *config_file);

// Card functions
Card *createCard(char *config_file_line);
int assignCardsToPlayers(Card **player_one_handcards, Card **player_two_handcards, char *config_file);
Card *getCardFromHand(Card *player_handcards, int card_number);
Card *getCardFromChosen(Card *player_chosencards, int card_number);
int exchangePlayerCards(Card **player_one_handcards, Card **player_two_handcards);
int sortCards(Card **player_cards);
Color parseColor(char *color);
void swapCards(Card **first_card, Card **second_card);

// Player functions
void printPlayer(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);
int addCardToHand(Card **player_handcards, Card *card);
void addCardToChosen(Card **player_chosencards, Card *card);
int removeCardFromHand(Card **player_handcards, Card *card);
int removeCardFromChosen(Card **player_chosencards, Card *card);
int addCardToRow(Card **player_cardrows, Card *card, int row_number);
int calculatePlayerPoints(Card ***player_cardrows);

int placeAction(char *input, bool *skip_prompt, int player_id, Card **player_handcards,
                Card **player_chosencards, Card ***player_cardrows);
int discardAction(char *input, bool *skip_prompt, int player_id, Card **player_handcards,
                  Card **player_chosencards, Card ***player_cardrows);

// Ask user input
int chooseCardToKeep(int player_id, Card **player_handcards, Card **player_chosencards);
int cardChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                      Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows);
int actionChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                        Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows);
int isActionInputCorrect(char *row_number, const char *card_number);
int actionChoosingLoop(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);
void freePlayer(Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);
void printPlayerPoints(char *config_file, Card ***player_one_cardrows, Card ***player_two_cardrows);
void writePlayerPointsToFile(char *config_file, int player_one_points, int player_two_points);
void printPlayerHandCards(Card *const *player_handcards);
void printPlayerChosenCards(Card *const *player_chosencards);
void printPlayerCardRows(Card **const *player_cardrows);
void helpAction(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);
void singleRowPointsCount(Card *head, int *points, int *row_length);
void freeLinkedList(Card* head);

//---------------------------------------------------------------------------------------------------------------------
///
/// This is the main function of the program. It checks if the correct number of arguments is given and if the config
/// file is valid. Then it creates the players and starts the game. The game takes place in a loop until
/// both players have no hand cards and no chosencards left. In each loop the player chooses two cards from his hand
/// cards and adds them to his chosen cards. Then the player chooses a chosen card and adds it to a row. The game ends
/// when both players have no hand cards and no chosencards left. The player with the most points wins the game.
///
/// @param argc The number of arguments
/// @param argv The arguments, argv[1] is the config file
///
/// @return
///      0 if the program was executed successfully
///      1 if the wrong number of arguments was given
///      2 if the config file could not be opened
///      3 if the config file is invalid
///      4 if there was a memory allocation error
//
int main(int argc, char *argv[])
{
  if (checkMainArgumentsCount(argc) != 0)
  {
    return WRONG_ARGUMENT_COUNT;
  }
  int config_file_error = checkConfigFile(argv[1]);
  if (config_file_error != 0)
  {
    return config_file_error;
  }
  int players_count = getPlayersCount(argv[1]);
  if (players_count == -1)
  {
    return CANNOT_OPEN_FILE;
  }
  printWelcomeMessage(players_count);
  Card *player_one_handcards = NULL;
  Card *player_one_chosencards = NULL;
  Card **player_one_cardrows = malloc(sizeof(Card**) * MAX_CARD_ROWS);
  if (player_one_cardrows == NULL)
  {
    return MEMORY_ALLOCATION_ERROR;
  }
  Card *player_two_handcards = NULL;
  Card *player_two_chosencards = NULL;
  Card **player_two_cardrows = malloc(sizeof(Card**) * MAX_CARD_ROWS);
  if (player_two_cardrows == NULL)
  {
    free(player_one_cardrows);
    return MEMORY_ALLOCATION_ERROR;
  }
  for (int i = 0; i < MAX_CARD_ROWS; i++)
  {
    player_one_cardrows[i] = NULL;
    player_two_cardrows[i] = NULL;
  }
  int assign_cards_error = assignCardsToPlayers(&player_one_handcards, &player_two_handcards, argv[1]);
  if (assign_cards_error != 0)
  {
    free(player_one_cardrows);
    free(player_two_cardrows);
    return assign_cards_error;
  }
  sortCards(&player_one_handcards);
  sortCards(&player_two_handcards);
  bool break_early = false;
  do
  {
    printCardChoosingPhase();
    if (cardChoosingPhase(&player_one_handcards, &player_one_chosencards, &player_one_cardrows,
                          &player_two_handcards, &player_two_chosencards, &player_two_cardrows) == 1)
    {
      freePlayer(&player_one_handcards, &player_one_chosencards, &player_one_cardrows);
      freePlayer(&player_two_handcards, &player_two_chosencards, &player_two_cardrows);
      break_early = true;
      break;
    }
    exchangePlayerCards(&player_one_handcards, &player_two_handcards);
    printActionPhase();
    if (actionChoosingPhase(&player_one_handcards, &player_one_chosencards, &player_one_cardrows,
                            &player_two_handcards, &player_two_chosencards, &player_two_cardrows) == 1)
    {
      freePlayer(&player_one_handcards, &player_one_chosencards, &player_one_cardrows);
      freePlayer(&player_two_handcards, &player_two_chosencards, &player_two_cardrows);
      break_early = true;
      break;
    }
  } while ((player_one_handcards != NULL || player_one_chosencards != NULL) &&
           (player_two_handcards != NULL || player_two_chosencards != NULL));
  if (!break_early)
  {
    printf("\n");
    printPlayerPoints(argv[1], &player_one_cardrows, &player_two_cardrows);
    freePlayer(&player_one_handcards, &player_one_chosencards, &player_one_cardrows);
    freePlayer(&player_two_handcards, &player_two_chosencards, &player_two_cardrows);
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function converts a given string to an integer using the strtol function. It returns the converted integer if
/// the conversion was successful and -1 otherwise. It also strips away whitespaces before the conversion.
///
/// @param str The string to convert
///
/// @return
///      -1 if the conversion failed
///      the converted integer if the conversion was successful
//
int stringToInt(const char *str)
{
  // Strip out spaces and newlines
  char sanitized_str[strlen(str) + 1];
  int j = 0;
  for (size_t i = 0; i < strlen(str); i++)
  {
    if (str[i] != ' ' && str[i] != '\n')
    {
      sanitized_str[j++] = str[i];
    }
  }
  sanitized_str[j] = '\0';
  char *endptr;
  long value = strtol(sanitized_str, &endptr, 10);
  // Check for conversion errors
  if (*endptr != '\0' || value < INT_MIN || value > INT_MAX)
  {
    return -1; // Conversion failed
  }
  return (int)value; // Conversion successful
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints the points of both players and the winner of the game to the console and writes the same
/// information to the config file.
///
/// @param config_file The path to the config file
/// @param player_one_cardrows The row of cards from the first player
/// @param player_two_cardrows The row of cards from the second player
///
/// @return void
//
void printPlayerPoints(char *config_file, Card ***player_one_cardrows, Card ***player_two_cardrows)
{
  int player_one_points = calculatePlayerPoints(player_one_cardrows);
  int player_two_points = calculatePlayerPoints(player_two_cardrows);
  if (player_one_points < player_two_points)
  {
    printf("Player 2: %i points\n", player_two_points);
    printf("Player 1: %i points\n", player_one_points);
    printf("\n");
    printf("Congratulations! Player 2 wins the game!\n");
  }
  else
  {
    printf("Player 1: %i points\n", player_one_points);
    printf("Player 2: %i points\n", player_two_points);
    printf("\n");
    printf("Congratulations! Player 1 wins the game!\n");
  }
  writePlayerPointsToFile(config_file, player_one_points, player_two_points);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function writes the points of both players and the winner of the game to the config file.
///
/// @param config_file The path to the config file
/// @param player_one_points The points of the first player
/// @param player_two_points The points of the second player
///
/// @return void
//
void writePlayerPointsToFile(char *config_file, int player_one_points, int player_two_points)
{
  FILE *file = fopen(config_file, "a");
  if (file == NULL)
  {
    printf("Error: Cannot open file: %s\n", config_file);
    return;
  }
  if (player_one_points < player_two_points)
  {
    fprintf(file, "\n");
    fprintf(file, "Player 2: %i points\n", player_two_points);
    fprintf(file, "Player 1: %i points\n", player_one_points);
    fprintf(file, "\n");
    fprintf(file, "Congratulations! Player 2 wins the game!\n");
  }
  else
  {
    fprintf(file, "Player 1: %i points\n", player_one_points);
    fprintf(file, "Player 2: %i points\n", player_two_points);
    fprintf(file, "\n");
    fprintf(file, "Congratulations! Player 1 wins the game!\n");
  }
  fclose(file);
  file = NULL;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function checks if the correct number of arguments is given.
///
/// @param argc The number of arguments
///
/// @return
///      0 if the correct number of arguments is given
///      1 if the wrong number of arguments is given
//
int checkMainArgumentsCount(int argc)
{
  if (argc != 2)
  {
    printf("Usage: ./a3 <config file>\n");
    return 1;
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function opens a file and returns a pointer to it.
///
/// @param config_file The path to the config file
///
/// @return
///      NULL if the file could not be opened
///      file pointer if the file could be opened
//
FILE *openFile(char *config_file)
{
  FILE *file = fopen(config_file, "r");
  if (file == NULL)
  {
    printf("Error: Cannot open file: %s\n", config_file);
    return NULL;
  }
  return file;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function reads a line from a file and returns it.
///
/// @param file The file to read from
/// @param line_number The line number to read
///
/// @return
///      NULL if the line could not be read
///      a string containing the line if the line could be read
//
char *readLine(FILE *file, int line_number)
{
  char *line = NULL;
  size_t len = 0;
  fseek(file, 0, SEEK_SET);
  for (int i = 0; i < line_number; i++)
  {
    getline(&line, &len, file);
  }
  char *result = strdup(line);
  free(line);
  line = NULL;
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function checks if the config file is valid. It checks if the magic number is correct and if the number of
/// players is correct.
///
/// @param config_file The path to the config file
///
/// @return
///      0 if the config file is valid
///      1 if the config file could not be opened
///      2 if the config file is invalid
//
int checkConfigFile(char *config_file)
{
  FILE *file = openFile(config_file);
  if (file == NULL) {
    return CANNOT_OPEN_FILE;
  }
  char *magic_number = "ESP\n";
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  read = getline(&line, &len, file);
  if (read == -1)
  {
    printf("Error: Invalid file: %s\n", config_file);
    fclose(file);
    free(line);
    return INVALID_FILE;
  }
  if (strcmp(line, magic_number) != 0)
  {
    printf("Error: Invalid file: %s\n", config_file);
    free(line);
    fclose(file);
    return INVALID_FILE;
  }
  free(line);
  fclose(file);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function gets the number of players from the config file. Its not really necessary, as we only play with two
/// players, but it might be useful to extend the game to more players.
///
/// @param config_file The path to the config file
///
/// @return
///      -1 if the file could not be opened
///      the number of players if the file could be opened
//
int getPlayersCount(char *config_file)
{
  // The number of players is defined in the second line of the file
  FILE *file = openFile(config_file);
  if (file == NULL)
  {
    return -1;
  }
  char *line = readLine(file, 2);
  int players_count = stringToInt(line);
  free(line);
  fclose(file);
  return players_count;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints a welcome message to the console.
///
/// @param players_count The number of players
///
/// @return void
//
void printWelcomeMessage(int players_count)
{
  printf("Welcome to SyntaxSakura (%i players are playing)!\n", players_count);
  printf("\n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function parses a string and returns the corresponding color.
///
/// @param color The string to parse
///
/// @return
///      RED if the string is "r"
///      GREEN if the string is "g"
///      BLUE if the string is "b"
///      WHITE if the string is "w"
///      '\0' if the string is invalid
//
Color parseColor(char *color)
{
  if (strcmp(color, "r") == 0)
  {
    return RED;
  }
  else if (strcmp(color, "g") == 0)
  {
    return GREEN;
  }
  else if (strcmp(color, "b") == 0)
  {
    return BLUE;
  }
  else if (strcmp(color, "w") == 0)
  {
    return WHITE;
  }
  else
  {
    printf("Error: Invalid color: %s\n", color);
    return '\0';
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function creates a card from a line in the config file. It returns a pointer to the card.
///
/// @param config_file_line The line from the config file
///
/// @return
///      NULL if the card could not be created
///      a pointer to the card if the card could be created
//
Card *createCard(char *config_file_line)
{
  Card *card = malloc(sizeof(Card));
  if (card == NULL)
  {
    printf("Error: Memory allocation error\n");
    return NULL;
  }
  int value = stringToInt(strtok(config_file_line, "_"));
  char *color = strtok(NULL, "_");
  color[strcspn(color, "\n")] = 0;
  card->color_ = parseColor(color);
  card->value_ = value;
  card->next_ = NULL;
  return card;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function assigns the cards from the config file to the players. It returns 0 if the cards could be assigned
/// successfully and an error code otherwise. It starts with the first line of cards in the config file and assigns
/// every second card to the first player and every other second card to the second player.
///
/// @param player_one_handcards The hand cards of the first player
/// @param player_two_handcards The hand cards of the second player
/// @param config_file The path to the config file
///
/// @return
///      0 if the cards could be assigned successfully
///      2 if the file could not be opened
///      4 if there was a memory allocation error
//
int assignCardsToPlayers(Card **player_one_handcards, Card **player_two_handcards, char *config_file)
{
  FILE *file = openFile(config_file);
  if (file == NULL)
  {
    return CANNOT_OPEN_FILE;
  }
  for (int i = CONFIG_CARDS_LINE_START; i < CONFIG_CARDS_LINE_END; i++)
  {
    char *line = readLine(file, i);
    Card *temp_card = createCard(line);
    free(line);
    line = NULL;
    if (temp_card == NULL)
    {
      fclose(file);
      return MEMORY_ALLOCATION_ERROR;
    }
    if (i % 2 != 0)
    {
      // Assign card to player one
      addCardToHand(player_one_handcards, temp_card);
    }
    else
    {
      addCardToHand(player_two_handcards, temp_card);
    }
  }
  fclose(file);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function exchanges the hand cards of the two players.
///
/// @param player_one_handcards The hand cards of the first player
/// @param player_two_handcards The hand cards of the second player
///
/// @return
///      0 if the cards could be exchanged successfully
//
int exchangePlayerCards(Card **player_one_handcards, Card **player_two_handcards)
{
  Card *temp_hand_cards = *player_one_handcards;
  *player_one_handcards = *player_two_handcards;
  *player_two_handcards = temp_hand_cards;
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints the card choosing phase text to the console.
///
/// @return void
//
void printCardChoosingPhase()
{
  printf("-------------------\n");
  printf("CARD CHOOSING PHASE\n");
  printf("-------------------\n");
  printf("\n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints the action phase text to the console.
///
/// @return void
//
void printActionPhase()
{
  printf("------------\n");
  printf("ACTION PHASE\n");
  printf("------------\n");
  printf("\n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function performs the card choosing phase. It starts with the first player and lets him choose two cards from
/// his hand cards and add them to his chosen cards. Then it does the same for the second player. It returns 0 if the
/// card choosing phase could be performed successfully and 1 otherwise.
///
/// @param player_one_handcards The hand cards of the first player
/// @param player_one_chosencards The chosen cards of the first player
/// @param player_one_cardrows The card rows of the first player
/// @param player_two_handcards The hand cards of the second player
/// @param player_two_chosencards The chosen cards of the second player
/// @param player_two_cardrows The card rows of the second player
///
/// @return
///      0 if the card choosing phase could be performed successfully
///      1 if the card choosing phase could not be performed successfully
//
int cardChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                      Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows)
{
  printPlayer(1, player_one_handcards, player_one_chosencards, player_one_cardrows);
  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(1, player_one_handcards, player_one_chosencards) == 1)
  {
    return 1;
  }
  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(1, player_one_handcards, player_one_chosencards) == 1)
  {
    return 1;
  }
  printf("\n");
  printPlayer(2, player_two_handcards, player_two_chosencards, player_two_cardrows);
  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(2, player_two_handcards, player_two_chosencards) == 1)
  {
    return 1;
  }
  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(2, player_two_handcards, player_two_chosencards) == 1)
  {
    return 1;
  }
  printf("\n");
  printf("Card choosing phase is over - passing remaining hand cards to the next player!\n");
  printf("\n");
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints the details of a player to the console. It prints the hand cards, the chosen cards and the card
/// rows of the player.
///
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return void
//
void printPlayer(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows)
{
  printf("Player %i:\n", player_id);
  printPlayerHandCards(player_handcards);
  printPlayerChosenCards(player_chosencards);
  printPlayerCardRows(player_cardrows);
  printf("\n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// A helper function to print the card on each row of a player. It will print nothing, if the row is empty.
///
/// @param player_cardrows The card rows of the player
///
/// @return void
//
void printPlayerCardRows(Card **const *player_cardrows)
{
  Card *head = NULL;
  if (player_cardrows != NULL && *player_cardrows != NULL)
  {
    for (int i = 0; i < MAX_CARD_ROWS; i++)
    {
      if ((*player_cardrows)[i] != NULL && (*player_cardrows)[i]->color_ != '\0')
      {
        printf("  row_%i: ", i+1);
        head = (*player_cardrows)[i];
        while (head != NULL)
        {
          if (head->next_ != NULL)
          {
            printf("%i_%c ", head->value_, head->color_);
          }
          else
          {
            printf("%i_%c\n", head->value_, head->color_);
          }
          head = head->next_;
        }
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// A helper function to print all the chosen cards of a player. It will print just the label "chosen cards:" if the
/// player has no chosen cards.
///
/// @param player_chosencards The chosen cards of the player
///
/// @return void
//
void printPlayerChosenCards(Card *const *player_chosencards)
{
  printf("  chosen cards:");
  Card *head = *player_chosencards;
  if (head != NULL && head->color_ != '\0')
  {
    printf(" ");
  }
  else
  {
    printf("\n");
  }
  while (head != NULL && head->color_ != '\0')
  {
    if (head->next_ != NULL)
    {
      printf("%i_%c ", head->value_, head->color_);
    }
    else
    {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// A helper function to print all the hand cards of a player. It will print just the label "hand cards:" if the player
/// has no hand cards.
///
/// @param player_handcards The hand cards of the player
///
/// @return void
//
void printPlayerHandCards(Card *const *player_handcards)
{
  printf("  hand cards:");
  Card *head = *player_handcards;
  if (head != NULL && head->color_ != '\0')
  {
    printf(" ");
  }
  else
  {
    printf("\n");
  }
  while (head != NULL && head->color_ != '\0')
  {
    if (head->next_ != NULL)
    {
      printf("%i_%c ", head->value_, head->color_);
    }
    else
    {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Retrieves a card from the hand cards of a player by searching for its value. It returns a pointer to the card.
///
/// @param player_handcards The hand cards of the player
/// @param card_number The value of the card to retrieve
///
/// @return
///      NULL if the card could not be found
///      a pointer to the card if the card could be found
//
Card *getCardFromHand(Card *player_handcards, int card_number)
{
  Card *head = player_handcards;
  while (head != NULL)
  {
    if (head->value_ == card_number)
    {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Retrieves a card from the chosen cards of a player by searching for its value. It returns a pointer to the card.
///
/// @param player_chosencards The chosen cards of the player
/// @param card_number The value of the card to retrieve
///
/// @return
///      NULL if the card could not be found
///      a pointer to the card if the card could be found
//
Card *getCardFromChosen(Card *player_chosencards, int card_number)
{
  Card *head = player_chosencards;
  while (head != NULL)
  {
    if (head->value_ == card_number)
    {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function simulates the choosing phase, where each player chooses two cards to keep from his hand cards. It
/// returns 0 if the action choosing phase could be performed successfully and 1 otherwise.
///
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return
///      0 if the action choosing phase could be performed successfully
///      1 if the action choosing phase could not be performed successfully
//
int chooseCardToKeep(int player_id, Card **player_handcards, Card **player_chosencards)
{
  Card *chosen_card;
  do
  {
    chosen_card = NULL;
    int card_number;
    printf("P%i > ", player_id);
    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    input[strcspn(input, "\n")] = 0;
    // Check if input is empty or only contains spaces
    if (strlen(input) == 0 || strspn(input, " ") == strlen(input))
    {
      printf("Please enter the number of a card in your hand cards!\n");
      free(input);
      continue;
    }
    if (strcmp(input, "quit") == 0)
    {
      free(input);
      return 1;
    }
    else if (strncmp(input, "quit", 4) == 0 && strlen(input) > 4)
    {
      printf("Please enter the correct number of parameters!\n");
      free(input);
      continue;
    }
    else if (stringToInt(input) < 1 || stringToInt(input) > 120)
    {
      printf("Please enter the number of a card in your hand cards!\n");
      free(input);
      continue;
    }
    card_number = stringToInt(input);

    chosen_card = getCardFromHand(*player_handcards, card_number);
    if (chosen_card == NULL)
    {
      printf("Please enter the number of a card in your hand cards!\n");
    }
    else
    {
      free(input);
      break;
    }
    free(input);
  } while (true);
  if (chosen_card != NULL)
  {
    removeCardFromHand(player_handcards, chosen_card);
    addCardToChosen(player_chosencards, chosen_card);
  }
  else
  {
    printf("Error: Chosen card is NULL\n");
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function adds a card to the chosen cards of a player. It adds the card to the correct position in the list
/// based on its value, keeping the list in ascending order.
///
/// @param player_chosencard The chosen cards of the player
/// @param card The card to add
///
/// @return void
//
void addCardToChosen(Card **player_chosencard, Card *card)
{
  if (*player_chosencard == NULL || (*player_chosencard)->color_ == '\0')
  {
    *player_chosencard = card;
    (*player_chosencard)->next_ = NULL;
    return;
  }
  Card *head = *player_chosencard;
  Card *prev = NULL;
  while (head != NULL && card->value_ > head->value_)
  {
    prev = head;
    head = head->next_;
  }
  if (prev == NULL)
  {
    // Insert at the beginning of the list
    card->next_ = *player_chosencard;
    *player_chosencard = card;
  }
  else
  {
    // Insert in the middle or at the end of the list
    prev->next_ = card;
    card->next_ = head;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function removes a card from the chosen cards of a player. It loops through the list and removes the card if
/// it is found. It returns 0 if the card could be removed and 1 otherwise.
///
/// @param player_chosencards The chosen cards of the player
/// @param card The card to remove
///
/// @return
///      0 if the card could be removed
///      1 if the card could not be removed
//
int removeCardFromChosen(Card **player_chosencards, Card *card)
{
  if (*player_chosencards == NULL)
  {
    return 1;
  }
  if ((*player_chosencards)->value_ == card->value_)
  {
    *player_chosencards = card->next_;
    card->next_ = NULL;
    return 0;
  }
  Card *head = *player_chosencards;
  while (head->next_ != NULL && head->next_ != card)
  {
    head = head->next_;
  }
  if (head->next_ != NULL)
  {
    head->next_ = card->next_;
    card->next_ = NULL;
    return 0;
  }
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function adds a card to the hand cards of a player. It adds the card to list in a way that keeps the list in
/// ascending order.
///
/// @param player_handcards The hand cards of the player
/// @param card The card to add
///
/// @return
///      0 if the card could be added
///      1 if the card could not be added
//
int addCardToHand(Card **player_handcards, Card *card)
{
  Card *head = *player_handcards;
  if (head == NULL)
  {
    *player_handcards = card;
    (*player_handcards)->next_ = NULL;
    return 0;
  }
  while (head != NULL)
  {
    if (head->next_ == NULL)
    {
      head->next_ = card;
      card->next_ = NULL;
      return 0;
    }
    head = head->next_;
  }
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function removes a card from the hand cards of a player. It loops through the list and removes the card if it
/// is found. It returns 0 if the card could be removed and 1 otherwise.
///
/// @param player_handcards The hand cards of the player
/// @param card The card to remove
///
/// @return
///      0 if the card could be removed
///      1 if the card could not be removed
//
int removeCardFromHand(Card **player_handcards, Card *card)
{
  if (*player_handcards == NULL)
  {
    return 1;
  }
  else if ((*player_handcards)->value_ == card->value_)
  {
    *player_handcards = card->next_;
    card->next_ = NULL;
    return 0;
  }
  Card *head = *player_handcards;
  while (head->next_ != NULL && head->next_ != card)
  {
    head = head->next_;
  }
  if (head->next_ != NULL)
  {
    head->next_ = card->next_;
    card->next_ = NULL;
    return 0;
  }
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function adds a card to a row of cards of a player. It adds the card to the correct position in the list based
/// on its value, keeping the list in ascending order. It uses the row number to determine the correct row.
/// The card can only be added if the card can be added at the beginning or end of the row.
///
/// @param player_cardrows The card rows of the player
/// @param card The card to add
/// @param row_number The row number to add the card to
///
/// @return
///      0 if the card could be added
///      1 if the card could not be added
//
int addCardToRow(Card **player_cardrows, Card *card, int row_number)
{
  card->next_ = NULL;
  Card *current = player_cardrows[row_number];
  Card *prev = NULL;
  if (current == NULL)
  {
    player_cardrows[row_number] = card;
    return 0;
  }
  if (card->value_ < current->value_)
  {
    card->next_ = current;
    player_cardrows[row_number] = card;
    return 0;
  }
  while (current != NULL && card->value_ > current->value_)
  {
    prev = current;
    current = current->next_;
  }
  if (current == NULL)
  {
    prev->next_ = card;
    return 0;
  }
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function swaps two cards. This is useful to sort them properly and only acts as a helper function.
///
/// @param first_card The first card to swap
/// @param second_card The second card to swap
///
/// @return void
//
void swapCards(Card **first_card, Card **second_card)
{
  Color temp_color = (*first_card)->color_;
  int temp_value = (*first_card)->value_;
  (*first_card)->color_ = (*second_card)->color_;
  (*first_card)->value_ = (*second_card)->value_;
  (*second_card)->color_ = temp_color;
  (*second_card)->value_ = temp_value;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function sorts the cards of a player. It uses the bubble sort algorithm to sort the cards in ascending order.
///
/// @param player_cards The cards of the player
///
/// @return
///      0 if the cards could be sorted
///      1 if the cards could not be sorted
//
int sortCards(Card **player_cards)
{
  if (*player_cards == NULL || (*player_cards)->next_ == NULL) {
    return 1;
  }
  int swapped;
  Card *ptr1;
  Card *lptr = NULL;
  do
  {
    swapped = 0;
    ptr1 = *player_cards;
    while (ptr1->next_ != lptr)
    {
      if (ptr1->value_ > ptr1->next_->value_)
      {
        swapCards(&ptr1, &(ptr1->next_));
        swapped = 1;
      }
      ptr1 = ptr1->next_;
    }
    lptr = ptr1;
  } while (swapped);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function simulates the action phase. It starts with the first player and lets him choose a card from his chosen
/// cards and add it to one of his card rows. Then it does the same for the second player. It returns 0 if the action
/// phase could be performed successfully and 1 otherwise.
///
/// @param player_one_handcards The hand cards of the first player
/// @param player_one_chosencards The chosen cards of the first player
/// @param player_one_cardrows The card rows of the first player
/// @param player_two_handcards The hand cards of the second player
/// @param player_two_chosencards The chosen cards of the second player
/// @param player_two_cardrows The card rows of the second player
///
/// @return
///      0 if the action phase could be performed successfully
///      1 if the action phase could not be performed successfully
//
int actionChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                        Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows)
{
  printPlayer(1, player_one_handcards, player_one_chosencards, player_one_cardrows);
  if (actionChoosingLoop(1, player_one_handcards,
                         player_one_chosencards, player_one_cardrows) == 1)
  {
    return 1;
  }
  printf("\n");
  printPlayer(2, player_two_handcards, player_two_chosencards, player_two_cardrows);
  if (actionChoosingLoop(2, player_two_handcards,
                         player_two_chosencards, player_two_cardrows) == 1)
  {
    return 1;
  }
  printf("\n");
  printf("Action phase is over - starting next game round!\n");
  printf("\n");
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function checks if the input for the action phase is correct. It checks if the row number and the card number
/// are valid. It returns true if the input is correct and false otherwise.
///
/// @param row_number The row number to check
/// @param card_number The card number to check
///
/// @return
///      true if the input is correct
///      false if the input is incorrect
//
int isActionInputCorrect(char *row_number, const char *card_number)
{
  if ((row_number == NULL || card_number == NULL) || strtok(NULL, " ") != NULL)
  {
    printf("Please enter the correct number of parameters!\n");
    return false;
  }
  else if (stringToInt(row_number) > 3 || stringToInt(row_number) < 1)
  {
    printf("Please enter a valid row number!\n");
    return false;
  }
  else
  {
    return true;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function performs the action choosing phase. It starts with the first player and lets him choose a card from
/// his chosen cards and add it to one of his card rows. Then it does the same for the second player. It returns 0 if
/// the action choosing phase could be performed successfully and 1 otherwise.
///
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return
///      0 if the action choosing phase could be performed successfully
///      1 if the action choosing phase could not be performed successfully
//
int actionChoosingLoop(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows)
{
  bool skip_prompt = false;
  do
  {
    if (!skip_prompt)
    {
      printf("What do you want to do?\n");
    }
    printf("P%i > ", player_id);
    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "quit") == 0)
    {
      free(input);
      return 1;
    }
    else if (strncmp(input, "place", 5) == 0 && strlen(input) > 5)
    {
     if (placeAction(input, &skip_prompt, player_id, player_handcards, player_chosencards, player_cardrows) == 1)
     {
       free(input);
       continue;
     }
    }
    else if (strncmp(input, "discard", 7) == 0 && strlen(input) > 7)
    {
      if (discardAction(input, &skip_prompt, player_id, player_handcards, player_chosencards, player_cardrows) == 1)
      {
        free(input);
        continue;
      }
    }
    else if (strcmp(input, "help") == 0)
    {
      helpAction(player_id, player_handcards, player_chosencards, player_cardrows);
    }
    else
    {
      printf("Please enter the correct number of parameters!\n");
      skip_prompt = true;
      free(input);
      continue;
    }
    skip_prompt = false;
    free(input);
  } while (*player_chosencards != NULL);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function prints the help message for the action phase.
///
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return void
//
void helpAction(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows)
{
  printf("\n"
         "Available commands:\n"
         "\n"
         "- help\n"
         "  Display this help message.\n"
         "\n"
         "- place <row number> <card number>\n"
         "  Append a card to the chosen row or if the chosen row does not exist create it.\n"
         "\n"
         "- discard <card number>\n"
         "  Discard a card from the chosen cards.\n"
         "\n"
         "- quit\n"
         "  Terminate the program.\n"
         "\n");
  printf("\n");
  printPlayer(player_id, player_handcards, player_chosencards, player_cardrows);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function performs the place action. It checks if the input is correct and if the card can be added to the
/// chosen row. It returns 0 if the place action could be performed successfully and 1 otherwise.
///
/// @param input The input string
/// @param skip_prompt A pointer to a boolean that indicates if the prompt should be skipped
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return
///      0 if the place action could be performed successfully
///      1 if the place action could not be performed successfully
//
int placeAction(char *input, bool *skip_prompt, int player_id, Card **player_handcards,
                Card **player_chosencards, Card ***player_cardrows)
{
  char *input_copy = strdup(input);
  if (input_copy == NULL) {
    printf("Error: Memory allocation failure\n");
    return 1;
  }
  char *row_number = strtok(input_copy + 5, " ");
  char *card_number = strtok(NULL, " ");
  if (isActionInputCorrect(row_number, card_number))
  {
    int card_number_int = stringToInt(card_number);
    Card *choosen_card = getCardFromChosen(*player_chosencards, card_number_int);
    if (choosen_card == NULL)
    {
      printf("Please enter the number of a card in your chosen cards!\n");
      *skip_prompt = true;
      free(input_copy);
      return 1;
    }
    else
    {
      removeCardFromChosen(player_chosencards, choosen_card);
      int row_number_int = stringToInt(row_number) - 1;
      int result = addCardToRow(*player_cardrows, choosen_card, row_number_int);
      if (result == 1)
      {
        printf("This card cannot extend the chosen row!\n");
        *skip_prompt = true;
        addCardToChosen(player_chosencards, choosen_card);
        free(input_copy);
        return 1;
      }
      printf("\n");
      printPlayer(player_id, player_handcards, player_chosencards, player_cardrows);
    }
  }
  else
  {
    free(input_copy);
    *skip_prompt = true;
    return 1;
  }
  free(input_copy);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function performs the discard action. It checks if the input is correct and if the card can be discarded. It
/// returns 0 if the discard action could be performed successfully and 1 otherwise.
///
/// @param input The input string
/// @param skip_prompt A pointer to a boolean that indicates if the prompt should be skipped
/// @param player_id The number of the player
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return
///      0 if the discard action could be performed successfully
///      1 if the discard action could not be performed successfully
//
int discardAction(char *input, bool *skip_prompt, int player_id, Card **player_handcards,
                  Card **player_chosencards, Card ***player_cardrows)
{
  char *card_number = strtok(input + 7, " ");
  if (card_number == NULL || strtok(NULL, " ") != NULL)
  {
    printf("Please enter the correct number of parameters!\n");
    *skip_prompt = true;
    return 1;
  }
  Card *choosen_card = getCardFromChosen(*player_chosencards, stringToInt(card_number));
  if (choosen_card == NULL)
  {
    printf("Please enter the number of a card in your chosen cards!\n");
    *skip_prompt = true;
    return 1;
  }
  else
  {
    removeCardFromChosen(player_chosencards, choosen_card);
    free(choosen_card);
    printf("\n");
    printPlayer(player_id, player_handcards, player_chosencards, player_cardrows);
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function performs the game end phase. It calculates the points of the given player and returns the total.
///
/// @param player_cardrows The card row of the player
///
/// @return
///      The total points of the player
//
int calculatePlayerPoints(Card ***player_cardrows)
{
  int points = 0;
  int longest_row_length = 0;
  for (int i = 0; i < MAX_CARD_ROWS; i++)
  {
    int row_length = 0;
    Card *head = (*player_cardrows)[i];
    while (head != NULL)
    {
      row_length++;
      head = head->next_;
    }
    if (row_length > longest_row_length)
    {
      longest_row_length = row_length;
    }
  }
  for (int i = 0; i < MAX_CARD_ROWS; i++)
  {
    int row_length = 0;
    Card *head = (*player_cardrows)[i];
    singleRowPointsCount(head, &points, &row_length);
    if (row_length == longest_row_length)
    {
      points *= 2;
    }
  }
  return points;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function counts the points of a single row of cards. It counts the points of the cards and adds them to the
/// total points. It also counts the length of the row and adds it to the total length.
///
/// @param head The head of the row
/// @param points A pointer to the total points
/// @param row_length A pointer to the total length
///
/// @return void
//
void singleRowPointsCount(Card *head, int *points, int *row_length)
{
  while (head != NULL)
  {
    (*row_length)++;
    if (head->color_ == RED)
    {
      (*points) += 10;
    }
    else if (head->color_ == WHITE)
    {
      (*points) += 7;
    }
    else if (head->color_ == GREEN)
    {
      (*points) += 4;
    }
    else if (head->color_ == BLUE)
    {
      (*points) += 3;
    }
    head = head->next_;
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function frees the memory of a player. It frees the hand cards, the chosen cards and the card rows of the
/// player.
///
/// @param player_handcards The hand cards of the player
/// @param player_chosencards The chosen cards of the player
/// @param player_cardrows The card rows of the player
///
/// @return void
//
void freePlayer(Card **player_handcards, Card **player_chosencards, Card ***player_cardrows)
{
  freeLinkedList(*player_handcards);
  *player_handcards = NULL;

  freeLinkedList(*player_chosencards);
  *player_chosencards = NULL;

  for (int i = 0; i < MAX_CARD_ROWS; i++)
  {
    freeLinkedList((*player_cardrows)[i]);
    (*player_cardrows)[i] = NULL;
  }
  free(*player_cardrows);
  *player_cardrows = NULL;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function frees a linked list of cards. It frees the memory of each card in the list.
///
/// @param head The head of the list
///
/// @return void
//
void freeLinkedList(Card* head)
{
  Card* temporary_card;
  while (head != NULL)
  {
    temporary_card = head;
    head = head->next_;
    free(temporary_card);
    temporary_card = NULL;
  }
}
