//
//  main.c
//
//  Created by Tomas Gonzalez on 7/17/18.
//  Copyright © 2018 Tomas Gonzalez. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>     //unable to use "syscalls.h" so used this as a substitute
#include <fcntl.h>

#define SIZE 2

char *getTurn(int *);
void switchTurn(int *);
void show(void);
void movePiece(int *, char, int, char, int, int);
void capturePiece(int *, char, int, char, int, int);
int isClear(int, int, int, int *, int);
int get(int, long, char *, int);
int calculate(char, int);
void update(int, int, int);

enum pieces {pawn = 1, knight, bishop, rook, queen, king};
enum color {WHITE = 1, BLACK = -1};


int main()
{
    int turn = 1;    //1 = white, -1 = black, game starts with white going first as default
    
    int *t = &turn;

    char c1;
    int c2;
    char d1;
    int d2;
    char command[20];
    
    int file = open("board.csv", O_RDWR);

    while(1)
    {
        printf("Enter command:\t[%s's turn]\n", getTurn(t));
        fgets(command, 20, stdin);
        if(command[0] == 'm' && command[1] == 'v')
        {
            if(sscanf(command, "mv %c%d %c%d", &c1, &c2, &d1, &d2) == 4)
            {
                if((c1 >= 'a' && 'h' >= c1) && (d1 >= 'a' && 'h' >= d1) && (c2 >= 1 && 8 >= c2) && (d2 >= 1 && 8 >= d2))
                    movePiece(t, c1, c2, d1, d2, file);
                else
                    printf("ERROR: Please enter a valid command\n");
            }
            else
                printf("ERROR: Please enter a valid command\n");
        }
        else if(command[0] == 'c' && command[1] == 'p')
        {
            if(sscanf(command, "cp %c%d %c%d", &c1, &c2, &d1, &d2))
            {
                if((c1 >= 'a' && 'h' >= c1) && (d1 >= 'a' && 'h' >= d1) && (c2 >= 1 && 8 >= c2) && (d2 >= 1 && 8 >= d2))
                       capturePiece(t, c1, c2, d1, d2, file);
                else
                    printf("ERROR: Please enter a valid command\n");
            }
            else
                printf("ERROR: Please enter a valid command\n");
        }
        else
        {
            if(!strcmp(command, "exit\n"))
            {
                printf("Thanks for playing!\n");
                exit(0);
            }
            else if(!strcmp(command, "show\n"))
            {
                show();
            }
            else if(!strcmp(command, "skip\n"))
            {
                switchTurn(t);
            }
            else
                printf("ERROR: Please enter a valid command\n");
        }
    }
    
    return 0;
}

/* Prints the full contents of board.csv in .tl5 format.   */
void show()
{
    FILE *board;
    board = fopen("board.csv", "r");
    int rows = 8;
    
    if(board == NULL)
    {
        printf("ERROR: board.csv appears to be empty.\n");
        exit(1);
    }
    else
    {
        printf("\t %-5.5s %-5.5s %-5.5s %-5.5s %-5.5s %-5.5s %-5.5s %-5.5s\n", "a", "b", "c", "d", "e", "f","g", "h");
        char line[100];
        while(fgets(line, 100, board) != NULL)
        {
            printf("%d\t|", rows--);
            char *word = strtok(line, " |.,\n");
            while(word != NULL)
            {
                printf("%-5.5s|", word);
                word = strtok(NULL, " |.,\n");
            }
            printf("\n");
        }
    }
    fclose(board);
}

