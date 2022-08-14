#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>


static uint8_t initial_board[9][9] = {
    {7,0,0,0,0,0,0,5,3},
    {0,0,0,0,0,0,0,0,0},
    {0,4,8,0,3,0,1,2,0},
    {8,0,1,3,0,0,2,7,5},
    {0,9,0,1,0,0,0,4,0},
    {0,5,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {2,0,5,0,8,4,0,0,0},
    {1,3,4,0,5,0,0,0,0},
};

static uint8_t solution[81] = { 0 };

static uint8_t board[9][9];


void win(void)
{
    // TODO: switch this to like "sudo ku" to make it harder
    system("/bin/bash");
}


void print_puzzle()
{
    puts("Current puzzle state:");
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            uint8_t cell_val = board[row][col];
            if (cell_val == 0)
            {
                printf("? ");
            }
            else
            {
                printf("%hhu ", board[row][col]);
            }
        }
        puts("");
    }
}


uint8_t check_puzzle()
{
    uint32_t col_bits[9] = { 0 };
    uint32_t box_bits[9] = { 0 };

    // Check for all values in row/col/box
    for (int row = 0; row < 9; row++)
    {
        uint32_t row_bits = 0;
        for (int col = 0; col < 9; col++)
        {
            uint8_t cell_val = board[row][col];
            if (cell_val == 0 || cell_val > 9)
            {
                return 0;
            }
            uint8_t cell_mask = 1 << (cell_val - 1);

            int box_num = ((row / 3)*3) + (col / 3);
            if (box_bits[box_num] & cell_mask)
            {
                return 0;
            }
            box_bits[box_num] |= cell_mask;

            if (row_bits & cell_mask)
            {
                return 0;
            }
            row_bits |= cell_mask;

            if (col_bits[col] & cell_mask)
            {
                return 0;
            }
            col_bits[col] |= cell_mask;
        }
    }

    return 1;
}


void solve_puzzle()
{
    char sol_str[1024] = {0};

    int8_t num_empty = 0;
    for (int i = 0; i < sizeof(board); i++)
    {
        uint8_t* itr = ((uint8_t*)board) + i;
        if (*itr == 0)
        {
            num_empty++;
        }
    }

    puts("Your solution:");
    if (fgets(sol_str, sizeof(sol_str), stdin) == NULL)
        return;

    int8_t num_values = 1;
    for (int i = 0; i < strlen(sol_str); i++)
    {
        if (sol_str[i] == ',')
        {
            num_values++;
        }
    }

    if (num_empty != num_values)
    {
        printf("Wrong number of values: %hhd, expected %hhd\n", num_values, num_empty);
        return;
    }

    char* tok = strtok(sol_str, ",");
    int8_t sol_idx = 0;
    while (tok != NULL)
    {
        solution[sol_idx++] = atoi(tok);
        tok = strtok(NULL, ",");
    }

    sol_idx = 0;
    for (int8_t i = 0; i < sizeof(board); i++)
    {
        uint8_t* itr = ((uint8_t*)board) + i;
        if (*itr == 0)
        {
            *itr = solution[sol_idx++];
        }
    }

    if (!check_puzzle())
    {
        puts("That doesn't seem right...");
    }
    else
    {
        puts("Wow, great job!");
        win();
    }
}


void reset_puzzle()
{
    memcpy(board, initial_board, sizeof(board));
}


void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}


void print_menu()
{
    puts("1. Print puzzle\n"\
    "2. Solve Puzzle\n"\
    "3. Reset puzzle\n"\
    ">"
    );
}


int main(int argc, char** argv, char** envp)
{
    init();
    reset_puzzle();

    char choice[0x10] = { 0 };

    while (1)
    {
        print_menu();

        if (!fgets(choice, sizeof(choice), stdin))
            break;

        switch (choice[0])
        {
            case '1':
                print_puzzle();
                break;
            case '2':
                solve_puzzle();
                break;
            case '3':
                reset_puzzle();
                break;
            case EOF:
                exit(0);
            default:
                puts("Invalid choice");
                break;
        }
    }
    return 0;
}
