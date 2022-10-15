#include <stdio.h>
#include <math.h>

//use a struct to keep track of all of the relevant info for each game
struct sudoku {
    //main grid for storing the sudoku board
    int grid[9][9];

    //a 3d array in which each layer represents the possible positions
    //of each number (1->9) i.e. initial layer holds all poss positions of ones
    int poss[9][9][9];


    //this grid is designed to limit the number of computations done
    //by checking for available space in sub before main 
    int sub_poss[3][3][9];
};

void displayPoss(struct sudoku *cur_sud){
    for(int z = 0; z < 9; z++) {
        printf("\nLayer: %d\n", z+1);
        printf("_____________________________________\n");
        for (int y = 0; y < 9;  y++){
            for (int x = 0; x < 9; x++){
                printf("|");
                printf(" ");
                printf("%d", cur_sud->poss[y][x][z]);
                printf(" ");
            }
            printf("|");
            printf("\n");
            printf("_____________________________________\n");
        }
        printf("\n_____________\n");
        for (int y = 0; y < 3;  y++){
            for (int x = 0; x < 3; x++){
                printf("|");
                printf(" ");
                printf("%d", cur_sud->sub_poss[y][x][z]);
                printf(" ");
            }
            printf("|");
            printf("\n");
            printf("_____________\n");
        }
    }
}



void displayGrid(struct sudoku *cur_sud){
    printf("Main Grid\n");
    printf("_____________________________________\n");
    for (int y = 0; y < 9;  y++){
        for (int x = 0; x < 9; x++){
            printf("|");
            printf(" ");
            printf("%d", cur_sud->grid[y][x]);
            printf(" ");
        }
        printf("|");
        printf("\n");
        printf("_____________________________________\n");
    }
}


void setDimension(struct sudoku *cur_sud){
    //use zero for empty cells and initialize all cells to empty
    for (int y = 0; y < 9;  y++){
        for (int x = 0; x < 9; x++){
            cur_sud->grid[y][x] = 0;
        }
    }

    //this will keep track of the possibilities for each cell
    //iniatilize so that it is a block of numbers layered 1-> 9
    for (int y = 0; y < 9;  y++){
        for (int x = 0; x < 9; x++){
            for(int z = 0; z < 9; z++){
                cur_sud->poss[y][x][z] = z+1;
            }
        }
    }

    for (int y = 0; y < 3;  y++){
        for (int x = 0; x < 3; x++){
            for(int z = 0; z < 9; z++){
                cur_sud->sub_poss[y][x][z] = 0;
            }
        }
    }
}


//function for updating the state of the possibilities array every time a number added to the sudoku grid
void update_poss(struct sudoku *cur_sud, int x, int y, int number){
    //remove entries from the possibilites array in that row and column
    for(int i = 0; i < 9; i++) {
        cur_sud->poss[y][i][number - 1] = 0;
        cur_sud->poss[i][x][number - 1] = 0;
        cur_sud->poss[y][x][i] = 0;
    }

    //remove entries from possibilities array within the 3x3 grid where the num is located
    int x_start = (int)floor(x/3) * 3;
    int y_start = (int)floor(y/3) * 3;
    int x_end = x_start + 3;
    int y_end = y_start + 3;

    //main poss grid update
    for(int ysub = y_start; ysub < y_end; ysub++){
        for(int xsub = x_start; xsub < x_end; xsub++){
            cur_sud->poss[ysub][xsub][number - 1] = 0;
        }
    }

    //sub poss grid update
    x_start = x_start/3;
    y_start = y_start/3;
    cur_sud->sub_poss[y_start][x_start][number -1] = number;
}

//purge poss grid of options based off of the relative locations of other possibilities
//in the same 3x3 grid
void purge_poss(struct sudoku *cur_sud) {}

//setting the sudoku object when receiving board as array
void load_grid(int grid[9][9], struct sudoku *cur_sud){
    for (int y = 0; y < 9;  y++){
        for (int x = 0; x < 9; x++){
            cur_sud->grid[y][x] = grid[y][x];

            //update possiblities array as we upload the grid
            if(grid[y][x] != 0){
                update_poss(cur_sud,x,y,grid[y][x]);
            }  
        }
    }
}


