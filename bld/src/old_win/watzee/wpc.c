#include <windows.h>
#include <string.h>
#include "watzee.h"

static BOOL AchievedGoal( short );
static short DecideGoal( void );
static void PickDiceToRoll( short );
static short GetDieCountValue( short );

static BOOL     roll_die[5];         /*  indicates which dice the computer
                                         wants to roll again                */
static short    goal;

/*
 * PCPlay -- simple logic to allow the computer to control a player;
 *           First, based on the values of the dice, determine a goal, such
 *           as a Full House, that has already been achieved or shouldn't
 *           be too hard to get, and then decide which dice to roll again
 *           to achieve that goal, if necassary
 */

extern BOOL PCPlay( HWND hwnd )
/*****************************/
{
    BOOL            roll_again;
    BOOL            achieved_goal;
    BOOL            done_turn;
    short           old_goal;
    static short    zero_out[] = { WATZEE, ACES, FULL_HOUSE, LARGE_STRAIGHT,
                                   TWOS, FOUR_KIND, THREE_KIND, SMALL_STRAIGHT,
                                   THREES, FOURS, FIVES, SIXES };
    short           i;

    roll_again = TRUE;
    done_turn = FALSE;
    memset( roll_die, FALSE, 5 );
    if( CurrentRoll < 2 || goal == 0 ) {
        goal = DecideGoal();                /*  what shall we roll for?  */
    }
    achieved_goal = AchievedGoal( goal );   /*  did we get what we want yet? */
    if( achieved_goal ) {
        roll_again = FALSE;
        switch( goal ) {                    /*  even though we do have our  */
        case ACES :                         /*  goal, we can still roll     */
        case TWOS :                         /*  some of the dice to         */
        case THREES :                       /*  maximize our score          */
        case FOURS :
        case FIVES :
        case SIXES :
            if( DiceInfo.count[goal] < 5 ) {
                roll_again = TRUE;
            }
            break;
        case THREE_KIND :
            if( DiceInfo.sum - GetDieCountValue( 3 ) * 3 < 9 ) {
                roll_again = TRUE;
            }
            break;
        case FOUR_KIND :
            if( GetDieCountValue( 1 ) < 4 ) {
                roll_again = TRUE;
            }
            break;
        case CHANCE :
            if( DiceInfo.sum < 20 ) {
                roll_again = TRUE;
            }
            break;
        }
    }
    if( !roll_again || (CurrentRoll == 2 && achieved_goal) ) {
       /* got our goal so score it  */
        SendMessage( hwnd, WMW_SCORE_CHECK, goal, 0 );
        SendMessage( hwnd, WMW_OK, 0, 0 );
        done_turn = TRUE;
    } else if( CurrentRoll == 2 && !achieved_goal ) {
       /*
        * if we ran out of rolls and did not get what we wanted,
        * perhaps we can score something else instead
        */
        if( goal != 0 && CurrentTurn < 12 ) {
            old_goal = goal;
            Player[CurrentPlayer][goal] = 0;
            goal = DecideGoal();
            Player[CurrentPlayer][old_goal] = UNDEFINED;
            achieved_goal = AchievedGoal( goal );
        }
        if( !achieved_goal ) {      /*  all hope is lost, time to zero out  */
            i = 0;                  /*  something                           */
            while( Player[CurrentPlayer][zero_out[i]] != UNDEFINED ) i++;
            goal = zero_out[i];
        }
        SendMessage( hwnd, WMW_SCORE_CHECK, goal, 0 );
        SendMessage( hwnd, WMW_OK, 0, 0 );
        done_turn = TRUE;
    } else {
       /*  roll some of the dice again  */
        PickDiceToRoll( goal );
        for( i = 0; i < 5; i++ ) {
            if( roll_die[i] ) {
                SendMessage( hwnd, WMW_DIE_CHECK, IDW_DICE1+i, 0 );
            }
        }
        SendMessage( hwnd, WMW_ROLL, 0, 0 );
    }
    return( done_turn );
}