/* Checks to see if the move is legal and then updates the board.   */
void movePiece(int *turn, char mine1, int mine2, char opp1, int opp2, int fp)
{
    int from = calculate(mine1, mine2);
    int to = calculate(opp1, opp2);
    char p[SIZE];
    char p2[SIZE];
    if(lseek(fp, from, 0) >= 0)
    {
        read(fp, p, SIZE);
    }
    if(lseek(fp, to, 0) >= 0)
    {
        read(fp, p2, SIZE);
    }
    int piece = p[1] - 48;  //piece in int to compare with enums
    if(p[1] == '0')         //if from-spot is empty
    {
        printf("ERROR: There is no piece here. Please enter another command.\n");
        return;
    }
    else if(p2[1] != '0')   //if to-spot is already occupied.
    {
        printf("ERROR: There is a piece already at %c%d. Cannot move there. Please enter another command.\n", opp1, opp2);
        return;
    }
    else if((*turn == WHITE && p[0] != ' ') || (*turn == BLACK && p[0] != '-')) //check ownership of move piece
    {
        printf("ERROR: You do not have control over this piece. Please enter another command.\n");
        return;
    }
    else if(!isClear(fp, from, to, turn, piece))        //check to make sure piece is not jumping over other pieces
    {
        printf("ERROR: This piece cannot move like that. Please enter another command.\n");
        return;
    }
    else
    {
        if(piece == pawn)
        {
            if(p[0] == '-' && (24 <= from && from <= 47))
            //black pawn - first move in second row can be 2 spaces
            {
                if((to == from+24) || (to == from+48))
                {
                    update(fp, from, to);
                }
                else
                {
                    printf("ERROR: A pawn cannot move there.\n");
                    return;
                }
            }
            else if(p[0] == ' ' && (144 <= from && from <= 167))
            //white pawn - first move in second row can be 2 spaces
            {
                if((to == from-24) || (to == from-48))
                {
                    update(fp, from, to);
                }
                else
                {
                    printf("ERROR: A pawn cannot move there.\n");
                    return;
                }
            }
            else
            {
                if((p[0] == '-' && (to-from == 24)) || (p[0] = ' ' && (from-to) == 24))
                {
                    update(fp, from, to);
                }
                else
                {
                    printf("ERROR: A pawn cannot move there.\n");
                    return;
                }
            }
        }
        else if(piece == knight)
        {
            if((to-from == 30) || (to-from == 51) || (to-from == 45) || (to-from == 18) || \
                                (to-from == -30) || (to-from == -51) || (to-from == -45) || (to-from == -18))
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A knight cannot move there.\n");
                return;
            }
        }
        else if(piece == bishop)
        {
            if((((to-from) % 21) == 0) || (((to-from) % 27) == 0))
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A bishop cannot move there.\n");
                return;
            }
        }
        else if(piece == rook)
        {
            if((((to-from) % 24) == 0) || (((to-from) % 3) == 0))
            {
                if(((((to-from) % 21) == 0) || (((to-from) % 27) == 0)) && ((to != 0) && (from != 0))) //stops movement if the rook is trying to move like a bishop
                {
                    printf("ERROR: A rook cannot move there.\n");
                    return;
                }
                else
                    update(fp, from, to);
            }
            else
            {
                printf("ERROR: A rook cannot move there.\n");
                return;
            }
        }
        else if(piece == queen)
        {
            if((((to-from) % 24) == 0) || (((to-from) % 3) == 0))   //can move as a bishop or a rook
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A queen cannot move there.\n");
                return;
            }

        }
        else if(piece == king)
        {
            if((to-from == 24) || (to-from == -24) || (to-from == 3) || (to-from == -3) || \
               (to-from == 21) || (to-from == -21) || (to-from == 27) || (to-from == -27))
            {
                update(fp, from, to);
            }
            else
            {
                printf("A king cannot move there.\n");
                return;
            }
        }
        else
            printf("ERROR WITH PIECE\n");
    }
    printf("Moved piece at %c%d to %c%d.\n", mine1, mine2, opp1, opp2);
    switchTurn(turn);
}