int count_entries(struct sudoku *cur_sud){
    int count = 0;
    for(int y = 0; y < 9; y++){
        for(int x = 0; x < 9; x++){
            if(cur_sud->grid[y][x] != 0){
                count++;
            }
        }
    }
    return count;
}

void copy_grid(struct sudoku *cur_sud, struct sudoku *new_sud) {
    for(int y = 0; y < 9; y++) {
        for(int x = 0; x < 9; x++) {
            if(cur_sud->grid[y][x] != 0) {
                new_sud->grid[y][x] = cur_sud->grid[y][x];
                update_poss(new_sud, x,y,cur_sud->grid[y][x]);
            }
        }
    }
}




//search poss grid within a 3x3 sub grid , row, or column for a single entry
int search_grid(struct sudoku *cur_sud, int subx, int suby, int level, int * result, char type){
    int count = 0;
    subx = subx*3;  suby = suby*3;
    if(type == 'm') { 
        for(int y = suby; y < suby + 3; y++){
            for(int x = subx; x < subx + 3; x++){
                if(count == 0 && cur_sud->poss[y][x][level] == level+1) {
                    result[0] = y;
                    result[1] = x;
                }
                if(cur_sud->poss[y][x][level] == level+1){
                    count++;
                }
            }
        }
        if (count == 1) {
            return 1;
        }
        else {
            result[0] = -1; result[1] = -1;
            return 0;
        }
    }
    else if(type == 'r') {
        for(int y = suby; y < suby + 3; y++) {
            for(int x = 0; x < 9; x++) {
                if(count == 0 && cur_sud->poss[y][x][level] == level+1) {
                    result[0] = y;
                    result[1] = x;
                }
                if(cur_sud->poss[y][x][level] == level+1){
                    count++;
                }
            }
            if(count == 1){
                return 1;
            }
            else {
                count = 0;
            }
        }
        return 0;
    }
    else if(type == 'c') {
        for (int x = subx; x < subx + 3; x++) {
            for (int y = 0; y < 9; y++) {
                if(count == 0 && cur_sud->poss[y][x][level] == level+1) {
                    result[0] = y;
                    result[1] = x;
                }
                if(cur_sud->poss[y][x][level] == level+1){
                    count++;
                }                
            }
            if(count == 1){
                return 1;
            }
            else {
                count = 0;
            }
        }
        return 0;
    }
}


//iterate through the possibility array to check for
int fill_grid(struct sudoku *cur_sud){
    int result[2] = {-1,-1};
    for (int i = 0; i < 9; i++){
        for (int y = 0; y < 3; y++){
            for(int x = 0; x < 3; x++){
                if( cur_sud->sub_poss[y][x][i] == 0)
                {
                    
                    int f_mini = search_grid(cur_sud,x,y,i, result, 'm');
                    if (f_mini)
                    {
                        cur_sud->grid[result[0]][result[1]] = i + 1; 
                        update_poss(cur_sud, result[1], result[0], i+1); 
                        return 1; 
                    }
                    
                    int f_rows = search_grid(cur_sud,x,y,i,result, 'r');
                    if(f_rows)
                    {
                        cur_sud->grid[result[0]][result[1]] = i + 1; 
                        update_poss(cur_sud, result[1], result[0], i+1); 
                        return 1; 
                    }

                    int f_columns = search_grid(cur_sud, x,y,i,result, 'c');
                    if (f_columns) {
                        cur_sud->grid[result[0]][result[1]] = i + 1; 
                        update_poss(cur_sud, result[1], result[0], i+1); 
                        return 1;                         
                    }

                }
            }
        }
    }
    return 0;
}