/*
 * DecideGoal -- based on the values of the dice, and what we have already
 *               scored, decide what we should go after next
 */

static short DecideGoal( void )
/*****************************/
{
    short   goal;
    short   top_goal;
    short   bottom_goal;
    short   top_goal_value;
    short   bottom_goal_value;
    short   pc;
    BOOL    got_three_kind;
    BOOL    got_four_kind;
    BOOL    got_full_house;
    BOOL    got_small_straight;
    BOOL    got_large_straight;
    short   i;

    pc = CurrentPlayer;
    goal = 0;
    top_goal = 0;
    bottom_goal = 0;
    top_goal_value = 0;
    bottom_goal_value = 0;
    got_three_kind = DiceInfo.got_three_kind;
    got_four_kind = DiceInfo.got_four_kind;
    got_full_house = DiceInfo.got_full_house;
    got_small_straight = DiceInfo.got_small_straight;
    got_large_straight = DiceInfo.got_large_straight;
    for( i = ACES; i <= SIXES; i++ ) {
        if( DiceInfo.count[i] >= 3 && Player[pc][i] == UNDEFINED ) {
            top_goal = i;
        }
    }
    if( top_goal == 0 ) {
        for( i = SIXES; i >= ACES; i-- ) {
            if( DiceInfo.count[i] == 2 && Player[pc][i] == UNDEFINED ) {
                top_goal = i;
                break;
            }
        }
        if( top_goal == 0 ) {
            for( i = ACES; i <= SIXES; i++ ) {
                if( DiceInfo.count[i] && Player[pc][i] == UNDEFINED ) {
                    top_goal = i;
                    break;
                }
            }
        }
    }
    if( top_goal != 0 ) {
        top_goal_value = top_goal * DiceInfo.count[top_goal];
        if( DiceInfo.count[top_goal] >= 3 ) {
            top_goal_value *= 3;
        } else if( DiceInfo.count[top_goal] == 2 ) {
            top_goal_value *= 2;
        }
    }
    if( DiceInfo.got_watzee && Player[pc][WATZEE] == UNDEFINED ) {
        bottom_goal = WATZEE;
        bottom_goal_value = 100;
    } else if( got_large_straight && Player[pc][LARGE_STRAIGHT] == UNDEFINED ) {
        bottom_goal = LARGE_STRAIGHT;
        bottom_goal_value = 40;
    } else if( got_four_kind && Player[pc][FOUR_KIND] == UNDEFINED ) {
        bottom_goal = FOUR_KIND;
        bottom_goal_value = DiceInfo.sum;
        if( bottom_goal_value < 20 ) {
            bottom_goal_value /= 2;
        }
    } else if( (got_small_straight || got_large_straight)
             && Player[pc][SMALL_STRAIGHT] == UNDEFINED ) {
        bottom_goal = SMALL_STRAIGHT;
        bottom_goal_value = 30;
    } else if( got_full_house && Player[pc][FULL_HOUSE] == UNDEFINED ) {
        bottom_goal = FULL_HOUSE;
        bottom_goal_value = 25;
    } else if( (got_three_kind || got_four_kind)
             && Player[pc][THREE_KIND] == UNDEFINED ) {
        bottom_goal = THREE_KIND;
        bottom_goal_value = DiceInfo.sum;
        if( bottom_goal_value < 18 ) {
            bottom_goal_value /= 2;
        }
    }
    if( bottom_goal == 0 ) {
        if( Player[pc][WATZEE] != 0 ) {
            bottom_goal = WATZEE;
            bottom_goal_value = 2;
        }
        if( Player[pc][CHANCE] == UNDEFINED ) {
            bottom_goal = CHANCE;
            bottom_goal_value = DiceInfo.sum;
        }
        if( Player[pc][FULL_HOUSE] == UNDEFINED ) {
            bottom_goal = FULL_HOUSE;
            bottom_goal_value = 2;
        }
        if( Player[pc][FOUR_KIND] == UNDEFINED ) {
            bottom_goal = FOUR_KIND;
            bottom_goal_value = 4;
        }
        if( Player[pc][THREE_KIND] == UNDEFINED ) {
            bottom_goal = THREE_KIND;
            bottom_goal_value = 5;
        }
        if( Player[pc][LARGE_STRAIGHT] == UNDEFINED ) {
            bottom_goal = LARGE_STRAIGHT;
            bottom_goal_value = 4;
        }
        if( Player[pc][SMALL_STRAIGHT] == UNDEFINED ) {
            bottom_goal = SMALL_STRAIGHT;
            bottom_goal_value = 5;
        }
        if( got_three_kind && Player[pc][FULL_HOUSE] == UNDEFINED ) {
            bottom_goal = FULL_HOUSE;
            bottom_goal_value = 10;
        }
        if( got_three_kind && Player[pc][FOUR_KIND] == UNDEFINED ) {
            bottom_goal = FOUR_KIND;
            bottom_goal_value = 8;
        }
        if( got_small_straight && Player[pc][LARGE_STRAIGHT] == UNDEFINED ) {
            bottom_goal = LARGE_STRAIGHT;
            bottom_goal_value = 20;
        }
        if( got_four_kind && Player[pc][WATZEE] != 0 ) {
            bottom_goal = WATZEE;
            bottom_goal_value = 20;
        }
    }
    if( top_goal != 0 && bottom_goal != 0 ) {
        goal = top_goal;
        if( top_goal_value < bottom_goal_value ) {
            goal = bottom_goal;
        }
    } else if( top_goal != 0 && bottom_goal == 0 ) {
        goal = top_goal;
    } else if( top_goal == 0 && bottom_goal != 0 ) {
        goal = bottom_goal;
    }
    return( goal );
}