/* Checks to see if the capture is legal and then updates the board.   */
void capturePiece(int *turn, char mine1, int mine2, char opp1, int opp2, int fp)
{
    int from = calculate(mine1, mine2);
    int to = calculate(opp1, opp2);
    char p[SIZE];
    char p2[SIZE];
    if(lseek(fp, from, 0) >= 0)
    {
        read(fp, p, SIZE);
    }
    if(lseek(fp, to, 0) >= 0)
    {
        read(fp, p2, SIZE);
    }
    int piece = p[1] - 48;
    if(p[1] == '0')     //if from-spot is empty
    {
        printf("ERROR: There is no piece here. Please enter another command.\n");
        return;
    }
    else if(p2[1] == '0')   //if to-spot is already occupied.
    {
        printf("ERROR: There is no piece at %c%d. Cannot capture there. Please enter another command.\n", opp1, opp2);
        return;
    }
    else if((*turn == WHITE && p[0] != ' ') || (*turn == BLACK && p[0] != '-'))     //checks ownership of moving piece.
    {
        printf("ERROR: You do not have control over this piece. Please enter another command.\n");
        return;
    }
    else if((*turn == WHITE && p2[0] == ' ') || (*turn == BLACK && p2[0] == '-'))   //checks ownership of piece about to be captured.
    {
        printf("ERROR: You are trying to capture your own piece. This is not allowed. Please enter another command.\n");
        return;
    }
    else if(!isClear(fp, from, to, turn, piece))
    {
        printf("ERROR: This piece cannot jump over other pieces. Please enter another command.\n");
        return;
    }
    else
    {
        if(piece == pawn)
        {
            if(p[0] == '-' && ((to == from+27) || (to == from+21)))
            {
                update(fp, from, to);
            }
            else if(p[0] == ' ' && ((to == from-27) || (to == from-21)))
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A pawn cannot capture there.\n");
                return;
            }
        }
        else if(piece == knight)
        {
            if((to-from == 30) || (to-from == 51) || (to-from == 45) || (to-from == 18) || \
               (to-from == -30) || (to-from == -51) || (to-from == -45) || (to-from == -18))
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A knight cannot capture there.\n");
                return;
            }
        }
        else if(piece == bishop)
        {
            if((((to-from) % 21) == 0) || ((to-from) % 27) == 0)
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A bishop cannot move there.\n");
                return;
            }
        }
        else if(piece == rook)
        {
            if((((to-from) % 24) == 0) || ((to-from) % 3) == 0)
            {
                if(((((to-from) % 21) == 0) || (((to-from) % 27) == 0)) && ((to != 0) && (from != 0)))
                //stops movement if the rook is trying to move like a bishop
                {
                    printf("ERROR: A rook cannot move there.\n");
                    return;
                }
                else
                    update(fp, from, to);
            }
            else
            {
                printf("ERROR: A rook cannot move there.\n");
                return;
            }
        }
        else if(piece == queen)
        {
            if((((to-from) % 24) == 0) || (((to-from) % 3) == 0))   //can move as a bishop or a rook
            {
                update(fp, from, to);
            }
            else
            {
                printf("ERROR: A queen cannot move there.\n");
                return;
            }
        }
        else if(piece == king)
        {
            if((to-from == 24) || (to-from == -24) || (to-from == 3) || (to-from == -3) || \
               (to-from == 21) || (to-from == -21) || (to-from == 27) || (to-from == -27))
            {
                update(fp, from, to);
            }
            else
            {
                printf("A king cannot move there.\n");
                return;
            }
        }
    }
    if((p2[1]-48) == 6) // if king is captured, game over.
    {
        printf("%s wins. %s king was captured.", getTurn(turn), getTurn(turn+1));
        exit(0);
    }
    printf("Captured piece at %c%d from %c%d.\n", opp1, opp2, mine1, mine2);
    switchTurn(turn);
}


/*Check if the move/capture is legal based on the piece that is at from and the piece at to.
    Certain pieces cannot jump over others. This includes pawns, rooks, queens, and bishops. Knights can jump over any piece and kings only
    move one space so it is impossible for them to jump over any piece. */
