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
 *  Description:  modified from CPP shell provided, simply listens from stdin
 *  Var. In:      
 *  Output:       
 * =====================================================================================
 *
 */
    bool 
Master::listen(const bool INIT=0)
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
            lastmove=mv(_mv(lastMoveCol,lastMoveRow),OPPONENT_PIECE);
            mark_move(GameStates,lastmove);
            NewStates=GameStates;
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

    bool
Master::tell_move(const mv& mymove)
{
    string madeMove = "ReturningTheMoveMade";
#if LOGGING
    f << madeMove<<" "<< mymove.first.first<<" " << mymove.first.second <<endl;
#endif
    cout << madeMove<<" "<< mymove.first.first<<" " << mymove.first.second <<endl;
    //lets do our own book keeping.
    mark_move(GameStates,mymove);
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
    KTreeNode_
Master::expand_one_child(KTreeNode_ parent)
{
    auto temp=getOneMove(NewStates, parent);
    //here we ask that NewStates is already altered.
	KTreeNode_ child(new KTreeNode);
	child->coord=temp.first;
	child->depth=(parent->depth)+1;
	parent->children.push_back(child);
    return child;
};
    void
Master::expand_all_children ( KTreeNode_ parent )
{
    auto temp=getAllMoves(NewStates, parent);
    for (auto it: temp)
    {
		KTreeNode_ child(new KTreeNode);
		child->coord=it.first;
		child->depth=(parent->depth)+1;
		parent->children.push_back(child);
    }
    return ;
}		/* -----  end of function Master::expand_all_children  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Master::update_frontier
 *  Description:  Update the current game node after it is reassigned
 *  Var. In:      Parent
 *  Output:       none
 * =====================================================================================
 */
    void
Master::update_frontier ( )
{
	expand_all_children(GameTree);
    //note that we only expand frontier at each new round, thus using GameStates is okay
    for (auto it: GameTree->children)
    {
		GameTree->children.push_back(it);
    }
    return ;
}		/* -----  end of function Master::update_frontier  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Master::IDSearch
 *  Description:  This is our iterative deepening search algorithm.
 *                Here we assume a few things, first, the frontier is already expanded
 *                second, for each depth n, we expand depth first to n and expand all 
 *                children at n (this follows from the logic of breadth first expandsion)
 *                Third, for each leaf node at n, we assess the with heuristics (using 
 *                NewStates, and start accounting for alpha and beta values.
 *                Fourth, when this is done, 
 *  Var. In:      None, traverse is only stopped if time is up
 *  Output:       None, upon finish the priority queue should be ready to pop
 * =====================================================================================
 */
    void
Master::IDSearch ( )
{
    unsigned n=1;
    do
    {
        //pruning parameters
        alpha=-100;
        beta=100;
        //at each iteration we search in order of priority
        //for that we copy the priority queue once
        auto temp_queue=Frontier;
        do
        {
            NewStates=GameStates;
            auto root=temp_queue.top();
            do_IDS(root,n);
            root->TotalValue=curval;
            root->children.clear();
            //this makes memory linear
            temp_queue.pop();
            // go to the next node
        }
        while (!temp_queue.empty());
        
        n++;
    }
    while (!time_up());
    return ;
}		/* -----  end of function Master::IDSearch  ----- */
    void
Master::do_IDS ( KTreeNode_ root , const unsigned& depth)
{
    if (alpha>beta) return;
    //first, what was the last move (temp) played?
    //if depth is divisible by 2, then it is MY_PIECE 
    //else OPPONENT_PIECE
    if (root->depth%2==0)
        mark_move(NewStates,mv(root->coord,MY_PIECE));
    else
        mark_move(NewStates,mv(root->coord,OPPONENT_PIECE));
    if (root->depth<=depth)
    {
        expand_all_children(root);
        for (auto it:root->children)
            do_IDS(it, depth+1);
        //root->TotalValue=curval;
        //annotate current values;
    }
    else
    {
        alpha=-100;
        beta=100;
        int val=depth*2;//addheuristic();
        if (val>curval) curval=val;
    }
    mark_move(NewStates,mv(root->coord,NO_PIECE));
    //unplay this move 
    root->children.clear();
    //this makes linear memory
    return;
}		/* -----  end of function Master::do_IDS  ----- */
