/*
 * =====================================================================================
 *
 *       Filename:  smartplayer.h
 *
 *    Description:  a derived class of master.h which uses some heuristics
 *
 *        Version:  1.0
 *        Created:  3/10/2013 5:09:34 PM
 *       Compiler:  gcc
 *         Author:  YU LIU
 * =====================================================================================
 */

#ifndef SMARTPLAYER_H
#define SMARTPLAYER_H
#include "master.h"
#include <algorithm>
/*
 * =====================================================================================
 *        Class:  Smartplayer
 *  Description:  As described
 * =====================================================================================
 */
class Smartplayer : public Master
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        Smartplayer (){};                             /* constructor */

        /* ====================  UTILITY     ======================================= */
        std::vector<int> connections (movetype TYPE=MY_PIECE)
        {
            std::vector<int> _connected;
            _connected.reserve(ROWS*COLS);
            //for fast push
            int i,j;
            auto traverse=[this,&i,&j,&TYPE](int inc_row, int inc_col)
            {
                int out=0,f=0;
                auto k=i,l=j;
                do 
                {
                    k+=inc_row;
                    l+=inc_col;
                    if(k<0 || k>=ROWS || l<0 || l>=COLS) break;
                    if(this->NewStates[k][l]==TYPE)
                        out++;
					else if (this->NewStates[k][l]==NO_PIECE)
						f++;
                    else break;
                } 
                while (true);
                return std::pair<int,int>(out,f);
            };
            for ( i = 0; i < ROWS; i++) 
                for ( j = 0; j < COLS; j++) 
                {
                    if (NewStates[i][j]==TYPE)
                    {
                        // try to find a new connections
						int OUTS[4]={traverse(1,0).first,traverse(0,1).first, traverse(1,1).first,traverse(1,-1).first};
						int FREE[4]={traverse(1,0).second,traverse(0,1).second, traverse(1,1).second,traverse(1,-1).second};
						for (int it=0; it<4;i++)
							if (FREE[it]+OUTS[it]<K) OUTS[it]=0;
                        std::sort(OUTS, OUTS+4);
                        _connected.push_back(OUTS[3]);
                    }
                }
            return _connected;
        };
        

        /* ====================  HEURISTICS     ======================================= */
		virtual float myconnections()
		{
            auto temp=connections();
            std::sort(temp.begin(), temp.end());
			auto val=*temp.rbegin();
            return float(val);
		};
		virtual float theirconnections()
		{
            auto temp=connections(OPPONENT_PIECE);
            std::sort(temp.begin(), temp.end());
			auto val=*temp.rbegin();
            return float(val);
		};
        virtual float count_connections()
        {
			auto my=myconnections();
			auto their=theirconnections();
            if (my>=K) return 4.0;
            else if (their>=K) return 0;
            else return(my+0.5);
        };
        /* ====================  INHERITED     ======================================= */
        //only need to alter these two.
        //in fact main_routine() can be left alone unless under special circumstances
        virtual void main_routine() override
        {
            while (!listen(1));
#if LOGGING
            f<<"Listened successfully"<<std::endl;
#endif
            IDSearch();
            update_frontier();
            auto m=Get_Move();
#if LOGGING
            f<<"Move get"<<std::endl;
#endif
            tell_move(m);
            while (listen(false))
            {
                IDSearch();
                update_frontier();
                m=Get_Move();
                tell_move(m);
            }
        };
        virtual float addheuristic() override
        {
            auto val=count_connections();
            return val;
        };


}; /* -----  end of class Smartplayer  ----- */


#endif