int isClear(int fp, int from, int to, int *turn, int piece)
{
    char p[2];
    if(piece == pawn)
    {
        if(*turn)   //if black's turn
        {
            from+=24;
            while(from < to)
            {
                if(lseek(fp, from, 0) >= 0)
                {
                    read(fp, p, SIZE);
                    if(p[1] != '0')
                        return 0;
                }
                from+=24;
            }
            return 1;
        }
        else        //if white's turn
        {
            from-=24;
            while(from > to)
            {
                if(lseek(fp, from, 0) >= 0)
                {
                    read(fp, p, SIZE);
                    if(p[1] != '0')
                        return 0;
                }
                from-=24;
            }
            return 1;
        }
    }
    else if(piece == knight)    //knights can jump over anything
    {
        return 1;
    }
    else if(piece == bishop)
    {
        int temp = from-to;
        if(temp > 0)
        {
            if(temp % 27 == 0)          //moving north-west
            {
                from-=27;
                while(from > to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from-=27;
                }
            }
            else    //temp % 21 must be true    //moving north-east
            {
                from-=21;
                while(from > to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from-=21;
                }
            }
        }
        else    //temp is negative
        {
            if(temp % 27 == 0)                  //moving south-east
            {
                from+=27;
                while(from < to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from+=27;
                }
            }
            else    //temp % 21 must be true    //moving south-west
            {
                from+=21;
                while(from < to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from+=21;
                }
            }
        }
        return 1;
    }
    else if(piece == rook)
    {
        int temp = from-to;
        //checks for diagonal movement since 21 and 27 are multiples of 3. It will return true
        //  because this will be caught later in an if statement.
        if(((temp % 21) == 0) || ((temp % 27) == 0))
            return 1;
        if(temp > 0)
        {
            if(temp % 24 == 0)          //moving north
            {
                from-=24;
                while(from > to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from-=24;
                }
            }
            else    //temp % 3 must be true    //moving west
            {
                from-=3;
                while(from > to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from-=3;
                }
            }
        }
        else    //temp is negative
        {
            if(temp % 24 == 0)                  //moving south
            {
                from+=24;
                while(from < to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from+=24;
                }
            }
            else    //temp % 3 must be true    //moving south-west
            {
                from+=3;
                while(from < to)
                {
                    if(lseek(fp, from, 0) >= 0)
                    {
                        read(fp, p, SIZE);
                        if(p[1] != '0')
                            return 0;
                    }
                    from+=3;
                }
            }
        }
        return 1;
    }
    else if(piece == queen)
    {
        //Since a queen can move the same way a bishop and rook can, calling isClear as a bishop or rook will return the appropriate result.
        int temp = from-to;
        if(((temp % 21) == 0) || ((temp % 27) == 0))
            return isClear(fp, from, to, turn, bishop);
        else if(((temp % 24) == 0) || ((temp % 3) == 0))
            return isClear(fp, from, to, turn, rook);
    }
    else if(piece == king)  //king cannot move more than one space so it is impossible to jump over anything.
    {
        return 1;
    }
    
    return 0;
}

/* Updates the board using lseek and read/write.    */
void update(int fp, int from, int to)
{
    char start[2];
    
    lseek(fp, from, 0);         //goes to the piece location
    read(fp, start, SIZE);      //assigns the piece info to 'start'
    lseek(fp, -2, 1);           //moves file location to the beginning of the first piece location.
    write(fp, " 0", SIZE);      //overwrites the first piece location with 0.
    
    lseek(fp, to, 0);
    write(fp, start, SIZE);
}

/* Switches the value at t to 0 or 1. 0 = WHITE, 1 = BLACK. The function will always switch to the other one.   */
void switchTurn(int *t)
{
    *t *= -1;
    /*
    if(*t == WHITE)
        (*t)++;
    else
        (*t)--;
     */
}

/* Returns which color is allowed to go. */
char *getTurn(int *t)
{
    if(*t == WHITE)
        return "White";
    else
        return "Black";
}

/* Calculates the position for lseek from the user's input  */
int calculate(char letter, int number)
{
    int L = (letter - 97) * 3;
    int n = (8 - number) * 24;
    int position = L + n;
    return position;
}


