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
	cin >> input;
#if LOGGING
	f<<"LISTENING, INIT IS "<<INIT<<"\t command is "<<input <<endl;
#endif
	while (true)
		if (input == end){
#if LOGGING
			f<<"DONE "<< input <<endl;
#endif
			return 0;
		}
		else if (input == begin){
			//first I want the gravity, then number of ROWS, then number of COLS, then the col of the last move, then the row of the last move then the values for all the spaces.
			// 0 for no gravity, 1 for gravity
			//then COLS
			//then ROWS
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
			int rowCount = g;
#if LOGGING
			f<<"col: "<<g<<endl;
#endif
			cin >>g;
			int colCount = g;
#if LOGGING
			f<<"row: "<<g<<endl;
#endif
			cin >>g;
			int lastMoveRow = g;
#if LOGGING
			f<<"r:"<<g<<endl;
#endif
			cin >> g;
			int lastMoveCol = g;
#if LOGGING
			f<<"c:"<<g<<endl;
#endif

			//add the deadline here:
			int deadline = -1;
			cin >>g;
			deadline = g;
            if (deadline>100) deadline-=100;
            //we leave some room for timeup
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
				GameStates.resize(rowCount);
				for (int i=0; i<rowCount; i++)
				{
					GameStates[i].resize(colCount);
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
			{
                tick();
				ROWS=rowCount;
				COLS=colCount;
				K=k;
				gravity=_gravity;
				time_limit=deadline;
				lastmove=mv(_mv(lastMoveRow,lastMoveCol),OPPONENT_PIECE);
				mark_move(GameStates,lastmove);
				NewStates=GameStates;
                GameTree->depth=0;
                GameTree->TotalValue=0;
                GameTree->coord=lastmove.first;
                GameTree->children.clear();
				expand_all_children(GameTree,false);
                determine_moves_made();
			}
#if LOGGING
			f<<"parsing done"<<endl;
            print_board();
#endif
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
/*
KTreeNode_
	Master::expand_one_child(KTreeNode_ parent)
{
#if LOGGING
	f <<"expanding a child" <<endl;
#endif
	auto temp=getOneMove(NewStates, parent);
	//here we ask that NewStates is already altered.
	KTreeNode_ child(new KTreeNode);
	child->coord=temp.first;
	child->depth=(parent->depth)+1;
	parent->children.push_back(child);
#if LOGGING
	f <<"output coord is "<<temp.first.first<<" "<<temp.first.second<<"\t"<<(int)temp.second<<endl;
#endif
	return child;
};
*/
void
	Master::expand_all_children ( KTreeNode_ parent, bool check )
{
#if LOGGING
	f <<"expanding all children" <<endl;
#endif
	if(check) if (game_over(parent)) return;
	auto temp=getAllMoves(NewStates, parent);
	for (auto it: temp)
	{
		KTreeNode_ child(new KTreeNode);
		child->coord=it.first;
		child->depth=(parent->depth)+1;
		parent->children.push_back(child);
#if LOGGING
		f <<"output coord is "<<it.first.first<<" "\
			<<it.first.second<<"\t"<<(int)it.second\
			<<"\t depth:"<<child->depth<<endl;
#endif
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
std::vector<KTreeNode_>
	Master::update_frontier ( )
{
	std::vector<KTreeNode_> out;
	out.reserve(COLS*ROWS);
	std::priority_queue<KTreeNode_,std::vector<KTreeNode_>, cmpr_1> temp;
	Frontier=temp;
	//note that we only expand frontier at each new round, thus using GameStates is okay
	for (auto it: GameTree->children)
	{
		Frontier.push(it);
		//First, we preserve the Frontier from results of last round
	}
	temp=Frontier;
	do
	{
		out.push_back(temp.top());
		temp.pop();
		//then we get these nodes in this order to search
	} while (!temp.empty());

	return out;
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
#if LOGGING
	f <<"doing interative deepening search" <<endl;
	__i__=1;
#endif
	unsigned n=1;
	do
	{
		//at each iteration we search in order of priority
		//for that we copy the priority queue once
		auto temp_queue=update_frontier();
		//reset_frontier();
		//we do not reset
		for(auto root:temp_queue)
		{
			//pruning parameters
			curval=0;
			if (time_up()) return;
			//           NewStates=GameStates;
			try
			{
				do_IDS(root,n);
			}
			catch (ERRORS a)
			{
				for (auto it:root->children)
					if(root->TotalValue<it->TotalValue)
						root->TotalValue=it->TotalValue;
				root->children.clear();
				if(a==TIME_UP) return ;
				else continue;
			}
			for (auto it:root->children)
				if(root->TotalValue<it->TotalValue)
					root->TotalValue=it->TotalValue;
#if LOGGING
			f <<"traversing through a node at depth "<<n<<" total iteration is (slightly less than) "<<__i__<<endl;
#endif
#if LOGGING4
			f <<"board for move "<<root->coord.first<<" "<<root->coord.second<<" has value "<<root->TotalValue <<endl;
            print_board();
#endif
			root->children.clear();
		}
		n++;
#if LOGGING
		f <<"finishing one round at "<<n<<endl;
#endif
	}
	while (!time_up());
	return ;
}		/* -----  end of function Master::IDSearch  ----- */
void
	Master::do_IDS ( KTreeNode_ root , const unsigned& depth)
{
#if LOGGING
	__i__++;
	f <<"IDS: Now at "<<root->depth<<endl;
#endif
	//first, what was the last move (temp) played?
	//if depth is divisible by 2, then it is MY_PIECE
	//else OPPONENT_PIECE
    moves_left=ROWS*COLS-moves_made-root->depth;
	if (root->depth%2)
    {
        whose_turn=MY_PIECE;
		mark_move(NewStates,mv(root->coord,MY_PIECE));
    }
	else
    {
        whose_turn=OPPONENT_PIECE;
		mark_move(NewStates,mv(root->coord,OPPONENT_PIECE));
    }
	if (root->depth<depth)
	{
        if(root->depth%2) alpha=-1000;
        else beta=1000;
		expand_all_children(root);
		for (auto it:root->children)
        {
			do_IDS(it, depth);
            if (root->depth%2)
            {
                if (it->TotalValue>root->TotalValue)
                    root->TotalValue=it->TotalValue;
                alpha=root->TotalValue;
            }
            else
            {
                if (it->TotalValue<root->TotalValue)
                    root->TotalValue=it->TotalValue;
                beta=root->TotalValue;
            }
            if (alpha>=beta)
            {
#if LOGGING2
                f <<"pruned "<<alpha<<" "<<beta<<endl;
#endif
                break;
            }
        }
	}
	else
	{
		auto val=addheuristic();
		root->TotalValue=val;
        mark_move(NewStates,mv(root->coord,NO_PIECE));
        return;
	}
	root->children.clear();
	//this makes linear memory
	mark_move(NewStates,mv(root->coord,NO_PIECE));
	//unplay this move
#if LOGGING2
	f <<"alpha beta is now "<<alpha<<" "<<beta<<endl;
#endif
	if(time_up()) throw TIME_UP;
	return;
}		/* -----  end of function Master::do_IDS  ----- */
void
	Master::test_init (  )
{
	//initialize a test situation
	GameStates.resize(5);
	for (int i=0; i<5; i++)
	{
		GameStates[i].resize(5);
	}
	ROWS=5;
	COLS=5;
	K=4;
	gravity=false;
	time_limit=4000;
	mv lastmove=mv(_mv(0,0),MY_PIECE);
	mark_move(GameStates,lastmove);
	lastmove=mv(_mv(2,2),OPPONENT_PIECE);
	mark_move(GameStates,lastmove);
	lastmove=mv(_mv(2,3),OPPONENT_PIECE);
	mark_move(GameStates,lastmove);
	NewStates=GameStates;
	GameTree->coord=_mv(1,0);
	GameTree->depth=0;
	expand_all_children(GameTree,false);
	tick();
	return ;
}		/* -----  end of function test_init  ----- */
void Master::print_board()
{
    for (int col =0; col<ROWS; col++){
        for (int row =0; row<COLS; row++){
            f<<(int)NewStates[col][row];
            f<<"\t";
        }
        f<<endl;
    }
};

float Master::connections (movetype TYPE)
{
	std::vector<float> _connected;
	_connected.reserve(ROWS*COLS);
	//for fast push
	int i,j;
	auto traverse=[this,&i,&j,&TYPE](int inc_row, int inc_col) -> float 
	{
		float out=1,f1=0,f2=0;
		auto k=i,l=j;
		bool flag=false;
		do 
		{
			k+=inc_row;
			l+=inc_col;
			if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
			else if(this->NewStates[k][l]==TYPE)
				out++;
			else flag=true;
		} 
		while (!flag);
		k=i,l=j;
		flag=false;
		do 
		{
			k+=inc_row;
			l+=inc_col;
			if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
			else if(this->NewStates[k][l]==NO_PIECE)
				f1++;
			else if (this->NewStates[k][l]==TYPE)
				;
			else flag=true;
		} 
		while (!flag);
		k=i,l=j;
		flag=false;
		do 
		{
			k-=inc_row;
			l-=inc_col;
			if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
			else if(this->NewStates[k][l]==TYPE)
				out++;
			else flag=true;
		} 
		while (!flag);
		k=i,l=j;
		flag=false;
		do 
		{
			k-=inc_row;
			l-=inc_col;
			if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
			else if(this->NewStates[k][l]==NO_PIECE)
				f2++;
			else if (this->NewStates[k][l]==TYPE)
				;
			else flag=true;
		} 
		while (!flag);

        if (out>=K) return 200;
        else if (f1*f2 > 0)
		{
			if (out>=K-1)
				return 100;
			if (out>=K-2)
				return 25;
			else return out+1;
		}
		else if (out+f1+f2<K)
			return 0;
		else
		{
			if (out>=K-1) return 50;
			else return out;
		}
		
	};
	for ( i = 0; i < ROWS; i++) 
		for ( j = 0; j < COLS; j++) 
		{
			if (NewStates[i][j]==TYPE)
			{
				// try to find a new connections
				float OUTS[4]={traverse(-1,1),traverse(1,0),traverse(0,1),traverse(1,1)};
				std::sort(OUTS, OUTS+4);
				_connected.push_back(OUTS[3]);
			}
		}
	if (_connected.size()==0) return 0; 
	std::sort(_connected.begin(), _connected.end());
	return (*_connected.rbegin());
};
