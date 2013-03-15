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
            /*
             *if (my>=K && their >=K )
             *{
             *    //if (whose_turn)
             *        //if (my>their)return 10;
             *    return 0;
             *}
             */
            return my-their;
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
            auto val2=myspaces();
#if LOGGING3
            f<<"val is  "<<val1<<std::endl;
            print_board();
#endif
            return val1+val2;
        };


}; /* -----  end of class Smartplayer  ----- */


#endif
