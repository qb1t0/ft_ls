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
        ft_putstr_fd("\nusage: ls [-hlarptms1RG] [file ... ]\n", 2);
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
**  Function ls_set_ftype() sets the file type
**  Takes info from stat.st_mode,
**   where high 4 bits represents file type()
**
**  from sys/stat.h :
**  #define S_IFIFO 0x1000  // fifo
**  #define S_IFCHR 0x2000  // character special
**  #define S_IFDIR 0x4000  // directory
**  #define S_IFBLK 0x6000  // block special
**  #define S_IFREG 0x8000  // regular
**  #define S_IFLNK 0xA000  // symbolic link
*/
void    ls_set_ftype(t_file *f, t_s info) //ToDo: Check if readlink need malloc for f->link
{
    f->ftype = 'x';
    if (f->mode & S_IFREG)
        f->ftype = '-';
    else if (f->mode & S_IFBLK)
        f->ftype = 'b';
    else if (f->mode & S_IFCHR)
        f->ftype = 'c';
    else if (f->mode & S_IFDIR)
        f->ftype = 'd';
    else if (f->mode & S_IFIFO)
        f->ftype = 'p';
    else if (f->mode & S_IFLNK)
        f->ftype = 'l';
    else if (f->mode & S_IFSOCK)
        f->ftype = 's';
    if (f->mode & S_IFBLK || f->mode & S_IFCHR)
    {
        f->mjr = major(info.st_rdev);
        f->min = minor(info.st_rdev);
    }
    else if (f->mode & S_IFLNK)
        readlink(f->wayback, f->link, 256);
}

/*
** Function ls_set_rights gets(writes) all rules from file,
** using getgrgid(gets GROUP id) and getpwuid(gets PASSWORD id):
**
** struct   group {
**  char    *gr_name;     // group name
**  char    *gr_passwd;   // group passwd
**  gid_t   gr_gid;       // group id
**  char    **gr_mem;     // group member
**  };
**
** struct passwd {
**  char  *pw_name;       // user name
**  char  *pw_passwd;     // user password
**  uid_t pw_uid;         // user id
**  gid_t pw_gid;         // group id
**  char  *pw_gecos;      // user info
**  char  *pw_dir;        // home directory
**  char  *pw_shell;      // software shell
**  };
*/

void    ls_set_rights(t_file *f, t_s st)
{
    struct passwd *s_p;
    struct group  *s_g;

    s_p = getpwuid(st.st_uid);
    s_g = getgrgid(st.st_gid);
    f->right = ft_strnew(9);
    f->userid = s_p ? s_p->pw_name : ft_itoa(st.st_uid);
    f->groupid = s_g ? s_g->gr_name : ft_itoa(st.st_gid);

    /*
    ** The value of st_mode(f->mode) has to be interpreted by bits:
    ** The low 12 bits are the file access permissions, sets with chmod.
    ** Each bit represents one file permission. The high 4 bits are the file type.
    ** The low 12 bits of the 16-bit number 16877 would be 000111101101. This combination means:
    ** ---rwxr-xr-x (read, write, execute for the owner of the file; read and execute for others).
    ** from sys/stat.h :
    ** #define S_IRWXU 00700  // read, write, execute:  owner
    ** #define S_IRUSR 00400  // read permission:       owner
    ** #define S_IWUSR 00200  // write permission:      owner
    ** #define S_IXUSR 00100  // execute permission:    owner
    ** #define S_IRWXG 00070  // read, write, execute:  group
    ** #define S_IRGRP 00040  // read permission:       group
    ** #define S_IWGRP 00020  // write permission:      group
    ** #define S_IXGRP 00010  // execute permission:    group
    ** #define S_IRWXO 00007  // read, write, execute:  other
    ** #define S_IROTH 00004  // read permission:       other
    ** #define S_IWOTH 00002  // write permission:      other
    ** #define S_IXOTH 00001  // execute permission:    other
    */

    f->right[0] = (f->mode & S_IRUSR) ? (char)'r' : (char)'-';
    f->right[1] = (f->mode & S_IWUSR) ? (char)'w' : (char)'-';
    f->right[2] = (f->mode & S_IXUSR) ? (char)'x' : (char)'-';
    f->right[3] = (f->mode & S_IRGRP) ? (char)'r' : (char)'-';
    f->right[4] = (f->mode & S_IWGRP) ? (char)'w' : (char)'-';
    f->right[5] = (f->mode & S_IXGRP) ? (char)'x' : (char)'-';
    f->right[6] = (f->mode & S_IROTH) ? (char)'r' : (char)'-';
    f->right[7] = (f->mode & S_IWOTH) ? (char)'w' : (char)'-';
    f->right[8] = (f->mode & S_IXOTH) ? (char)'x' : (char)'-';
    if (f->mode & S_ISUID)
        f->right[2] = (f->right[2] == '-') ? (char)'S' : (char)'s';
    if (f->mode & S_ISGID)
        f->right[5] = (f->right[5] == '-') ? (char)'S' : (char)'s';
    if (f->mode & S_ISVTX)
        f->right[8] = (f->right[8] == '-') ? (char)'T' : (char)'t';
}

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
    {
        /* substitute 182 years in seconds *= ( x 24(hours) x 3600(seconds)) */
        ft_strncpy(date + 6, t_file + 19, 5);
        date[11] = '\0';
    }
    else
        ft_strncpy(date + 6, t_file + 10, 6);
    date[12] = '\0';
    return (date);
}

