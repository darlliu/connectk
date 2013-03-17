#ifndef THREATS_H
#define THREATS_H
#include "smartplayer.h"
/*
 * =====================================================================================
 *        Class:  Smartplayer
 *  Description:  As described
 * =====================================================================================
 */

class threats : public Smartplayer
{
    public:
		/* Running tally of threats for both players */
	int threat_counts[MAX_CONNECT_K + 1][2];
	myline line[];


		threats() {};

int threatBits(int threat, int type)
/* Bit pack the threat value */
{
	int connect_k = 6;
	int place_p = 2;
	int BITS_PER_THREAT = 6;
        if (threat < 1)
                return 0;

        /* No extra value for building sequences over k - p unless it is
           enough to win */
        if (b->turn == type && connect_k - threat <= b->moves_left)
                threat = connect_k - place_p + 1;
        else if (threat >= connect_k - place_p)
                threat = connect_k - place_p - (type == b->turn);

        return 1 << ((threat - 1) * BITS_PER_THREAT);
}

void threatMark(int i, int threat, int type)
{
        int j, index = 0;
		int connect_k = 6;
		int place_p = 2;

        if (threat <= 0)
                return;

        /* No extra value for building sequences over k - p unless it is
           enough to win */
		//this->NewStates.
        //if (b->turn == type && connect_k - threat <= b->moves_left)
		if ( == type && connect_k - threat <= b->moves_left)
                threat = connect_k - place_p + 1;
        else if (threat >= connect_k - place_p)
                threat = connect_k - place_p - (type == b->turn);

        /* Do not mark if this threat is dominated by a preceeding threat;
           Likewise supress any smaller threats */
        for (j = i; j >= 0 && j > i - connect_k; j--)
                if (line[j].threat[0] > threat)
                        return;
                else if (line[j].threat[0] < threat) {
                        line[j].threat[0] = 0;
                        line[j].threat[1] = 0;
                }

        /* Store up to two threats per tile in the line */
        if (line[i].threat[index])
                index++;
        line[i].threat[index] = threat;
        line[i].turn[index] = type;
}


int threatWidth(int x, int y, int dx, int dy,
                         int *ptype, int *pdouble)
{
        int min, max, count = 0;
        int p, type = 0;
		int connect_k = 6;

        /* Check if this tile is empty */
        //p = piece_at(b, x, y);
		p = this->NewStates[x][y];
        //if (!piece_empty(p))
               // return 0;
		if ( p == NO_PIECE ) return 0;

        /* Push forward the max and find the window type */
        for (max = 1; max < connect_k; max++) {
                //p = piece_at(b, x + dx * max, y + dy * max);
			p = this->NewStates[x + dx * max][ y + dy * max];
                //if (p == PIECE_ERROR)
                       // break;
                if (p != NO_PIECE ) {
                        if (type == NO_PIECE)
                                type = p;
                        else if (type != p)
                                break;
                        count++;
                }
        }
        max--;

        /* Try to push the entire window back */
        for (min = -1; min > -connect_k; min--) {
                //p = piece_at(b, x + dx * min, y + dy * min);
				p = this->NewStates[x + dx * min][ y + dy * min];
                //if (p == PIECE_ERROR || piece_empty(p))
                        //break;
                if (type == NO_PIECE)
                        type = p;
                else if (type != p)
                        break;
                if (max - min > connect_k - 1) {
                        //p = piece_at(b, x + dx * max, y + dy * max);
					    p = this->NewStates[x + dx * max][ y + dy * max];
                        if (p == type)
                                count--;
                        max--;
                }
                count++;
        }
        min++;

        /* Push back min if we haven't formed a complete window, this window
           can't be a double */
        if (max - min < connect_k - 1) {
                for (min--; min > max - connect_k; min--) {
                       // p = piece_at(b, x + dx * min, y + dy * min);
						p = this->NewStates[x + dx * min][y + dy * min];
                        //if (p == PIECE_ERROR)
                                //break;
                        if ( p != NO_PIECE) {
                                if (type != p)
                                        break;
                                if (type == NO_PIECE)
                                        type = p;
                                count++;
                        }
                }
                *pdouble = 0;
                min++;
        }

        *ptype = type;
        if (max - min >= connect_k - 1)
                return count;
        return 0;
};

int placeThreat (int x, int y, int type)
{
	int board_stride = 8;
	b->data[(y + 1) * board_stride + x + 1] = type;
};

int threatMatch(int x, int y, int dx, int dy)
{
        int i;
        int weight = 0;
		int board_size = 6;

        /* Mark the maximum threat for each */
        for (i = 0; x >= 0 && x < board_size && y >= 0 && y < board_size; i++) {
                int count[2], tmp, double_threat = 1;
                int type[2];

                count[0] = threatWidth(x, y, dx, dy, type, &double_threat);
                count[1] = threatWidth(x, y, -dx, -dy, type + 1, &double_threat);
                if (count[1] > count[0]) {
                        tmp = count[1];
                        count[1] = count[0];
                        count[0] = tmp;
                        tmp = type[1];
                        type[1] = type[0];
                        type[0] = tmp;
                }
                line[i].threat[0] = 0;
                line[i].threat[1] = 0;
                threatMark(i, count[0], type[0]);
                if (double_threat)
                        threatMark(i, count[1], type[1]);
                x += dx;
                y += dy;
        }

        /* Commit stored line values to the board */
        x -= dx;
        y -= dy;
        for (i--; x >= 0 && x < board_size && y >= 0 && y < board_size; i--) {
                int bits[2];
                int p;

                bits[0] = threatBits(line[i].threat[0], line[i].turn[0]);
                bits[1] = threatBits(line[i].threat[1], line[i].turn[1]);
                //p = piece_at(b, x, y);
				p = NewStates[x][y];
				
                //if (piece_empty(p) && line[i].threat[0]) {
				if (p == NO_PIECE && line[i].threat[0]) {
                        threat_counts[line[i].threat[0]][line[i].turn[0] - 1]++;
                        if (line[i].threat[1])
                                threat_counts[line[i].threat[1]]
                                             [line[i].turn[1] - 1]++;
                        if (p >= PIECE_THREAT0)
                                placeThreat(b, x, y, p - PIECE_THREAT0 +
                                             bits[0] + bits[1]);
                        else
                                placeThreat(b, x, y, bits[0] + bits[1]);
                }
                if (b->turn != line[i].turn[0])
                        bits[0] = -bits[0];
                if (b->turn != line[i].turn[1])
                        bits[1] = -bits[1];
                weight += bits[0] + bits[1];
                x -= dx;
                y -= dy;
        }

        return weight;
};

float threats(movetype TYPE)
{

};


};


#endif