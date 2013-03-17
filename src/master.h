/*
* =====================================================================================
*
*       Filename:  master.h
*
*    Description:  A master class that handles the routine flow of the game
*                  replacing AIShell routine and make it slightly more efficient
*                  In particular it can be inherited with newer heuristics and pruning
*                  strategy etc.
*
*        Created:  2/20/2013 6:34:21 PM
*       Compiler:  g++
*
*         Author:  YU LIU,
*
* =====================================================================================
*/
#ifndef MASTER_H
#define MASTER_H
#include"headers.h"

/*
* =====================================================================================
*        Class:  KTree
*  Description:  Connect K tree that is not balanced in general.
*                Note: this class requires fast implementation
*
* =====================================================================================
*/
class KTreeNode
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	KTreeNode ():
		TotalValue(0),depth(0)
	{coord.first=0; coord.second=0;};
	/* constructor */
	// here is out overloaded operator for priority queue
	std::list<std::shared_ptr<KTreeNode>> children;
	//vector provides constant time access and fixed size;
	//however, list provides constant time resizing, reordering and pruning cost
	//in this case the traverse counts per operation is limited by depth which is small
	float TotalValue;
	//the total value of features given by some heuristic traversal operation
	unsigned depth;
	//depth of this node. If this turns out unnecessary then we can get rid of it.
	_mv coord;
	//coordinate of the current node
}; /* -----  end of class KTree  ----- */

