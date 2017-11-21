#ifndef FT_LS_LSQ_H
# define FT_LS_LSQ_H

# include <stdlib.h> //for write
# include <dirent.h>
# include <time.h> //for time func()
# include <sys/stat.h> //for struct stat
# include <stdio.h>
# include "libft/libft.h"
# include "libft/ft_printf/ft_printf.h"

/*
** Variable /short f;/ uses for flag representation:
** Has size 2 bytes, where we can store all info about active flags
** { 12 13 14 15 | 11 10 9 8 | 7 6 5 4 | 3 2 1 0 } <- number of bit
** { 0  0  0  0  |  0  0 0 0 | 0 0 0 0 | 0 0 0 0 } <- bit representation
** Encrypting variable byte-codes:
** 0 -  if any flags was activated
** 1 -  (-h): Display help usage
** 2 -  (-l): Showing in long format
** 3 -  (-a): Shows hidden files (like ./ || ../)
** 4 -  (-r): Reverse output showing list
** 5 -  (-t): Sorting by the time
** 6 -  (-m): Coma-separated argument output
** 7 -  (-s): Display file-size (in bytes) before filename
** 8 -  (-1): Semi-column output format
** 9 -  (-G): Colored output format
** 10-  (-R): Recursive subdirectories output
*/

//typedef struct      s_flag{
//    unsigned char   f;      //any flags is on
//    unsigned char   l;      //(-l): list
//    unsigned char   a;      //(-a): hidden files
//    unsigned char   r;      //(-r): reverse
//    unsigned char   t;      //(-t): time-sorted
//    unsigned char   mo;     //(-1): (minus one) - semi column format
//    unsigned char   m;      //(-m): semi string split by commas
//    unsigned char   h;      //(-h): help
//    unsigned char   c;      //(-C): colored
//    unsigned char   s;      //(-s): size in bytes before file name
//    unsigned char   re;     //(-R): recursive output
//}                   t_flag;


typedef struct      s_file{
    char            *name;
    char            *path;
    mode_t           type;
    char            *right;
    char            *date;
    off_t           tsize;
    nlink_t         nlinks;
    blkcnt_t        blocks;
    time_t          seconds;
    long            nanosec;

    struct s_file   *next;


}                   t_file;

typedef struct stat t_s;

#endif //FT_LS_LSQ_H