/*
 * PickDiceToRoll -- determine which dice should be rolled again in order
 *                   to achieve the specified goal
 */

static void PickDiceToRoll( short goal )
/**************************************/
{
    short   die_value;
    short   lower_bound;
    short   upper_bound;
    short   n;
    short   i;
    short   j;

    switch( goal ) {
    case 0 :
        memset( roll_die, TRUE, 5 );
        break;
    case ACES :
    case TWOS :
    case THREES :
    case FOURS :
    case FIVES :
    case SIXES :
        for( i = 0; i < 5; i++ ) {
            if( Dice[i].value != goal ) {
                roll_die[i] = TRUE;
            }
        }
        break;
    case THREE_KIND :
    case FOUR_KIND :
        n = 3;
        if( goal == FOUR_KIND ) {
            n = 4;
        }
        for( i = n; i >= 1; i-- ) {
            die_value = GetDieCountValue( i );
            if( die_value ) break;
        }
        for( i = 0; i < 5; i++ ) {
            if( Dice[i].value != die_value ) {
                roll_die[i] = TRUE;
            }
        }
        break;
    case FULL_HOUSE :
        if( GetDieCountValue( 5 ) != 0 ) {
            roll_die[0] = TRUE;
            roll_die[1] = TRUE;
        } else if( GetDieCountValue( 4 ) != 0 ) {
            die_value = GetDieCountValue( 1 );
            for( i = 0; i < 5; i++ ) {
                if( Dice[i].value == die_value ) {
                    roll_die[i] = TRUE;
                    break;
                }
            }
        } else if( GetDieCountValue( 3 ) != 0 ) {
            die_value = GetDieCountValue( 1 );
            if( die_value != 0 ) {
                for( i = 0; i < 5; i++ ) {
                    if( Dice[i].value == die_value ) {
                        roll_die[i] = TRUE;
                        break;
                    }
                }
            }
        } else {
            die_value = GetDieCountValue( 2 );
            if( die_value ) {
                for( i = 0; i < 5; i++ ) {
                    if( Dice[i].value != die_value ) {
                        roll_die[i] = TRUE;
                    }
                }
            } else {
                memset( roll_die, TRUE, 4 );
            }
        }
        break;
    case SMALL_STRAIGHT :
    case LARGE_STRAIGHT :
        if( GetDieCountValue( 5 ) != 0 ) {
            memset( roll_die, TRUE, 4 );
        } else {
            for( i = ACES; i <= SIXES; i++ ) {
                if( DiceInfo.count[i] > 1 ) {
                    n = 1;
                    for( j = 0; j < 5; j++ ) {
                        if( Dice[j].value == i ) {
                            roll_die[j] = TRUE;
                            n++;
                            if( n == DiceInfo.count[i] ) break;
                        }
                    }
                }
            }
            lower_bound = ACES;
            upper_bound = FOURS;
            if( goal == LARGE_STRAIGHT ) {
                upper_bound = FIVES;
            }
            for( i = upper_bound-lower_bound; i >= 1; i-- ) {
                do {
                    n = 0;
                    for( j = lower_bound; j <= upper_bound; j++ ) {
                        if( DiceInfo.count[j] ) {
                            n++;
                        }
                    }
                    if( n == i ) break;
                    lower_bound++;
                    upper_bound++;
                } while( upper_bound <= SIXES );
                if( n == i ) break;
                lower_bound = ACES;
                upper_bound = FOURS;
                if( goal == LARGE_STRAIGHT ) {
                    upper_bound = FIVES;
                }
            }
            for( i = 0; i < 5; i++ ) {
                if( Dice[i].value<lower_bound || Dice[i].value>upper_bound ) {
                    roll_die[i] = TRUE;
                }
            }
        }
        break;
    case WATZEE :
        for( i = 4; i >= 1; i-- ) {
            die_value = GetDieCountValue( i );
            if( die_value ) break;
        }
        for( i = 0; i < 5; i++ ) {
            if( Dice[i].value != die_value ) {
                roll_die[i] = TRUE;
            }
        }
        break;
    case CHANCE :
        for( i = 0; i < 5; i++ ) {
            if( Dice[i].value < 4 ) {
                roll_die[i] = TRUE;
            }
        }
        break;
    }
}



