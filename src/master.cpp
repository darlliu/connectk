/*
 * =====================================================================================
 *
 *       Filename:  master.cpp
 *
 *    Description:  See master.h
 *
 *        Created:  2/20/2013 8:53:53 PM
 *
 * =====================================================================================
 */
#include"master.h"
#include<iostream>
#include<string>
#include <stdlib.h>
using namespace std;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  listen
 *  Description:  
 *  Var. In:      
 *  Output:       
 * =====================================================================================
 */
bool Master::listen(bool INIT=0)
{
	string begin =  "makeMoveWithState:"; 
	string end = "end";
	string input;
    cin >> input;
#if LOGGING
    std::ofstream f;
    f.open("logs.txt");
    f<<"LISTENING, INIT IS "<<INIT<<endl;
#endif
    if (input == end){
			return 0;
    }
    else if (input == begin){
        //first I want the gravity, then number of cols, then number of rows, then the col of the last move, then the row of the last move then the values for all the spaces.
        // 0 for no gravity, 1 for gravity
        //then rows
        //then cols
        //then lastMove col
        //then lastMove row.
        //then deadline.
        //add the K variable after deadline. 
        //then the values for the spaces.
        //cout<<"beginning"<<endl;
        int g;
        cin >> g;
        bool _gravity=true;
        if (g==0){
            _gravity = false;
        }
#if LOGGING
        f<<g<<endl;
#endif 
        cin >> g;
        int colCount = g;
        cin >>g;
#if LOGGING
        f<<g<<endl;
#endif 
        int rowCount = g;
        cin >>g;
#if LOGGING
        f<<g<<endl;
#endif 
        int lastMoveCol = g;
        cin >> g;
#if LOGGING
        f<<g<<endl;
#endif 
        int lastMoveRow = g;

        //add the deadline here: 
        int deadline = -1;
        cin >>g;
#if LOGGING
        f<<g<<endl;
#endif 
        deadline = g;

        cin >> g;
#if LOGGING
        f<<g<<endl;
#endif 
        int k = g;

        //now the values for each space.

        //allocate 2D array.
        /* 
        int **gameState = NULL;
        gameState = new int*[colCount];
        for (int i =0; i<colCount; i++){
            gameState[i] = new int[rowCount];
        }
        */
        if (INIT)
        {
            GameStates.resize(rowCount);
            NewStates.resize(rowCount);
            for (int i=0; i<rowCount; i++)
            {
                GameStates[i].resize(colCount);
                NewStates[i].resize(colCount);
            }
        }
		int temp;
        for (int col =0; col<colCount; col++){
            for (int row =0; row<rowCount; row++){
                cin >>temp;
				GameStates[col][row]=(movetype)temp;
#if LOGGING
                f<<g<<"\t";
#endif 
            }
#if LOGGING
                f<<g<<endl;
#endif 
        }
        {
            COLS=colCount;
            ROWS=rowCount;
            K=k;
            gravity=_gravity;
            time_limit=deadline;
            NewStates=GameStates;
            lastmove=mv(_mv(lastMoveRow,lastMoveCol),OPPONENT_PIECE);
        }
		return 1;
    }
    else {
        cout<<"unrecognized command "<< input <<endl;
        return 0;
    }
    //otherwise loop back to the top and wait for proper input.
};

bool Master::tell_move(mv mymove)
{
    string madeMove = "ReturningTheMoveMade";
    cout << madeMove<<" "<< mymove.first.first<<" " << mymove.first.second <<endl;
    return true;
};
