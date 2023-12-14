# Assignment 3 - SyntaxSakura

## Introduction

In Assignment 3 (A3) of the ESP practicals (KU), you will implement a card game for two
players, which is loosely inspired by [Ohanami](https://de.wikipedia.org/wiki/Ohanami).
Please take care to follow the assignment description exactly, even if you know the original game, because this version 
uses different rules than the original and even slight differences in your implementation can cause 
testcases to fail.

This document describes the structure of the assignment and the expected functionality of the program.

## Learning  Goals

- Heap and Memory Management
- File I/O
- Structs
- Enums
- Linked Lists

## Specifications

Some general specifications must be followed for every assignment in this course. A list of these specifications can be found here:
[Assessment of Assignments - Deductions](https://www.notion.so/coding-tugraz/Assessment-of-Assignments-How-do-you-get-your-points-d7cca6cc89a344e38ad74dd3ccb73cb3?pvs=4#e3203e50b1fc4ba787bdbf5fb46262ae). Please follow these specifications to avoid point deductions. As memory management is a central learning goal of this assignment, pay close attention to this section within the specifications.

Additionally, the following specifications must be followed for this assignment (A3) as well:
* **Attention:** Each card in the game has to be represented as a **struct**. These structs should hold at least two values, the unique number associated with each card and the color of the card. Feel free to extend the structs of the cards with different attributes that you deem necessary for your implementation. If structs are not used for the cards, this will result in a **deduction of up to -25 %**.
* **Attention:** All cards must be stored **on the heap**. If this is not the case, this will result in a **deduction of up to -25 %**.
* **Attention:** All rows of cards in the game, including hand cards, chosen cards and rows of cards that are placed down by the player, have to be represented internally as a **linked list** data structure. If this is not the case, this will result in a **deduction of up to -25 %**.
* **Attention:** As the user input can be of arbitrary length, it must be read in dynamically using **realloc**. If this is not the case, this will result in a **deduction of up to -25 %**. 

## Grading

Assignment 3 is worth **40 points**.

**Attention:** To receive a positive grade on the ESP KU **at least 6 of the public test cases have to be passed** (among other conditions).  

Information on how your assignment submission will be assessed can be found here: [Assessment of Assignments](https://www.notion.so/coding-tugraz/Assessment-of-Assignments-How-do-you-get-your-points-d7cca6cc89a344e38ad74dd3ccb73cb3).
Please read this carefully as it also contains guidelines on what your final code should look like (Coding Standard, Style Guide).

Here you can find an overview of how your total grade for the KU will be calculated: [Procedure and Grading](https://www.notion.so/coding-tugraz/Procedure-and-Grading-59975fee4a9c4047867772cd20caf73d)

## Submission
- Deliverables: `a3.c`
- Push to your repository
- **Deadline: 17.01.2024, 23:59 (GMT+1)**

## Assignment Description

In this assignment you will implement a card game for two players which is loosely inspired by [Ohanami](https://de.wikipedia.org/wiki/Ohanami). The interface will run on the Command Line.

First you will have to parse a config file and set up the game. Then the players will each get 10 hand cards and the first game round starts. Each round of the game consists of two phases - the card choosing phase and the action phase.

In the card choosing phase, each player chooses two cards from their hand cards. Then the remaining hand cards are handed
over to the other player and the action phase starts.

In the action phase, each player places down their previously chosen two cards to create or extend their own rows of cards.
Each player can create at most three rows of cards. The rows are always sorted in ascending order and can only be extended at the beginning or the end. Players can also discard previously chosen cards that they do not want to/cannot place.

The game ends when all cards have been placed or discarded. At the end of the game the players receive points based on the cards in their own rows. The player with the most points wins the game.

### General Notes
The notes given in this section are important for the whole assignment description.

- `\n` shouldn't be printed as separate characters, instead it represents the newline character.
- Note the leading and trailing whitespaces in the text fields! You can make them visible by selecting the text field.
- Words in sharp brackets (like `<CARD_NUMBER>`) should be replaced by calculated or given data. They are *not* part of the output itself! For a better understanding you can look at the example outputs at the end of this document.
- The user input is **case-insensitive**. This means that `HELP`, `help` and `hElP` would all be valid inputs.
- `P<NR> > ` (command prompt, always printed with a leading player identifier and a trailing space) in the output signifies that the program should wait for a user input at this point. Execution should only continue after the user has ended their input with `ENTER`.
- If the user's input is invalid, the corresponding error message should be printed. After this **only the command prompt (including the player identifier)** is printed again and the user can make a new input. This process is repeated as long as the input is invalid. Make sure that your program can handle inputs of the wrong type, for example string input when a number is expected. Also make sure that your program can handle inputs of arbitrary length including empty inputs.
- All additional whitespaces (leading, trailing, or additional whitespaces between command and parameters or between two parameters) in the input should be ignored. 
- Take care with your memory management! That means that when allocating memory on the heap, you always need to check that the system was able to provide that memory. If the program runs out of memory you should first free all memory previously allocated on the heap, then print the correct error message and terminate the program with the corresponding return value.

## Program Sequence


<details>
<summary><h4>Starting the Game</h4></summary>

The program is called with one command line parameter. This parameter is the path to a
config file that contains the number of players and the game cards, and should be loaded at start.

For example:

```
./a3 configs/config_01.txt
```

If the program is called
- with the wrong number of parameters
- with a config file that cannot be opened for reading
- with a config file that does not start with the correct [magic number](https://en.wikipedia.org/wiki/Magic_number_(programming))

the corresponding error message should be printed and the program should terminate with the correct return
value (see Return Values and Error Messages).

<details>
<summary><h5>Parsing the Config File</h5></summary>

Every config file contains a [magic number](https://de.wikipedia.org/wiki/Magische_Zahl_(Informatik)) in the first line
(`ESP\n`). This string has to be checked for its correctness. If the magic number is correct you can assume that the file is correctly formatted and no further checks of the file's content are needed.

The beginning of a valid config file could for example look like this:

```txt
ESP\n
2\n
57_g\n
37_r\n
28_w\n
29_r\n
89_r\n
44_b\n
14_w\n
115_r\n
<...>
```

- The first line specifies the magic number which has to be `ESP` in ASCII-text.  
- The second line specifies the number of players, for this assignment this number is always `2`.  
- After that the cards for the game are listed one card per line.  
- Keep in mind the newline character `'\n'` at the end of each line.

Every card consists of an integer number equal or greater than `1` and equal or lesser than `120` and one of 
four colors (blue `b`, green `g`, white `w`, red `r`). You can find an example for a valid config file [here](./configs/config_01.txt).

To represent the attributes of the individual cards in a structured way you have to use a **struct** to store the
*numerical value* and the *color* of the cards. Feel free to extend this struct with any member variables that you think you need in your program. Further, all cards are unique. There are in total 120 different cards that can show up in the config files with every card having a different numerical value.

</details>

<details>
<summary><h5>Welcome Message</h5></summary>

At the beginning of the game the following welcome message is printed:

```
Welcome to SyntaxSakura (2 players are playing)!\n
\n
```

</details>

<details>
<summary><h5>Distributing the Cards</h5></summary>

Next the cards are distributed in the order that they appear in the config file to the players until every player has ten cards. A config file with the correct magic number will always contain at least enough cards for this distribution.

This would look as follows: `Player 1` gets the first card from the config file, then `Player 2` gets the second card. 
Now every player has one card and the second distribution round starts, that means again `Player 1`
receives the third card, then `Player 2` the fourth. Then the third distribution round starts...

The cards of each player have to be stored as a **linked list**. Furthermore, we recommend that each player's cards are sorted in
ascending order by their numerical value because this is necessary for the output. 

If there are more cards listed in the config file than are needed for this distribution, the remaining
cards are not relevant for the game and can be ignored.

After the cards have been distributed the first game round starts.

</details>

</details>

<details>
<summary><h4>Running the Game</h4></summary>

One game round consists of two different phases, the card choosing phase and the action phase.

- First each player, starting with `Player 1`, will choose which two cards they want to place.  
- After both players have chosen their cards, the action phase starts with `Player 1`.  
- After both players are finished with their action phase the next card choosing phase begins, starting again
with `Player 1`.

If the players have used up all their hand cards the game ends (see Finishing the Game > Scoring).

<details>
<summary><h5>Command: Quit</h5></summary>

- `quit`

From the start of the first card choosing phase it should be possible to terminate the program with the command `quit` whenever the program is waiting for user input.

The `quit` command has no parameters. If `quit` is called with any parameters, the correct error message (see Return Values and Error Messages) should be printed, the command prompt should be printed again and the program should wait for a new user input.

If a valid `quit` command is entered, the program should terminate with the  return value `0` (see Return Values and Error Messages). Make sure to free all previously allocated memory before termination.

</details>

<details>
<summary><h5>Player Status Info</h5></summary>

At certain points during the game round (see the following sections for these points) the current player's status information should be printed. It includes their hand cards (in ascending order; might be empty), their chosen cards (in ascending order; might be empty) as well as all non-empty rows of that player.

A card will always be printed to the console in the following format: `<CARD_NUMBER>_<CARD_COLOR>`.
The card color is always printed as a lower case letter (for example `67_r`).
In all lists two cards are always separated by a single space.

The player status information looks like this:

```
\n
Player <NR>:\n
  hand cards: <LIST_OF_HAND_CARDS>\n
  chosen cards: <LIST_OF_CHOSEN_CARDS>\n
  row_1: <LIST_OF_CARDS_IN_ROW_1>\n
  row_2: <LIST_OF_CARDS_IN_ROW_2>\n
  row_3: <LIST_OF_CARDS_IN_ROW_3>\n
\n
```

If the hand cards or the chosen cards are empty, the `\n` directly follows the `:`. 
If a row is empty it should not be displayed. 

For example, if `Player 1` has no rows yet, their player status info might look like this:

```
\n
Player 1:\n
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 67_r 81_g 89_r 119_w\n
  chosen cards:\n
\n
```

If `Player 1` has already chosen two cards and then placed one of them in their row 2, their status info might look like this:

```
\n
Player 1:\n
  hand cards: 38_b 48_g 56_w 57_g 67_r 81_g 89_r 119_w\n
  chosen cards: 28_w\n
  row_2: 14_w\n
\n
```

</details>

<details>
<summary><h5>Card Choosing Phase</h5></summary>

First print the following header:
```
-------------------\n
CARD CHOOSING PHASE\n
-------------------\n
```

Then the players take turns choosing exactly two cards from their hand cards which they want to keep.

At the beginning of each player's turn in the card choosing phase the player's status information should be printed. 
In this case the chosen cards are empty as no cards have been chosen yet.

Next the player is asked to choose two cards from their hand cards. First, the following question is asked: 

```
Please choose a first card to keep:\n
P<NR> > 
```

> **Note:** `<NR>` should from now on always be replaced with the index of the current player (e.g. `1` for `Player 1`).

A valid input is a card number from a card the player has in their **hand cards** or the `quit` command.

If the user input is invalid, the following error message should be printed:

```
Please enter the number of a card in your hand cards!\n
```

After the player has choosen a valid first card, the following question is asked:

```
Please choose a second card to keep:\n
P<NR> > 
```
A valid input is once again a card number from a card the player has in their **hand cards** or the `quit` command. The card number that was chosen for the first card is not valid, because it is not part of the hand cards anymore. If the user input is invalid, the error message is the same as for the first question.

The following example shows `Player 1` choosing their two cards:

```
\n
Player 1:\n
  hand cards: 14_w 48_g 67_r 81_g 89_r 119_w\n
  chosen cards:\n
  row_1: 56_w\n
  row_2: 57_g 60_g 61_r\n
\n
Please choose a first card to keep:\n
P1 > 1\n
Please enter the number of a card in your hand cards!\n
P1 > 67\n
Please choose a second card to keep:\n
P1 > 48\n
```

After the first player has chosen two cards, the second player's turn in the card choosing phase starts. The card choosing phase ends after both players have chosen two cards each.

Afterwards each player passes the remaining cards in their hand to the other player. This means that `Player 1` passes their remaining hand cards to `Player 2` and `Player 2` passes their remaining hand cards to `Player 1`.

The program should print the following:
```
\n
Card choosing phase is over - passing remaining hand cards to the next player!\n
\n
```

Then the action phase starts.

</details>

<details>
<summary><h5>Action Phase</h5></summary>

First print the following header:
```
------------\n
ACTION PHASE\n
------------\n
```

In the action phase the players place their previously chosen cards to create or extend their own rows.

Each player can create a maximum of three rows with the two cards that they previously chose from their hand cards. Cards can be placed to create a new row if the maximum has not yet been reached or to extend already existing rows.

Rows can only be extended at the beginning or at the end. Cards cannot be inserted in the middle of a row. If the
numerical value of the card is lower than the first card in the row, it will be placed at the beginning of the row. 
If the numerical value of the card is higher than the last card in the row, it will be placed at the end of the row.
If a card cannot be placed or the player does not want to place the card they can discard it.

At the beginning of each player's turn in the action phase the player's status information should be printed. The section `hand cards` now contains the new cards that were passed to the player by the other player after the card choosing phase. The section `chosen cards` contains the two cards that were chosen by the player in the previous card choosing phase.

Next, the player is asked to input a command with the following question:

```
What do you want to do?\n
P<NR> > 
```
After that the user can input commands.

For example:
```
\n
Player 1:\n
  hand cards: 5_r 29_r 37_r 44_b 60_g 61_r 110_b 115_r\n
  chosen cards: 48_g 67_r\n
  row_1: 56_w\n
  row_2: 57_g 60_g 61_r\n
\n
What do you want to do?\n
P1 > 
```

A player can choose between four actions/commands during their action phase.
- `help`
- `place <ROW> <CARD_NUMBER>`
- `discard <CARD_NUMBER>`
- `quit`

> **Hint:** You can use [strtok](https://en.cppreference.com/w/cpp/string/byte/strtok) to split a string into parts.

If the player inputs an unrecognized command or there are too many or not enough parameters for the command it should not be executed. Instead, the correct error message (see Return Values and Error Messages) should be printed, the command prompt should be printed again and the player should be able to input another command.

The player can enter commands as long as they have not yet played all chosen cards. After every successful `help`, `place` or `discard` action the (updated) status information of the player should be printed again. If the player still has chosen cards, the program should then print the question (`What do you want to do?\n`) and the command prompt again and wait for a new input.

If the first player has no chosen cards left, the second player starts their turn in the action phase.

After both players have completed their turn in the action phase, the program should print the following:
```
\n
Action phase is over - starting next game round!\n
\n
```

If the players still have hand cards, now the next game round starts once again with the card choosing phase. If there are no hand cards left, the program should continue with the scoring (see Finishing the Game > Scoring).

##### Command: Help

The command `help` prints the following help text:

```
\n
Available commands:\n
\n
- help\n
  Display this help message.\n
\n
- place <row number> <card number>\n
  Append a card to the chosen row or if the chosen row does not exist create it.\n
\n
- discard <card number>\n
  Discard a card from the chosen cards.\n
\n
- quit\n
  Terminate the program.\n
\n
```

##### Command: Place

- `place <ROW> <CARD_NUMBER>`

This command is used to extend the chosen row with a chosen card or, if the row does not yet exist, to create a new row with the chosen card.   

If the row cannot be extended with the chosen card, the row number is not valid or the player's **chosen cards** do not 
contain the specified card print the correct error message (see Return Values and Error Messages),
do not modify the current state, print the command prompt again and wait for another input.

##### Command: Discard

- `discard <CARD_NUMBER>`

This command is used to discard a chosen card, thus not creating or extending a row.

The chosen card will be removed from the chosen cards of the player. It is not needed for the rest of the game 
and can be deleted.

If the entered card number is not part of the player's **chosen cards** print the 
correct error message (see Return Values and Error Messages), do not modify the current state, print the command prompt again and wait for another input.

##### Command: Quit

The command `quit` works the same way in the action phase as it did in the card choosing 
phase (see Running the Game > Command: Quit).

</details>

</details>

<details>
<summary><h4>Finishing the Game</h4></summary>

<details>
<summary><h5>Scoring</h5></summary>

After all cards have been placed each player receives points for each card contained in their rows.
The cards give a different amount of points based on the color of the card.

| Color  | Blue | Green | White | Red  |
|:------:|:----:|:-----:|:-----:|:----:|
| Points |  3   |   4   |   7   |  10  |

The points gained from the longest row of each player are multiplied by two. If a player has multiple "longest" rows at the end of the game the points of the row with the lowest row number (lowest index) amongst the "longest" rows get multiplied by two.

For example:
```
row_1: 48_g 56_w         =>  4 + 7              = 11                         
row_2: 57_g 60_g 61_r    =>  (4 + 4 + 10) * 2   = 36   
row_3: 115_r             =>  10                 = 10                      
```
This player would receive `11 + 36 + 10 = 57` points in total.

The player with the most points wins the game. The program should print a list of both players sorted by their
points in descending order. Then the program should congratulate the winner.

```
\n
Player <NR>: <POINTS> points\n
Player <NR>: <POINTS> points\n
\n
Congratulations! Player <NR> wins the game!\n
```

For example:
```
\n
Player 1: 12 points\n
Player 2: 6 points\n
\n
Congratulations! Player 1 wins the game!\n
```

If both players have the same amount of points they should additionally be sorted in ascending order by player index. In this case both players should be congratulated in ascending order by player index.

```
\n
Player 1: 12 points\n
Player 2: 12 points\n
\n
Congratulations! Player 1 wins the game!\n
Congratulations! Player 2 wins the game!\n
```

</details>

<details>
<summary><h5>Documenting the Results</h5></summary>

Before you terminate the program you should also append the final score that the players received to the end of the loaded config file without overwriting any of the previous content. The score should be written to the file in the same format as you printed it to the command line. There should be one empty line between the last card in the config file and the scores. The file should end after the final congratulations message.

For example, the end of a config file after documenting the results could look like this:

```txt
<...>
33_g\n
38_b\n
30_g\n
\n
Player 1: 12 points\n
Player 2: 3 points\n
\n
Congratulations! Player 1 wins the game!\n
```

If the file cannot be opened for writing you should print the following error message to the console:

```
Warning: Results not written to file!\n
```

Afterwards the program terminates with return value `0`.

> **Hint:** Executing `make reset` will reset all config files to their original content. Executing `make run` or `make test` also automatically includes this reset.

</details>

</details>

<details>
<summary><h4>Return Values and Error Messages</h4></summary>

The program has to react to errors during the game and print the corresponding error messages and/or return the correct
return values. You can find all error messages that should be printed and all return values in the table below.

Only one error should be printed at once. If multiple errors occur at the same time, 
only the highest message in the below table should be printed.

`<CONFIG_FILE>` should be replaced with the command line parameter that was entered by the user
calling the program in the error messages.

| Return Value | Error Message                                      | Meaning                                                            | Program Phase|
|--------------|----------------------------------------------------|--------------------------------------------------------------------|-------------|
| 0            | -                                                  | The program terminated successfully                                | - |
| 1            | `Usage: ./a3 <config file>\n`                      | The wrong number of command line parameters was entered            | Starting the Game |
| 2            | `Error: Cannot open file: <CONFIG_FILE>\n`         | The config file could not be opened for reading                               | Starting the Game |
| 3            | `Error: Invalid file: <CONFIG_FILE>\n`             | The config file contains the wrong *magic number*                  | Starting the Game |
| 4            | `Error: Out of memory\n`                           | The program was not able to allocate new memory                    | - |
|              | `Please enter the number of a card in your hand cards!\n`     | The player's hand cards do not contain this card, or the input is not a number and not a `quit` command  | Card Choosing Phase |
|              | `Please enter a valid command!\n`                  | The command does not exist                                         | Action Phase |
|              | `Please enter the correct number of parameters!\n` | The command was called with too many or too few parameters    | Card Choosing Phase, Action Phase |
|              | `Please enter a valid row number!\n`               | The entered row number is below 1 or above 3, or the `<ROW>` parameter is not a number                      | Action Phase |
|              | `Please enter the number of a card in your chosen cards!\n`     | The player's chosen cards do not contain this card, or the `<CARD_NUMBER>` parameter is not a number   | Action Phase |
|              | `This card cannot extend the chosen row!\n`        | The chosen card cannot extend the chosen row                       | Action Phase |
|              | `Warning: Results not written to file!\n`         | The results could not be appended to the config file               | Documenting the Results |
</details>


## Examples

The following example output was created using [config file 1](./configs/config_01.txt).

<details>
<summary><h4>Example Output</h4></summary>

```
Welcome to SyntaxSakura (2 players are playing)!

-------------------
CARD CHOOSING PHASE
-------------------

Player 1:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 67_r 81_g 89_r 119_w
  chosen cards:

Please choose a first card to keep:
P1 > 89
Please choose a second card to keep:
P1 > 67

Player 2:
  hand cards: 5_r 29_r 30_g 33_g 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards:

Please choose a first card to keep:
P2 > 29
Please choose a second card to keep:
P2 > 5

Card choosing phase is over - passing remaining hand cards to the next player!

------------
ACTION PHASE
------------

Player 1:
  hand cards: 30_g 33_g 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards: 67_r 89_r

What do you want to do?
P1 > place 1 89

Player 1:
  hand cards: 30_g 33_g 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards: 67_r
  row_1: 89_r

What do you want to do?
P1 > place 1 67

Player 1:
  hand cards: 30_g 33_g 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards:
  row_1: 67_r 89_r


Player 2:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards: 5_r 29_r

What do you want to do?
P2 > help

Available commands:

- help
  Display this help message.

- place <row number> <card number>
  Append a card to the chosen row or if the chosen row does not exist create it.

- discard <card number>
  Discard a card from the chosen cards.

- quit
  Terminate the program.


Player 2:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards: 5_r 29_r

What do you want to do?
P2 > discard 5

Player 2:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards: 29_r

What do you want to do?
P2 > place 2 29

Player 2:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards:
  row_2: 29_r


Action phase is over - starting next game round!

-------------------
CARD CHOOSING PHASE
-------------------

Player 1:
  hand cards: 30_g 33_g 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards:
  row_1: 67_r 89_r

Please choose a first card to keep:
P1 > 30
Please choose a second card to keep:
P1 > 33

Player 2:
  hand cards: 14_w 28_w 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards:
  row_2: 29_r

Please choose a first card to keep:
P2 > 14
Please choose a second card to keep:
P2 > 28

Card choosing phase is over - passing remaining hand cards to the next player!

------------
ACTION PHASE
------------

Player 1:
  hand cards: 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards: 30_g 33_g
  row_1: 67_r 89_r

What do you want to do?
P1 > place 1 33

Player 1:
  hand cards: 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards: 30_g
  row_1: 33_g 67_r 89_r

What do you want to do?
P1 > place 1 30

Player 1:
  hand cards: 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r


Player 2:
  hand cards: 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards: 14_w 28_w
  row_2: 29_r

What do you want to do?
P2 > place 1 28

Player 2:
  hand cards: 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards: 14_w
  row_1: 28_w
  row_2: 29_r

What do you want to do?
P2 > place 1 14

Player 2:
  hand cards: 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards:
  row_1: 14_w 28_w
  row_2: 29_r


Action phase is over - starting next game round!

-------------------
CARD CHOOSING PHASE
-------------------

Player 1:
  hand cards: 38_b 48_g 56_w 57_g 81_g 119_w
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r

Please choose a first card to keep:
P1 > 38
Please choose a second card to keep:
P1 > 57

Player 2:
  hand cards: 37_r 44_b 60_g 61_r 110_b 115_r
  chosen cards:
  row_1: 14_w 28_w
  row_2: 29_r

Please choose a first card to keep:
P2 > 44
Please choose a second card to keep:
P2 > 110

Card choosing phase is over - passing remaining hand cards to the next player!

------------
ACTION PHASE
------------

Player 1:
  hand cards: 37_r 60_g 61_r 115_r
  chosen cards: 38_b 57_g
  row_1: 30_g 33_g 67_r 89_r

What do you want to do?
P1 > place 2 38

Player 1:
  hand cards: 37_r 60_g 61_r 115_r
  chosen cards: 57_g
  row_1: 30_g 33_g 67_r 89_r
  row_2: 38_b

What do you want to do?
P1 > place 2 57

Player 1:
  hand cards: 37_r 60_g 61_r 115_r
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r
  row_2: 38_b 57_g


Player 2:
  hand cards: 48_g 56_w 81_g 119_w
  chosen cards: 44_b 110_b
  row_1: 14_w 28_w
  row_2: 29_r

What do you want to do?
P2 > discard 45
Please enter the number of a card in your chosen cards!
P2 > discard 44

Player 2:
  hand cards: 48_g 56_w 81_g 119_w
  chosen cards: 110_b
  row_1: 14_w 28_w
  row_2: 29_r

What do you want to do?
P2 > place 1 110

Player 2:
  hand cards: 48_g 56_w 81_g 119_w
  chosen cards:
  row_1: 14_w 28_w 110_b
  row_2: 29_r


Action phase is over - starting next game round!

-------------------
CARD CHOOSING PHASE
-------------------

Player 1:
  hand cards: 37_r 60_g 61_r 115_r
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r
  row_2: 38_b 57_g

Please choose a first card to keep:
P1 > 61
Please choose a second card to keep:
P1 > 115

Player 2:
  hand cards: 48_g 56_w 81_g 119_w
  chosen cards:
  row_1: 14_w 28_w 110_b
  row_2: 29_r

Please choose a first card to keep:
P2 > place 1 15
Please enter the number of a card in your hand cards!
P2 > 48
Please choose a second card to keep:
P2 > 81

Card choosing phase is over - passing remaining hand cards to the next player!

------------
ACTION PHASE
------------

Player 1:
  hand cards: 56_w 119_w
  chosen cards: 61_r 115_r
  row_1: 30_g 33_g 67_r 89_r
  row_2: 38_b 57_g

What do you want to do?
P1 > place 1 115

Player 1:
  hand cards: 56_w 119_w
  chosen cards: 61_r
  row_1: 30_g 33_g 67_r 89_r 115_r
  row_2: 38_b 57_g

What do you want to do?
P1 > place 2 61

Player 1:
  hand cards: 56_w 119_w
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r 115_r
  row_2: 38_b 57_g 61_r


Player 2:
  hand cards: 37_r 60_g
  chosen cards: 48_g 81_g
  row_1: 14_w 28_w 110_b
  row_2: 29_r

What do you want to do?
P2 > place 1 48
This card cannot extend the chosen row!
P2 > place 2 48

Player 2:
  hand cards: 37_r 60_g
  chosen cards: 81_g
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g

What do you want to do?
P2 > place 2 81

Player 2:
  hand cards: 37_r 60_g
  chosen cards:
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g 81_g


Action phase is over - starting next game round!

-------------------
CARD CHOOSING PHASE
-------------------

Player 1:
  hand cards: 56_w 119_w
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r 115_r
  row_2: 38_b 57_g 61_r

Please choose a first card to keep:
P1 > 56
Please choose a second card to keep:
P1 > 119

Player 2:
  hand cards: 37_r 60_g
  chosen cards:
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g 81_g

Please choose a first card to keep:
P2 > 37
Please choose a second card to keep:
P2 > 60

Card choosing phase is over - passing remaining hand cards to the next player!

------------
ACTION PHASE
------------

Player 1:
  hand cards:
  chosen cards: 56_w 119_w
  row_1: 30_g 33_g 67_r 89_r 115_r
  row_2: 38_b 57_g 61_r

What do you want to do?
P1 > place 1 119

Player 1:
  hand cards:
  chosen cards: 56_w
  row_1: 30_g 33_g 67_r 89_r 115_r 119_w
  row_2: 38_b 57_g 61_r

What do you want to do?
P1 > place 3 56 

Player 1:
  hand cards:
  chosen cards:
  row_1: 30_g 33_g 67_r 89_r 115_r 119_w
  row_2: 38_b 57_g 61_r
  row_3: 56_w


Player 2:
  hand cards:
  chosen cards: 37_r 60_g
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g 81_g

What do you want to do?
P2 > place 3 37

Player 2:
  hand cards:
  chosen cards: 60_g
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g 81_g
  row_3: 37_r

What do you want to do?
P2 > discard 60

Player 2:
  hand cards:
  chosen cards:
  row_1: 14_w 28_w 110_b
  row_2: 29_r 48_g 81_g
  row_3: 37_r


Action phase is over - starting next game round!


Player 1: 114 points
Player 2: 62 points

Congratulations! Player 1 wins the game!
```

</details>

After executing the program the config file should look like this:

<details>
<summary><h4>Example File after Documenting the Results</h4></summary>

```
ESP
2
57_g
37_r
28_w
29_r
89_r
44_b
14_w
115_r
119_w
60_g
67_r
61_r
48_g
5_r
81_g
110_b
56_w
33_g
38_b
30_g

Player 1: 114 points
Player 2: 62 points

Congratulations! Player 1 wins the game!
```

</details>