/*
** Function ls_set_default() setting default fields values :
** Initializing by 0 all values in created struct t_file *f
*/

void    ls_set_default(t_file *f)
{
    f->min = 0;
    f->mjr = 0;
    f->acl = 0;
    f->name = 0;
    f->mode = 0;
    f->link = 0;
    f->date = 0;
    f->ftype = 0;
    f->right = 0;
    f->tsize = 0;
    f->nlinks = 0;
    f->blocks = 0;
    f->userid = 0;
    f->groupid = 0;
    f->nanosec = 0;
    f->seconds = 0;
    f->wayback = 0;
}

/*
**  Function ls_object_parse works with stat struct (t_s inf; //watch typedef in header),
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
**    // precision for the following timestamp fields.
**    struct timespec st_atim;  // Time of last access
**    struct timespec st_mtim;  // Time of last modification
**    struct timespec st_ctim;  // Time of last status change };
*/

void ls_object_parse(char *name, char *wayback)
{
    t_s     inf;
    t_file  *f;

    f = (t_file *)malloc(sizeof(t_file));
    ls_set_default(f);
    if (lstat(name, &inf) < 0)
        ls_error(NULL, name, 0);
    f->name = ft_strdup(name);
    f->wayback = wayback ? wayback : ft_strdup(name);
    f->mode = inf.st_mode;
    f->nlinks = inf.st_nlink;
    f->blocks = inf.st_blocks;
    f->tsize = inf.st_size;
    f->seconds = inf.st_mtimespec.tv_sec;
    f->nanosec = inf.st_mtimespec.tv_nsec;
    f->date = ls_time_format(f->seconds);
    ls_set_rights(f, inf);
    ls_set_ftype(f, inf);
    (f->mode & S_IFLNK) ? lstat(f->link, &inf) : 0;
    f->next = g_file;
    g_file = f;
}

/*
** Function ls_subtree_parse reads "char *way" sub-directory
** starts for "."(when no ls args) or for recursive sub-directories bypass
**
** - Using opendir() function , which opens a directory stream corresponding to the
** directory name(char *way), and returns a pointer to the directory stream.
** The stream is positioned at the first entry in the directory.
**
** - Using readdir() function , which returns a pointer to a dirent structure,
** representing the next directory entry in the directory stream pointed to by dirp.
** It returns NULL on reaching the end of the directory stream or if an error occurred.
**
** Meaning: for each access to readdir(), it returns next file placed in "char *way"
** Also, it uses d_name to wrote file name in those. (size: 256 || 260 bytes)
**
** from dirrent.h :
** struct dirent {
**    ino_t          d_ino;       // inode number
**    off_t          d_off;       // offset to the next dirent
**    unsigned short d_reclen;    // length of this record
**    unsigned char  d_type;      // type of file
**    char           d_name[256]; // filename
** };
*/

void ls_subtree_parse(char *way)
{
    struct dirent   *d;
    DIR             *curr_d;
    char            *prev;

    if (!(curr_d = opendir(way)))
        ls_error(NULL, way, 0);
    prev = ft_strjoin(way, "/");
    while ((d = readdir(curr_d)))
        ls_object_parse(d->d_name, ft_strjoin(prev, d->d_name));
    if (closedir(curr_d) < 0)
        ls_error(NULL, way, 0);
}

short   ls_object_cmp(t_file *f1, t_file *f2)
{

}

/*
** Function ls_object_sort() :
** Sorting output objects order in output list
** ACL bonus implementation, using listxattr() && getxattr()
** ToDo: add more description here
*/

void ls_object_sort(void)
{
    t_file  *buf;
    char    *check;
    char    list[256];

    buf = g_file;
    if (!buf)
        exit(ls_error(NULL, " ", 0));
    while (buf->next && listxattr(buf->wayback, list, 256, 0))
    {
        if (g_f & 0x1)
            buf->acl = getxattr(buf->link, list, 0, 0, 0, 0);
        else
            buf->acl = getxattr(buf->wayback, list, 0, 0, 0, 0);
        buf->name_shrt = buf->name;
        if ((check = ft_strrchr(buf->name, '/')))
            buf->name_shrt = check;
        buf = buf->next;
    }

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
        ls_object_parse(av[arg++], NULL);
    if (!g_file)
        ls_subtree_parse(".");
    ls_object_sort();
    return (0);
}