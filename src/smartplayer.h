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
        float connections (movetype TYPE=MY_PIECE)
        {
            std::vector<float> _connected;
            _connected.reserve(ROWS*COLS);
            //for fast push
            int i,j;
            auto traverse=[this,&i,&j,&TYPE](int inc_row, int inc_col) -> float 
            {
                float out=0,f1=0,f2=0;
                auto k=i,k2=i, l2=j, l=j;
                bool flag1, flag2;
                do 
                {
                    flag1=flag2=false;
                    k+=inc_row;
                    l+=inc_col;
                    k2-=inc_row;
                    l2-=inc_col;
                    if(k<0 || k>=ROWS || l<0 || l>=COLS) flag1=true;
                    else if(this->NewStates[k][l]==TYPE)
                        out++;
					else if (this->NewStates[k][l]==NO_PIECE)
						f1++;
                    else flag1=true;
                    if(k2<0 || k2>=ROWS || l2<0 || l2>=COLS) flag2=true;
                    else if(this->NewStates[k2][l2]==TYPE)
                        out++;
					else if (this->NewStates[k2][l2]==NO_PIECE)
						f2++;
                    else flag2=true;
                } 
                while (!flag1 || !flag2);
                if (f1 * f2>0)
                {
                    if (out>=K-2)
                        return (float)K;
					else return out;
                }
                else if (f1 || f2)
                {
                    if (out >= K-1)
                        return (float)K;
                    else if (out+f1+f2<K)
                        return 0.0;
                    else return out;
                }
                else 
                {
                    if (out>=K) return (float) K;
                    else return 0;
                }
				return 0;
            };
            for ( i = 0; i < ROWS; i++) 
                for ( j = 0; j < COLS; j++) 
                {
                    if (NewStates[i][j]==TYPE)
                    {
                        // try to find a new connections
                        float OUTS[4]={traverse(-1,1),traverse(1,0),traverse(0,1),traverse(1,1)};
                        std::sort(OUTS, OUTS+4);
#if LOGGING3
            f<<"One Value is "<<OUTS[3]<<std::endl;
#endif
                        _connected.push_back(OUTS[3]);
                    }
                }
			if (_connected.size()==0) return 0; 
            std::sort(_connected.begin(), _connected.end());
            return (*_connected.rbegin());
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
            if (my>=K && their >=K )
            {
                if (whose_turn) return 10;
                else return 0;
            }
            else if (my>=K) return 10;
            else if (their>=K) return 0;
            else if (my>their) return 3;
            else return 1;
        };
        virtual float myspaces()
        {
            auto val=spaces();
#if LOGGING3
            f<<"my spaces is "<<val<<std::endl;
#endif
            return val;
        }
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
            if (val1==0) return 0;
            else if (val1==10) return 10;
            auto val2=myspaces();
            return val1+val2;
        };


}; /* -----  end of class Smartplayer  ----- */


#endif