/*
 * AchievedGoal -- determine whether the specified goal has been achieved
 */

static BOOL AchievedGoal( short goal )
/*----------------------------------*/
{
    BOOL    achieved_goal;

    switch( goal ) {
    case 0 :
        achieved_goal = FALSE;
        break;
    case ACES :
    case TWOS :
    case THREES :
    case FOURS :
    case FIVES :
    case SIXES :
        achieved_goal = DiceInfo.count[goal] != 0;
        break;
    case THREE_KIND :
        achieved_goal = DiceInfo.got_three_kind;
        break;
    case FOUR_KIND :
        achieved_goal = DiceInfo.got_four_kind;
        break;
    case FULL_HOUSE :
        achieved_goal = DiceInfo.got_full_house;
        break;
    case SMALL_STRAIGHT :
        achieved_goal = DiceInfo.got_small_straight;
        break;
    case LARGE_STRAIGHT :
        achieved_goal = DiceInfo.got_large_straight;
        break;
    case WATZEE :
        achieved_goal = DiceInfo.got_watzee;
        break;
    case CHANCE :
        achieved_goal = TRUE;
        break;
    }
    return( achieved_goal );
}



/*
 * GetDieCountValue -- return the value of a die of which we have exactly 'n'
 *                     return 0 if there is no die value which appears 'n'
 *                     times
 */

static short GetDieCountValue( short n )
/*------------------------------------*/
{
    short   die_value;
    short   i;

    die_value = 0;
    for( i = SIXES; i >= ACES; i-- ) {
        if( DiceInfo.count[i] == n ) {
            die_value = i;
            break;
        }
    }
    return( die_value );
}
