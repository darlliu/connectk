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
        std::vector<float> connections (movetype TYPE=MY_PIECE)
        {
            std::vector<float> _connected;
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
                return std::pair<float,float>((float)out,(float)f);
            };
            for ( i = 0; i < ROWS; i++) 
                for ( j = 0; j < COLS; j++) 
                {
                    if (NewStates[i][j]==TYPE)
                    {
                        // try to find a new connections
                        float OUTS[4]={traverse(-1,1).first,traverse(1,0).first,traverse(0,1).first,traverse(1,1).first}
                            , FREE[4]={traverse(-1,1).second,traverse(1,0).second,traverse(0,1).second,traverse(1,1).second};
						for (int it=0; it<4;it++)
							if (FREE[it]+OUTS[it]<K) OUTS[it]=0;
                            else OUTS[it]-=OUTS[it]/(1+FREE[it]);
                        std::sort(OUTS, OUTS+4);
                        _connected.push_back(OUTS[3]);
                    }
                }
            return _connected;
        };

        float spaces (movetype TYPE=MY_PIECE)
        {
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
            auto temp=connections();
            std::sort(temp.begin(), temp.end());
			auto val=*temp.rbegin();
#if LOGGING3
            f<<"My connections is "<<val<<std::endl;
#endif
            return float(val);
		};
		virtual float theirconnections()
		{
            auto temp=connections(OPPONENT_PIECE);
            std::sort(temp.begin(), temp.end());
			auto val=*temp.rbegin();
#if LOGGING3
            f<<"Their connections is "<<val<<std::endl;
#endif
            return float(val);
		};
        virtual float count_connections()
        {
			auto my=myconnections();
			auto their=theirconnections();
            if (my>=K) return 10;
            else if (their>=K) return 0;
            else return my-their;
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
