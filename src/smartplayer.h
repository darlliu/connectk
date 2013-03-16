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
        float winning_seqs (movetype TYPE=MY_PIECE)
        {
            std::vector<float> _connected;
            _connected.reserve(ROWS*COLS);
            //for fast push
            int i,j;
            auto traverse=[this,&i,&j,&TYPE](int inc_row, int inc_col) -> float 
            {
                float out=1,out2=0;
                auto k=i,l=j;
                int cnts=0;
                bool flag=false;
                do 
                {
                    k+=inc_row;
                    l+=inc_col;
                    if (cnts>=K-1) break;
                    if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
                    else if (this->NewStates[k][l]==TYPE)
                    {
                        out++;
                        cnts=0;
                    }
                    else if (this->NewStates[k][l]==NO_PIECE)
                    {
                        out++;
                        cnts++;
                    }
                    else flag=true;
                } 
                while (!flag);
                k=i,l=j;
                flag=false;
                cnts=0;
                do 
                {
                    k-=inc_row;
                    l-=inc_col;
                    if (cnts>=K-1) break;
                    if(k<0 || k>=ROWS || l<0 || l>=COLS) flag=true;
                    else if (this->NewStates[k][l]==TYPE)
                    {
                        out++;
                        cnts=0;
                    }
                    else if (this->NewStates[k][l]==NO_PIECE)
                    {
                        out++;
                        cnts++;
                    }
                    else flag=true;
                } 
                while (!flag);
                if( out+out2-K-1>0 ) return out+out2-K-1;
                else return 0;
            };
            for ( i = 0; i < ROWS; i++) 
                for ( j = 0; j < COLS; j++) 
                {
                    if (NewStates[i][j]==TYPE)
                    {
                        // try to find a new connections
                        float OUTS=traverse(-1,1)+traverse(1,0)+traverse(0,1)+traverse(1,1);
                        _connected.push_back(OUTS);
                    }
                }
            float val=0;
            for (auto it: _connected)
            {
                val+=it;
            };
            return (val);
        };

        float spaces (movetype TYPE=MY_PIECE)
        {
            //return a naive estimate of the "spaces" occupied by one player
            float _spaces=1;
            //for fast push
            int i,j;
            auto traverse = [this, &i,&j, &TYPE] ()
            {
                float spaces=0;
                //for (unsigned idx=0; idx<depth; idx++)
                for (int ii=-1; ii<2;ii++)
                for (int jj=-1; jj<2; jj++)
                {
                    int k=i,l=j;
                    if (ii==0 && jj==0) continue;
                    k+=ii;
                    l+=jj;
                    if(k<0 || k>=ROWS || l<0 || l>=COLS) continue;
                    if (this->NewStates[k][l]==MY_PIECE\
                            ||this->NewStates[k][l]==NO_PIECE)
                        spaces+=1.0;
                }
                return spaces;
            };
            float pieces=0;
            for ( i = 0; i < ROWS; i++) 
                for ( j = 0; j < COLS; j++) 
                {
                    if (NewStates[i][j]==TYPE)
                    {
                        pieces+=1;
                        _spaces+=traverse();
                    }
                }
            _spaces/=pieces;
            return _spaces/9;
        };
        

        /* ====================  HEURISTICS     ======================================= */
		virtual float myconnections()
		{
			auto val=connections();
#if LOGGING3
            f<<"My connections is "<<val<<std::endl;
#endif
            return val;
		};
		virtual float theirconnections()
		{
            auto temp=connections(OPPONENT_PIECE);
#if LOGGING3
            f<<"Their connections is "<<temp<<std::endl;
#endif
            return temp;
		};
        virtual float count_connections()
        {
            //a naive heuristics that aims to achieve three goals:
            //1, decisively pick moves that result in immediate win
            //2, decisively avoid moves that result in immediate loss
            //3, weakly prefer moves that increase our max connection
			auto my=myconnections();
			auto their=theirconnections();
            if (my==100) return 100;
            else if (their==100) return -100;
            return (my-their)/K;
        };
        virtual float myspaces()
        {
            auto val=spaces();
#if LOGGING3
            f<<"my spaces is "<<val<<std::endl;
#endif
            return val;
        };
        virtual float my_seqs()
        {
            return winning_seqs();
        };
        virtual float their_seqs()
        {
            return winning_seqs(OPPONENT_PIECE);
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
            auto val1=count_connections();
            auto val2=myspaces();
            auto val3=my_seqs();
            auto val4=their_seqs();
#if LOGGING3
            f<<"val is  "<<val1<<std::endl;
            print_board();
#endif
            return val1+val2+0.5*val3-0.5*val4;
        };


}; /* -----  end of class Smartplayer  ----- */


#endif