void is_solvable(struct sudoku *cur_sud) {
    int solvable = 1;
    int zc = 0;
    //because we are checking for vacant slots we can just check for if there is a single zero in a row
    //i am not sure whether or not it is sufficient to check just rows or columns so im going to check all
    for (int y = 0; y < 9; y++) {
        for(int x = 0; x <9; x++) {
            if(cur_sud->grid[y][x] == 0) {
                zc++;
            }
            if(zc > 1) {
                zc= 0;
                break;
            }
        }
        if(zc == 1) {
            return 0;
        }
    }
    //must be done seperately in order to fully check each row and column separately 
    for (int y = 0; y < 9; y++) {
        for(int x = 0; x <9; x++) {
            if(cur_sud->grid[x][y] == 0) {
                zc++;
            }
            if(zc > 1) {
                zc = 0;
                break;
            }
        }
        if(zc == 1) {
            return 0;
        }
    }
    int subx = 0; int suby = 0;
    for(int i = 0 ; i < 9; i++) {
        for (int y = suby*3; y < (suby*3) + 3; y++) {
            for(int x = subx*3; x < (subx*3) + 3; x++) {
            if(cur_sud->grid[y][x] == 0) {
                zc++;
            }
            if(zc>1) {
                zc= 0;
                break;
            }
        }
        subx++;
        if(i == 2){
            suby++;
            subx=0;
        }
    } 
}




//find a 3x3 sub grid, row, or column with only two entries in the poss array
//pick one and calculate how many entries can logically be added and whether or not
//it is logically consistent
void bin_gen(struct sudoku *cur_sud, struct sudoku *new_sud){
    copy_grid(cur_sud, new_sud);
    
}


int main(){

    //use temporary sample board
    int med[9][9] = {{0,0,3,0,0,0,0,6,0},
                     {0,0,7,2,1,0,9,0,3},
                     {0,4,0,0,0,3,0,1,8},
                     {0,0,0,5,0,1,0,2,6},
                     {0,8,2,0,0,0,1,5,0},
                     {0,0,1,0,2,4,0,0,0},
                     {0,0,0,3,0,0,0,0,0},
                     {0,0,0,1,5,8,4,3,7},
                     {7,3,0,4,0,0,0,0,0}};
    int eas[9][9] ={{3, 0, 6, 5, 0, 8, 4, 0, 0}, 
                    {5, 2, 0, 0, 0, 0, 0, 0, 0}, 
                    {0, 8, 7, 0, 0, 0, 0, 3, 1}, 
                    {0, 0, 3, 0, 1, 0, 0, 8, 0}, 
                    {9, 0, 0, 8, 6, 3, 0, 0, 5}, 
                    {0, 5, 0, 0, 9, 0, 6, 0, 0}, 
                    {1, 3, 0, 0, 0, 0, 2, 5, 0}, 
                    {0, 0, 0, 0, 0, 0, 0, 7, 4}, 
                    {0, 0, 5, 2, 0, 6, 3, 0, 0}};

    int hrd[9][9] ={{0, 0, 9, 0, 0, 0, 1, 0, 0}, 
                    {0, 2, 1, 0, 0, 5, 0, 9, 6}, 
                    {0, 7, 0, 0, 0, 0, 0, 0, 0}, 
                    {0, 5, 0, 0, 0, 0, 6, 0, 2}, 
                    {0, 0, 0, 0, 0, 0, 8, 3, 1}, 
                    {0, 0, 2, 0, 0, 3, 0, 0, 0}, 
                    {0, 1, 0, 0, 2, 9, 0, 0, 3}, 
                    {0, 0, 0, 0, 7, 8, 0, 0, 0}, 
                    {4, 0, 0, 0, 0, 1, 9, 0, 0}};
    int expert[9][9] ={{0, 7, 0, 0, 0, 0, 4, 0, 5}, 
                    {0, 0, 0, 0, 0, 1, 0, 0, 6}, 
                    {2, 0, 5, 0, 7, 0, 0, 0, 0}, 
                    {0, 0, 4, 2, 0, 0, 0, 0, 8},
                    {0, 0, 0, 7, 0, 0, 0, 1, 0}, 
                    {1, 3, 0, 0, 0, 5, 0, 0, 9}, 
                    {0, 0, 0, 5, 0, 0, 1, 0, 0}, 
                    {9, 0, 0, 3, 0, 0, 0, 6, 0}, 
                    {6, 0, 0, 0, 0, 0, 0, 0, 4}};
    
    struct sudoku board;
    setDimension(&board);
    load_grid(expert, &board);

    int original_entries = count_entries(&board);
    int goAgain = 1;
    int mini = 1;

    while(mini) {mini = fill_grid(&board);}

    //visual analytics for debugging and improving code   
    int new_entries = count_entries(&board);
    displayPoss(&board);
    displayGrid(&board);
    printf("Original Entries: %d\n", original_entries);
    printf("Entries Added: %d\n", new_entries - original_entries);
    printf("Empty Cells: %d", 81 - new_entries);
    return 0;
    
}