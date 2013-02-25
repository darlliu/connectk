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
#include<stdlib.h>
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
#if LOGGING
    f<<"LISTENING, INIT IS "<<INIT<<endl;
#endif
    cin >> input;
    while (true)
    if (input == end){
#if LOGGING
        f<<"DONE "<< input <<endl;
#endif
	return 1;
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
        f<<"grav:"<<g<<endl;
#endif 
        cin >> g;
        int colCount = g;
#if LOGGING
        f<<"col: "<<g<<endl;
#endif 
        cin >>g;
        int rowCount = g;
#if LOGGING
        f<<"row: "<<g<<endl;
#endif 
        cin >>g;
        int lastMoveCol = g;
#if LOGGING
        f<<g<<endl;
#endif 
        cin >> g;
        int lastMoveRow = g;
#if LOGGING
        f<<g<<endl;
#endif 

        //add the deadline here: 
        int deadline = -1;
        cin >>g;
        deadline = g;
#if LOGGING
        f<<g<<endl;
#endif 

        cin >> g;
        int k = g;
#if LOGGING
        f<<g<<endl;
#endif 

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
            GameStates.resize(colCount);
            NewStates.resize(colCount);
            for (int i=0; i<colCount; i++)
            {
                GameStates[i].resize(rowCount);
                NewStates[i].resize(rowCount);
            }
        }
		int temp;
        for (int col =0; col<colCount; col++){
            for (int row =0; row<rowCount; row++){
                cin >>temp;
                //actually by the email from the Java shell author 
                //this does NOT parse the correct games board, then why is it still here?
				//GameStates[col][row]=(movetype)temp;
#if LOGGING
                f<<temp<<"\t";
#endif 
            }
#if LOGGING
                f<<endl;
#endif 
        }
#if LOGGING
                f<<"parsing done";
#endif 
        {
            COLS=colCount;
            ROWS=rowCount;
            K=k;
            gravity=_gravity;
            time_limit=deadline;
            NewStates=GameStates;
            lastmove=mv(_mv(lastMoveCol,lastMoveRow),OPPONENT_PIECE);
            mark_move(lastmove);
       }
        return 1;
    }
    else {
#if LOGGING
        f<<"unrecognized command "<< input <<endl;
#endif
        continue;
    }
    //otherwise loop back to the top and wait for proper input.
};

bool Master::tell_move(mv mymove)
{
    string madeMove = "ReturningTheMoveMade";
#if LOGGING
    f << madeMove<<" "<< mymove.first.first<<" " << mymove.first.second <<endl;
#endif
    cout << madeMove<<" "<< mymove.first.first<<" " << mymove.first.second <<endl;
    //lets do our own book keeping.
    mark_move(mymove);
    return true;
};


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  expand_one_child
 *  Description:  expand the current node with one child, note that we need to check 
 *                child nodes if we don't expand all of them at the same time
 *  Var. In:      parent tree (one node)
 *  Output:       none, node is expand
 *  Note:         when we expand, we can do one of the following:
 *                1, expand every possible child, no need to recheck, proceed to check,
 *                this is NOT depth first though b/c at each depth everything must be 
 *                expanded
 *                2, expand one child and when later back to expand more but has to 
 *                check that the expanded node is NOT already assigned (use the 
 *                signature passing the current node;
 * =====================================================================================
 */
void Master::expand_one_child(KTreeNode_ parent)
{
    
};