typedef std::shared_ptr<KTreeNode> KTreeNode_;
class cmpr_1
{
public:
	cmpr_1(){};
	bool operator ()(const KTreeNode_ left, const KTreeNode_ right)
	{
		return left->TotalValue < right->TotalValue;
	};
};
/*
* =====================================================================================
*        Class:  Master
*  Description:  the master class and also a base class to be inherited
* =====================================================================================
*/
class Master
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	Master ():
		time_limit(4500), //default time limit is 5000 ms
		gravity(0),
		GameTree(new KTreeNode)
	{
#if 1
		f.open("logs.txt");
#endif
		// our tree, don't lose it!
	};                             /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	mv getOneMove(const states& States,movetype mt=MY_PIECE)
	{
		//get one legal move without excluding any
		//because we search from bottom up gravity doesn't matter
#if LOGGING
		f<<"Now determining move"<<std::endl;
#endif
		for (int i=0; i<ROWS; i++)
			for (int j=0; j<COLS; j++)
			{
				if (States[i][j]==NO_PIECE)
					return mv(_mv(i,j),mt);
			}
			return mv(_mv(-1,-1),NO_PIECE);
	};
	mv getOneMove(const states& States,KTreeNode_ node,movetype mt=MY_PIECE)
	{
		//get one legal move except if it is already in the node's children
#if LOGGING
		f<<"Now determining move"<<std::endl;
#endif
		for (int i=0; i<ROWS; i++)
			for (int j=0; j<COLS; j++)
			{
				if (States[i][j]==NO_PIECE)
					//we check if the move is made in the children already
						for (auto it: node->children)
							if (it->coord.first==i && it->coord.second==j) goto THEN;
				return mv(_mv(i,j),mt);
THEN:
				{
					continue;
				}
			}
			return mv(_mv(-1,-1),NO_PIECE);
	};
    void determine_moves_made()
    {
        moves_made=0;
        for (unsigned i = 0; i<ROWS; i++)
            for (unsigned j = 0; j<COLS; j++)
            {
                if (GameStates[i][j]!=NO_PIECE) 
                    moves_made++;
            }
       return;
    };
	std::vector<mv> getAllMoves(const states& States,movetype mt=MY_PIECE)
	{
#if LOGGING
		f<<"Now determining all moves"<<std::endl;
#endif
		std::vector<mv> out;
		for (int i=0; i<ROWS; i++)
		{
			for (int j=0; j<COLS; j++)
			{
				if (States[i][j]==NO_PIECE)
				{
					out.push_back( mv(_mv(i,j),mt) );
					if (gravity) goto SKIP;
				}
			}
SKIP:
			{
				continue;
			}
		}
		return out;
	};
	std::vector<mv> getAllMoves(const states& States,KTreeNode_ node,movetype mt=MY_PIECE)
	{
		//this is the version that checks the children, use only when necessary
#if LOGGING
		f<<"Now determining all moves"<<std::endl;
#endif
		std::vector<mv> out;
		for (int i=0; i<ROWS; i++)
		{
			for (int j=0; j<COLS; j++)
			{
				if (States[i][j]==NO_PIECE)
					//we check if the move is made in the children already
				{
					for (auto it:node->children)
						if ((it)->coord.first==i && (it)->coord.second==j) goto THEN;
					out.push_back( mv(_mv(i,j),mt) );
					if (gravity) goto SKIP;
                    continue;
				}
                else continue;
THEN:
				{
#if LOGGING
					f<<"Escaped at depth"<<(int)node->depth<<" i , j "<<i<<j<<std::endl;
#endif
					continue;
				}
			}
SKIP:
			{
				continue;
			}
		}
		return out;
	};

	mv Get_Move(){return mv(Frontier.top()->coord,MY_PIECE);};

	/* ====================  MUTATORS      ======================================= */
	bool listen(const bool);
	bool tell_move(const mv&);

	void tick() {t0=hclock::now();};
	ms tock()
	{
		t1=hclock::now();
		auto timer_t = t1-t0;
		return std::chrono::duration_cast<ms> (timer_t);
	};
	bool time_up()
	{
		auto timer_t=tock();
		if (timer_t.count()>time_limit)
		{
			tick();
			return true;
		}
		else return false;
	};
	void mark_move(states& S, mv move)
	{
		/*if (S[c][r]!=NO_PIECE)
		return false;
		else*/
        if (move.first.first<0 || move.first.second<0) return;
		S[move.first.first][move.first.second]=move.second;
		return;
	};
	void reset_frontier()
	{
		for (auto it: GameTree->children)
		{
			it->depth=0;
			it->TotalValue=0;
			//reset the Frontier candidates for next round.
		}
	};
	std::vector<KTreeNode_> update_frontier();
	void IDSearch();
	void do_IDS(KTreeNode_, const unsigned&);
	KTreeNode_ expand_one_child(KTreeNode_ parent);
	void expand_all_children(KTreeNode_ parent,bool check=true);
	void test_init();
	/*void reform_frontier()
	{
	//simply touch and reform the frontier after search done
	std::priority_queue<KTreeNode_,std::vector<KTreeNode_>, cmpr_1> temp;
	auto out=Frontier.top();
	do
	{
	out=Frontier.top();
	temp.push(out);
	Frontier.pop();
	} while (!Frontier.empty());
	Frontier=temp;
	return;
	};*/

    void print_board();

	/* ====================  UTILITIES     ======================================= */
	float connections (movetype TYPE=MY_PIECE);
	bool game_over(KTreeNode_ );
	/* ====================  VIRTUALS     ======================================= */
	virtual float addheuristic(){return 0.0;}; //some heuristic

	virtual void main_routine()
	{
		while (!listen(1));
#if LOGGING
		f<<"Listened successfully"<<std::endl;
#endif
		auto m=getOneMove(GameStates);
#if LOGGING
		f<<"Move get"<<std::endl;
#endif
		tell_move(m);
		while (listen(false))
		{
			m=getOneMove(GameStates);
			tell_move(m);
		}
	};

protected:
	/* ====================  DATA MEMBERS  ======================================= */
	hclock::time_point t0, t1;
	int time_limit;
	int ROWS,COLS;
	float alpha,beta,curval;
	int K,moves_made, moves_left;
    movetype whose_turn;
	mv lastmove;
	bool gravity;
	std::priority_queue<KTreeNode_,std::vector<KTreeNode_>,cmpr_1> Frontier;
	// our priority tree frontier which is maintained via vector for constant time access
	// note that the frontier is quite small in most cases (it's the immediate candidates
	// for next move)
	KTreeNode_ GameTree;
	// The main tree
	states GameStates, NewStates;
	// std vector does deep copy by default so a reassignment can be used to make copy
	// this is a state, NOT the search tree and not describe the search space. constant size

#if 1
	std::ofstream f;
	unsigned __i__;
#endif
}; /* -----  end of class Master  ----- */
#endif
