#include "lsq.h"

short g_f;
t_file *g_file;

/*
** Error-management function
** Type: [1] - Printing undefined char e, which is ft_ls illegal option error
**       [0] - Perror() + name error
*/

int  ls_error(char e, char *s, short type)
{
    if (type)
    {
        ft_putstr_fd("ls: illegal option -- ", 2);
        write(2, &e, 1);
        ft_putstr_fd("\nusage: ls [-hlarptms1RG] [file ... ]", 2);
    }
    else
        write(2, "ft_ls : ", 8) ? perror(s) : 0;
    exit(0);
}

/*
** Function ls_help('-h' flag representing):
** Output represents program help info, after which one force exit().
*/
int ls_help(void)
{
    ft_printf("\t FT_LS(v1.0) HELP GUIDE:\n");
    ft_printf("ft_ls Program supports following flags: \n"
    "\f-l Long format, displaying Unix file types, permissions, number of "
    "hard links, owner, group, size, last-modified date and filename\n"
    "\f-a Lists all files in the given directory, including those whose names"
    " start with \".\" By default, these files are excluded from the list.\n"
    "\f-R Recursively lists subdirectories.\n"
    "\f-t Sort the list of files by modification time.\n"
    "\f-h Shows program help menu.\n"
    "\f-r Reverse the order of the sort to get reverse collating sequence "
    "oldest first\n"
    "\f-1 (The numeric digit one.) Force output to be one entry per line.\n"
    "\f-m Stream output format; list pathnames across the page, separated by"
    " a <comma> character followed by a <space> character.\n"
    "\f-s Indicate the total number of file system blocks\n"
    "\f-p Write a <slash> ( '/' ) after each filename if that file is a "
    "directory."
    "\f-G Turns on colour output format\n");

    exit(0);
}
/*
** Recursive function ls_flag_parse checks & activate flags in g_f:
**
** Variable /short g_f;/ uses for flag representation:
** Has size 2 bytes, where we can store all info about active flags
** { 12 13 14 15 | 11 10 9 8 | 7 6 5 4 | 3 2 1 0 } <- number of bit
** { 0  0  0  0  |  0  0 0 0 | 0 0 0 0 | 0 0 0 0 } <- bit representation
**
** Encrypting variable byte-codes:
** 0x0   -  (-''): If flags wasn't activated
**  -    -  (-h ): Display help usage and exit program
** 0x1   -  (-l ): Showing in long format
** 0x4   -  (-a ): Shows hidden files (like ./ || ../)
** 0x8   -  (-r ): Reverse output showing list
** 0x10  -  (-t ): Sorting by the time
** 0x20  -  (-m ): Coma-separated argument output
** 0x40  -  (-s ): Display file-size (in bytes) before filename
** 0x80  -  (-1 ): Semi-column output format
** 0x100 -  (-G ): Colored output format
** 0x200 -  (-p ): Write a '/' after filename if that file is a directory.
** 0x400 -  (-R ): Recursive subdirectories output
*/

void ls_flag_parse(char *s, int i)
{
    if (!s[i])
        return ;
    if (s[i] == 'h')
        ls_help();
    else if (s[i] == 'R')
        g_f &= 0x400;
    else if (s[i] == 'l')
        g_f &= 0x1;
    else if (s[i] == 'a')
        g_f &= 0x4;
    else if (s[i] == 'r')
        g_f &= 0x8;
    else if (s[i] == 't')
        g_f &= 0x10;
    else if (s[i] == 'm')
        g_f &= 0x20;
    else if (s[i] == 's')
        g_f &= 0x40;
    else if (s[i] == '1')
        g_f &= 0x80;
    else if (s[i] == 'G')
        g_f &= 0x100;
    else if (s[i] == 'p')
        g_f &= 0x200;
    else
        exit(ls_error(s[i], NULL, 1));
    ls_flag_parse(s, ++i);
}

/*
** Function ls_get_rights gets(writes) all rules from file,
** using getgrgid(gets GROUP id) and getpwuid(gets PASSWORD id):
** struct group {
**       char    *gr_name;     // group name
**       char    *gr_passwd;   // group passwd
**       gid_t   gr_gid;       // group id
**       char    **gr_mem;     // group member };
*/

void    ls_get_rights(t_file *f) {}

/*
** Function ls_time format created for formatting output date
** In case where, current date > than last modification date on 182 days || higher
** than we ls writes just month / day / year ("Mar 16  2016")
**                   else month / day / time ("Aug 22 02:35")
** ctime() returns in following format: (Mon Aug 13 08:23:14 2012)
** Www Mmm dd hh:mm:ss yyyy,        where Www is the weekday,
**                                  Mmm the month in letters,
**                                  dd the day of the month,
**                                  hh:mm:ss the time,
**                                  yyyy the year.
**
*/

char *ls_time_format(time_t secnds) //ToDo: Check for leaks && '\0' position
{
    time_t t_now;
    char   *t_file;
    char   *date;

    date = malloc(13);
    t_now = time(NULL);
    t_file = ctime(&secnds);
    ft_strncpy(date, t_file + 4, 6);
    if (secnds < (t_now - 182 * 24 * 3600))
    {   //substitute 182 years in seconds *= (x24(hours)x3600(seconds))
        ft_strncpy(date + 6, t_file + 19, 5);
        date[11] = '\0';
    }
    else
        ft_strncpy(date + 6, t_file + 10, 6);
    date[12] = '\0';
    return (date);
}

/*
**  Function ls_tree_parse works with stat struct (t_s inf; //watch typedef in header),
**    those gives info about input directory :
**    struct    stat {
**    dev_t     st_dev;         // ID of device containing file
**    ino_t     st_ino;         // Inode number
**    mode_t    st_mode;        // File type and mode
**    nlink_t   st_nlink;       // Number of hard links
**    uid_t     st_uid;         // User ID of owner
**    gid_t     st_gid;         // Group ID of owner
**    dev_t     st_rdev;        // Device ID (if special file)
**    off_t     st_size;        // Total size, in bytes
**    blksize_t st_blksize;     // Block size for filesystem I/O
**    blkcnt_t  st_blocks;      // Number of 512B blocks allocated
**    // Since Linux 2.6, the kernel supports nanosecond
**    //precision for the following timestamp fields.
**    struct timespec st_atim;  // Time of last access
**    struct timespec st_mtim;  // Time of last modification
**    struct timespec st_ctim;  // Time of last status change };
*/

void ls_tree_parse(char *name)
{
    t_s     inf;
    t_file *f;

    f = (t_file *)malloc(sizeof(t_file));
    if (lstat(name, &inf) < 0)
        ls_error(NULL, name, 0);
    f->name = ft_strdup(name);
    f->type = inf.st_mode;
    f->nlinks = inf.st_nlink;
    f->blocks = inf.st_blocks;
    f->tsize = inf.st_size;
    f->seconds = inf.st_mtimespec.tv_sec;
    f->nanosec = inf.st_mtimespec.tv_nsec;
    f->date = ls_time_format(f->seconds);
    f->next = g_file;
    g_file = f;
}

/*
** Main function:
** Read && Parse flags in input **av
** (No matter [-a -l] || [-al] )
** Trying to read objects
** Memory free
*/

int main(int ac, char **av)
{
    int     arg;

    arg = 1;
    g_f = 0;
    g_file = NULL;
    while (arg < ac && av[arg][0] == '-')
        ls_flag_parse(av[arg++], 0);
    while (arg < ac)
        ls_tree_parse(av[arg++]);
    //arg < ac ?
    //ls_flag_define(f);
    //ls_flag_parse(f);
    return (0);
}