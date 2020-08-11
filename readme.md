# 介绍
这是我个人学习unix高级环境编程的一个笔记,没什么亮点,代码基本都能在GNU的编译器下面跑起来,编辑器用的vscode.

总体上杂乱无章的,个人的一个过程记录而已并没有太大的参考价值,大牛请绕道吧
## 基础开场白
ine _GUN_SOURCE 宏不一定必须有宏值,但是是对别的宏功能的一种封装,它可能存在于其他的文件之中.
    +   这个程序一直是在realloc一点点往外扩大空间.所以再初始化的时候把指针和linesize都安全初始化,在使用完了之后把这些内存都free()掉.这个api容易造成可控的内存泄漏.
    +   getline是GUN中的,不存在标准库中.
+   int fflush(FILE *stream):fflush()是一个C语言标准输入输出的库中的函数,功能是冲洗流中的信息,该函数通常用于处理磁盘文件,fflush会强制缓冲区的数据写入指定的stream中.
    +   如果指针指向一个输出流或者一个最近的一次操作不是输入的更新刘,输出刷新会创建任意未写入的数据给他将要写入的文件流和最近数据呗修改流,然后最后文件状态标记为更新的基础文件的时间戳
    +   对于打开已使用基础文件进行读取的流,如果文件尚未处于EOF,并且该文件是能够搜索的文件,则基础打开文件描述的文件偏移量设置为流的文件位置,并且任何并没独处的ungetc()或者ungetwc()退回到流上的字符都被丢失.
    +   如果stream是空的指针,则fflush将对上面定义行为的所有流执行此刷新操作.
    +   返回值0位成功
+   临时文件的问题:1)不冲突 2)及时销毁 
    +   char * tmpnam(char * s); ---> Create a name for a temporary file.这个api存在于标准io库中.这个api在并发编程的时候,可能会引发时间切片进行抢占.
    +   FILE \*tmpfile(void) ---funciton opens a unique temporary file which binary IO.这个函数产生一个不知道啥名字的文件(w+b)的形式,返回给我一个FILE\*,匿名文件.名字都没有,而且是原子操作,不会出现抢占异常.还可以使用fclose()对他进行关闭.
    +   ulimit -a :查看系统目前IO的数据量
+   异常处理的方案.
    +   perror --> print a system error message. void perror(const char * s);
    +   在<string.h>中有一个strerror,char *strerror(int errnum);
```
code:
        fprintf(stderr,"fopen() failed! errno = %d\n",errno);
        perror("fopen()");
        fprintf(stderr,"fopen() failed! %s\n",strerror(errno));
result:
fopen() failed! errno = 2
fopen(): No such file or directory
fopen() failed! No such file or directory
```
+   fopen()-->stream open functions.成功的话返回一个FILE指针,失败的话返回一个errno.
    +   栈区在函数调用完了就会释放.返回的指针不会再栈中.
    +   静态区 static,是在进程结束之前保留的.但是调用函数之后这个static是固定的,调用几次都只用一个static的结构体区域.
    +   堆,在开辟空间之后在对应的fclose()还可以free,如果函数的返回值有一个指针,并且有一个逆操作的-->一般都在堆区域
    +   mode中w w+ r r+所建立的属性都是**0666 & ~umask**这样创建的一个文件.
        +   umask负责一个用户创建一个文件时候的默认值,它与chmod的效果刚好相反,umask设置的是权限补码,而chmod设置的是文件的权限吗.profile中可以设置umask的值.
        +   umask的值越大,权限越低.
        +   umask:在登陆一个系统之后创建一个文件总是有一个默认的权限,这个
+   一个进程最大打开文件个数:在你打开一个进程时候已经有3个文件已经打开stdin stdout stderr,可以使用ulimit -n 来进行修改.centos默认是1024个.
+   getchar() is equivalent to getc(stdin).
    +   fgetc , getc 这个2函数是不一样的,getc是作为一个宏定义使用的,节省运行时间
+   putchar(c); is equivalent to putc(c,stdout).putc fputc  
    +   同样的putc也是宏定义的,fputc 是函数.这之中的f--> function.
+   fread,fwrite
    +   gets()-->不要用,不检查缓冲区的异常.char \*gets(char \*s),只约定了一个地址,输入缓冲区-->回车-->放入地址,这个地址没有划分空间大小,因此会出现一处问题
    +   char \* fgets(char\*s,int size,FILE \* stream);这个函数是安全的,可以指定大小.这个函数正常结束 size-1 或者遇到\n.
        +   在fgetc的时候读取一个字符串都会引入一个\n \0.
    +   size_t fread(void * ptr,size_t size,size_t nmemb,FILE * stream);
    +   size_t fwrite(const void *ptr,size_t size,size_t nmemb,FILE * stream);
    +   这2个操作都是2进制类型的操作,对于成块的数据操作的时候是非常好用的.但是对于数据的边界没有验证,如果中间有一个字节出现错误,那么所有的数据都会错位.
        +   当前数据量足够的时候,返回1.
        +   当文件当中只有5个字节的时候,不足够的时候,返回0会出现问题.
        +   建议当做单字节使用,如果文件错位,后面的都会错位.
+   sprintf(char *s,format *f,...) -->把数据格式化输出到一个string中
    +   atoi()--->在读取字符串时候不是数字的内容会自动截断.sprintf--->可以作为itoa(并不存在的)....
    +   int snprintf(char *str,size_t size,const char *format,...);防止sprintf越界.
+   fscanf(FILE) sscanf(s).....前者从流中读取,后者从字符串中读取.
+   int fseek(FILE \*stream,long offset,int whence); long ftell(FILE \*stream);void rewind(FILE)  0-2GB,但是现在的文件不是2GB能够承载的.
    +   int fseeko(FILE *stream,off_t offset,int whence); off_t 32 64都有可能----->On many architectures both off_t and long are 32-bit types,but compilation with #define _FILE_OFFSET_BITS 64 will turn off_t into a 64-bit type.
+   fflush():printf中如果不加入\n是不会刷新缓冲区的.
    +   If stream argument is NULL,fflush() flushess all open output steams.
    +   缓冲区的作用:在大多数情况下是好事,合并系统调用 -------     
        1.  行缓冲:换行时候刷新,强制刷新(标准输出是这样的,因为它是一个终端设备)
        2.  全缓冲:慢了的时候刷新,强制刷新(默认模式,只要不是终端设备)
        3.  无缓冲:立即刷新stderr
    +   setvbuf() --> int
     setvbuf(FILE *restrict stream, char *restrict buf, int type,
         size_t size);The setvbuf() function may be used to alter the buffering behavior of a
     stream.  The type argument must be one of the following three macros:

           _IONBF  unbuffered

           _IOLBF  line buffered

           _IOFBF  fully buffered
##  文件IO /系统调用IO
fd是在文件IO中贯穿始终的类型.
+   文件描述符的概念:文件描述符fd int 0 1 2 对于流的一种映射表,有限使用当前范围内最小的.文件描述符对应的FILE \* 结构体 是独立的,就算是打开同一个文件,不同的流吗,也是会相对独立的结构体.
+   open,close,write,read,lseek.标准IO中的都是根据上面5个系统调用IO函数实现的.
    +   int open(const char* pathname,int flags);
        +   RETURN:open() and creat() return a new file descriptor,or -1 if an error occured(文件描述符没有负数这么说,所以会返回-1)
        + flags是一个位选项, flags must include one of the following access modes : O_RDONLY,O_WRONLY or O_RDWR
        +   常用的有一些,O_DIRECT try to minimize cache effects of the I/O to and from this file.buf应该理解成为一个写的缓冲区,cache理解为一个读的缓冲区,不改变文件的修改时间,O_NONBLOCK不阻塞 
        +   **r -> O_RDONLY ,r+ -> O_RDWR , w -> O_WRONLY|O_CREAT|O_TRUNC , w+ -> O_RDWR|O_CREAT|O_TRUNC,0777** 位图的创建.
        +   open函数有两参数和三参数形式.在使用O_CREAT的时候,使用三参数,(重载 -> 定参,多参数 -> 不报错),只有在O_CREAT时候 第三个参数才有用!!!!!一晚上的教训!!!
    +   read():ssize_t read(int fd,void *buf,size_t count);
    +   write():ssize_t write(int fd,const void * buf,size_t count);
    +   lseek();off_t lseek(int fd,off_t offset,int whence);跟fseek()差不多.
+   文件IO与标准IO的区别:
    +   文件IO:是切换系统内核的实现方式
    +   标准IO:在缓冲区中缓存然后整体放入缓冲区的一种调用方式
    +   区别:文件IO响应速度快,标准IO吞吐量大
    +   面试:如何让一个程序变快?--->需要一份为2来作答,响应速度,想让吞吐量变大......从用户体验的角度是吞吐量大才会快,这样可以减少内核的切换频率.
    +   提醒:系统IO和标准IO不能混用.
    +   转换fileno,fopen进行2个IO转换.int fileno(FILE \*stream);FILE * fdopen(int fd);
+   文件共享:int truncate(int fd,off_t length);int ftruncate()
+   原子操作:不可分割的操作,解决的竞争和冲突
+   程序中的重定向:dup,dup2--->dup() duplicates an existing object descriptor and returns its value to
     +  dup :创建一个newfd，newfd指向oldfd的位置，并且dup返回的文件描述符总是取系统当前可用的最小整数值
     +  dup2 :将已有的一个newfd重定向到oldfd的位置，返回第一个不小于oldfd的整数值
     
     复制文件描述符.
+   同步:sync,fsync,fdatasync ------ fcntl(); ioctl();管家级别的函数:
    +   sync -- force completion of pending disk writes (flush cache)-->全局催促.全局刷新流.
    +   fsync -- synchronize a file's in-core state with that on disk-->刷一个文件流
    +   fdatasync--->同步一个文件,但是不刷亚数据
    +   fcntl():文件描述符相关的函数都来来自于这个函数-->fcntl() provides for control over descriptors.  The argument fildes is a descriptor to be operated on by cmd as follows.
    +   iocntl():设备相关的内容都由它来管/dev/....... 
+   虚拟目录:当前进程的文件描述符的信息,ls -al /dev/fd   看得是terminal打开的ls
##  文件系统
做一个myls,可以有一个系统的.-a -i -l
### 目录和文件
+   获取文件的属性:stat fstat lstat
    +   int fstat(int fildes, struct stat *buf);
    +   int stat(const char *restrict path, struct stat *restrict buf);
    +   int lstat(const char *restrict path, struct stat *restrict buf);
```
struct stat { /* when _DARWIN_FEATURE_64_BIT_INODE is NOT defined */
         dev_t    st_dev;    /* device inode resides on */
         ino_t    st_ino;    /* inode's number */ 
         mode_t   st_mode;   /* inode protection mode */
         nlink_t  st_nlink;  /* number of hard links to the file */
         uid_t    st_uid;    /* user-id of owner */
         gid_t    st_gid;    /* group-id of owner */
         dev_t    st_rdev;   /* device type, for special file inode */
         struct timespec st_atimespec;  /* time of last access */
         struct timespec st_mtimespec;  /* time of last data modification */
         struct timespec st_ctimespec;  /* time of last file status change */
         off_t    st_size;   /* file size, in bytes */
         quad_t   st_blocks; /* blocks allocated for file */
         u_long   st_blksize;/* optimal file sys I/O ops blocksize */
         u_long   st_flags;  /* user defined flags for file */
         u_long   st_gen;    /* file generation number */
     };

```
+   空洞文件:空洞文件特点就是offset大于实际的大小,也就是说一个文件两头有数据,中间是空的,\0来填充,系统不会傻到在磁盘上实际存放这么多东西的,第一是一种浪费,第二是一种威胁,因为黑客利用这个漏洞不断侵蚀磁盘资源,计算机就会崩溃.
    +   使用ls展现的是文件的逻辑大小,
    +   du展现的是文件的物理大小."文件系统使用了若干块以存放指向实际数据块的各个指针"。
    +   空洞文件作用很大，例如迅雷下载文件，在未下载完成时就已经占据了全部文件大小的空间，这时候就是空洞文件。下载时如果没有空洞文件，多线程下载时文件就都只能从一个地方写入，这就不是多线程了。如果有了空洞文件，可以从不同的地址写入，就完成了多线程的优势任务。
+   文件权限的更改/管理:在st_mode中,用于表现文件的类型,文件的访问权限,以及特殊权限位置
    +   文件类型:st_mode : dcp-lsp 目录|字符设备文件|块设备文件|常规文件|符号联结文件|sock文件|管道文件    --->   总共7种文件.
    +   UNIX系统提供了7个宏函数,来判断文件的类型.
        1.  S_ISREG(m) is it a regular file.
        2.  S_ISDIR(m) is directory?
        3.  S_ISCHR(m) character device?
        4.  S_ISBLK(m) block device?
        5.  S_ISFIFO(m) FIFO (named pippe)
        6.  S_ISLNK(m) symbolic link?
        7.  S_ISSOCK(m) socket?
    +   也可以用按照位图的值进行比较.
+   umask :0666 & ~umask 是这么一个公式,我的苹果电脑默认是022,002自动生成的是0644
    +   int chmod
    +   int fchmod  //自己去man吧.
+   粘住位: t位,给某一个可执行二进制执行的文件设置t位,在内存中保存它的一些信息,使它调用的更快.
+   文件系统:FAT,UFS(不开源,开源)文件系统:文件系统:文件或者数据的存储和管理
    +   FAT16/32 :静态存储的单链表<arr>,他这种承载能力有限,所以不能像UNIX一样使用文件的模式,而是使用了盘符模式<怕大文件>
        +   swap的交换分区是内存速度的1/200000倍.一些内存清理程序,就是把内存的数据挤到了swap/.360搞了一个进程,吃分区吃满了把其他的挤到swap中.
        +   磁盘清理也是改变链表的顺序
    +   UFS(开源):在最初的FFS文件系统设计中,为了使文件系统在遭到毁灭性打击之后,如硬盘放生整个磁道,整个盘面或者整个柱面损坏时候能够得以恢复,在文件系统初始化的时候,会将文件系统的重要数据复制到整个磁盘的多个位置,以便发生硬件损坏的时候能够读取,而UFS文件系统也保持这个优点<br>另外,为了提高运行效率,UFS文件系统与磁盘的结构也有着完美的结合,UFS将整个磁盘的所有逻辑柱面平均分配为若干个组,每个组成为一个柱面.在UFS内部就用柱面组队文件系统进行分段组织和管理,每个柱面组中都有文件系统关键数据结构的备份,所有文件在
        +   UFS文件系统在创建的时候
+   目录:硬链接,符号链接(软连接):
    +   硬链接:是一种关联,2个指针指向同一个空间.目录项的同义词(不能给分区建立,不能给目录建立)
    +   符号链接:ln -s (symbol) <--一个快捷方式 被记录在inode中.(可以跨分区,可以给目录建立)
    +   int link(const char * oldpath,const char * newpath);
    +   int unlink(const char * pathname);
    +   remove ---> rm 但是文件夹非空的是不能删除的
    +   rename -- change the name of location of a file.
+   utime.h:int utimes(const char *fielname,const struct utimbuf *time)
```
struct utimbuf{
    atime
    mtime
    };
```
+   目录创建和销毁:int chdir(const char * path);脱离控制终端,在umount之后必须切换运行环境,让进程一直在一个存在的环境中运行.
+   更改路径:int fchdir(int fd);
    +   chroot:假根技术,让一个进程在子目录跑,告诉进程当前的目录是一个根目录.顾名思义:change root directory(更改root目录),在linux系统中,系统默认的结构是以'/',经过chrootHi后,系统读取道德目录文件将不是在旧的系统根目录下,而是一个新的根下.
        1.  增加了系统的安全性,限制了用户的权利:经过chroot之后,在新的根下将访问不到旧系统的根目录结构和文件,这样就增强了系统的安全性,这个一般是在登陆(login)和使用chroot,以达到用户不能访问一些特定的文件.
        2.  建立一个与原系统隔离的系统目录结构,方便用户的开发:使用chroot之后,系统读取的是新的根下的目录和文件,这是一个原系统不相关的目录结构,在这个新的环境下,可以用来测试软件的静态编译以及与系统不相关的独立开发.
        3.  切换系统的根目录位置,引导Linux系统启动以及急救系统:chroot的作用就是切换系统的根位置,而这个作用最为明显的就是在系统初始化引导磁盘的处理过程中使用,从RAM磁盘切换到系统的根位置并执行真正的init.另外当系统出现一些问题时候也可以使用chroot来切换到一个临时的系统.
+   glob(3):搜索匹配函数,是我认为最方便的目录操作函数,包含在glob.h库中,操作结构体<pre>typedef struct {
             size_t gl_pathc;        /* count of total paths so far */
             int gl_matchc;          /* count of paths matching pattern */
             size_t gl_offs;         /* reserved at beginning of gl_pathv */
             int gl_flags;           /* returned flags */
             char **gl_pathv;        /* list of paths matching pattern */
     } glob_t;</pre>
+   目录流操作:
    +   opendir() :打开一个与给定的目录name相对应的目录流,并返回一个指向该目录流的指针,打开后,该目录指向了目录中的第一个目录项,RETURN VALUE,打开成功指向目录的流指针;打开失败返回NULL并且设置相应的errno
    +   closedir():函数关闭与指针dir相联系的目录流,关闭后,目录流描述符dir不再可用.
    +   readdir(3) :该函数返回一个指向dirent的结构体指针,该结构体代表了由dir指向的目录流中的下一个目录项;如果读到了EOF或者出现错误那么则返回NULL<pre>       struct dirent { 
             ino_t          d_ino;       /* inode number */ 
             off_t          d_off;       /* offset to the next dirent */ 
             unsigned short d_reclen;    /* length of this record */ 
             unsigned char  d_type;      /* type of file */ 
             char           d_name[256]; /* filename */ 
         };</pre>
    +   rewinddir()
    +   seekdir()
    +   telldir()
+   分析目录/读取目录内容:int getcwd -->get current working path.
+   一般使用递归,但是要设置一个递归上线,防止栈破裂.
### 系统数据文件和信息
```
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>
     
struct passwd {
                   char    *pw_name;       /* user name */
                   char    *pw_passwd;     /* encrypted password */
                   uid_t   pw_uid;         /* user uid */
                   gid_t   pw_gid;         /* user gid */
                   time_t  pw_change;      /* password change time */
                   char    *pw_class;      /* user access class */
                   char    *pw_gecos;      /* Honeywell login info */
                   char    *pw_dir;        /* home directory */
                   char    *pw_shell;      /* default shell */
                   time_t  pw_expire;      /* account expiration */
                   int     pw_fields;      /* internal: fields filled in */
           };
```
+   /etc/passwd:在不同的OS中存储的信息是不同的因此需要标准的函数进行**和稀泥操作**
    +   getpwuid
    +   getpwnam

+   /etc/group :组信息.
    +   getgrgid()
    +   getgrnam();
+   /etc/shadow:macos没有shadow文件.在linux中,存储密码的.
    +   hash不是加密,是一种混淆算法. 哈希只能混淆但是不能解密,哈希加密的密码混淆的是相同的,可以用来撞库.(防备管理员监守自盗)
    +   加密:可以用穷举进行破解,口令随机校验.
        +   口令随机效验:2次成功才成功,登陆界面会让你重新输入一遍,他会随机效验.
        +   安全:攻击成本大于收益
        +   char * crypt(const char *key, const char *salt);salt = $ID$salt$ , 1 MD5 2a: 5 SHA-256 6 SHA-512
        +   getpass("...");s
+   时间戳:
    +   time();从内核中取出来一个时间戳,这个时间戳年份是从1900年开始的,月份是从0月开始的.
    +   localtime()
    +   gmtime() --> 格林威治时间
    +   mktime() --> 把一个越界结构体转变成一个时间戳,并且将越界部分计算完毕:The functions mktime() and timegm() convert the broken-out time (in the
     structure pointed to by *timeptr) into a time value with the same encod-
     ing as that of the values returned by the time(3) function
    +   strftime() --> 放在静态类型中,赶紧用,转化完了放一会可能就变了
+   有一些bug可以对客户说是特性,一层窗户纸的事儿.
### 进程环境
+   main函数:
    +   int main(int argc, char ** argv) ----> 
+   进程的终止:
    +   正常终止:
        1. 从main函数返回,
        2. exit().
        3. 调用\_exit或者\_EXIT ---> 系统调用
        4. 最后一个进程从启动例程返回.
        5. 最后一个线程调用了pthread_exit
    +   异常终止:
        1.  abort调用,弃用一个进程
        2.  接到一个信号并终止 cmd + c
        3.  最后一个线程对其取消请求作出相应.
    +   父进程看子进程的退出状态echo $?:
        +   void exit(int status);The C Standard (ISO/IEC 9899:1999 (``ISO C99'')) defines the values 0,EXIT_SUCCESS, and EXIT_FAILURE as possible values of status.  Cooperatingprocesses may use other values; in a program which might be called by amail transfer agent, the values described in sysexits(3) may be used to provide more information to the parent process.
        +   exit函数的参数的退出状态是 status & 0377 言外之意 只保留了第八位有限字符,所以exit返回值是-128 到 127的 255个数字
        +   _exit(),直接做退出,不做IO清理,abort
    +   atexit() and on_exit():钩子函数
        +   atexit注册一个函数,在进程正常终止之前被调用,释放没有释放的内容,类似**C++的析构函数**,把逆操作放在里面.
+   命令行参数的分析:
    +   getopt --> 跟python的那个getoptparse差不多
    +   getopt_long():直接把man手册的例子贴在下面吧
```
//  如有有冒号 可以使用全局变量
     extern char *optarg;
     extern int optind;
     extern int optopt;
     extern int opterr;
     extern int optreset;

     int
     getopt(int argc, char * const argv[], const char *optstring);
     
EXAMPLES
     #include <unistd.h>
     int bflag, ch, fd;

     bflag = 0;
     while ((ch = getopt(argc, argv, "bf:")) != -1) {
             switch (ch) {
             case 'b':
                     bflag = 1;
                     break;
             case 'f':
                     if ((fd = open(optarg, O_RDONLY, 0)) < 0) {
                             (void)fprintf(stderr,
                                 "myname: %s: %s\n", optarg, strerror(errno));
                             exit(1);
                     }
                     break;
             case '?':
             default:
                     usage();
             }
     }
     argc -= optind;
     argv += optind;
``` 
在const char * s传入需要代入的字符,如果有需要进行参数进行修饰的,加上一个冒号':s'
+   环境变量:使用export可以导出所有的环境变量[Key = Value]
    +   environ(char **) ,字符串数组.这是一个全局变量,extern char ** environ;
    +   getenv(const char *): 向函数中传入环境变量名字,它会返回环境变量的值.
    +   setenv(const char * name,const char * value,int overwrite);如果name存在如果不存在.
    +   putenv()
+   C程序的存储空间布局:
    +   x86:
        +   0-3G:user:0xc0000000
        +   3G-4G:kernel:0x08048000
        +   在i386上,由于历史原因,可执行文件的基本地址为0x8048000,这样做的原因是当栈堆直接放在可执行文件的下面时,允许在本文下方有相当大的堆栈,而仍保持在地址0x08000000上方,从而允许堆栈和可执行文件可以共存于同一个第二页表中.<br>如果栈溢出了,我们希望进程崩溃而不是覆盖其他的数据,因此栈应该有很大很大的空间,同时栈增长到这片空间的末端时,进程会崩溃,空指针解引用会导致进程崩溃,这是一个可以方便利用的特质,所以定义了除了第一页外,栈获得前128MB的地址空间,这意味着代码必须在128MB以后,所以地址类似于0x08000000.<br>据说,在NIX/i386上,0x08048000曾经是STACK_TOP(也就是说,堆栈从0x08048000附近向下增长到0)
    +   x64
        +   amd64使用更大的地址,存在一些技术限制,所以建议将2GB的地址空间专门用于代码和数据来提高效率,所以栈被移除了该范围.<br>0x40000(4MB)足够大,可以补货任何可能不正常的结构偏移(指针指向0对象,获取对象中成员变量),从而允许amd64运行较大的数据单元,但是有足够小不会浪费太多的那2GB地址
        +   0x400000(4MB)迄今为止最大的实际页面大小,并且目前所有其他虚拟内存的单元大小的公倍数.

|segment|含义|
|:---:|:---:|
|argument & environment|命令行和环境变量(环境表和环境字符串)|
|stack|栈|
|heap|堆|
|bss|未初始化数据段|
|data|初始化数据段|
|text|正文段(代码段)|

1.  栈(stack):由编译器自动分配释放,存放函数的参数值,局部变量的值等等(但是不包括static声明的变量,static意味着在数据段中存放变量).除此之外,在函数被调用时,栈用来传递参数和返回值.由于栈的先进后出的特点,所以栈特别方便是用来保存和回复数据.其操作方式类似于数据结构中的栈.
2.  堆(heap):堆是用于存放进程运行中被动态分配的内存段,它的大小并不固定,可以动态扩张或者缩小,当进程调用malloc/free等函数分配内存时候,新分配的内存就被动态添加到了堆上(堆被扩张)/释放的内存从堆中被提出(堆被缩减).堆一般由程序员分配释放,若程序员不进行释放,程序结束时候可能由OS回收.**它与数据结构的堆是两回事,分配方式类似于链表**.
3.  全局数据去(静态区)static:全局变量和静态变量的存储是放在一块的,初始化的全局变量和静态变量在一块区域,未初始化的全局变量和静态变量在相邻的另一块区域,另外文字常量区,常量字符串就是放在这里,程序接触后由OS系统.
    1.  BSS段(bss segment):通常是指哟领你刚来存放程序中**未初始化**的全局变量的一块内存区域,bss段属于静态内存分配.
    2.  数据段(data segment):通常是用来存存放**已初始化**的全局变量的一块内存区域,数据段属于静态内存分配.
4.  代码段(code segment):通常指用来存放程序执行代码的一块内存区域.这部分区域的大小在程序执行之前就已经确定了,并且内存区域通常属于只读.(某些框架也允许代码为可写,即可以修改程序),在代码段中,也有可能包含一些只读的常数变量,例如字符串常量等,程序段为程序代码在内存中的映射,一个程序可以在内存中有多个副本.
    +   使用pmap(shell)可以看所有映射的地址

+   库:
    +   动态库
    +   静态库
    +   手工装载库:内核中的库就是一些插件,如果能够加载则加载....
        +   dlopen()
        +   dlclose()
        +   dlerror()
        +   dlsym()
+   函数跳转
        +   getconf PAGE_SIZE ---->4096 4k一页,默认的.
        +   goto:不能执行跨函数的跳转
        +   catch---throw
        +   ret = setjmp(jmp_buf save) ---> 设置跳转点,ret为跳转函数进行跳转的时候带回来的信号值
        +   longjmp(jmp_buf,int val) ---> 跳转   可以安全的跳转 长跳转
+   资源的获取以及控制
    +   getrlimit();
    +   setrlimit();
### 线程与进程
#### 进程基本知识
1.  进程标识符pid.
    +   类型pid_t
    +   ps命令
    +   进程标识是顺次向下使用的
    +   getpid():获得当前进程的进程号
    +   getppid():获得父进程的进程号
2.  进程的产生
    +   fork():memory copy-->稍微有一些不一样其他都差不多
        +   duplicating意味着拷贝,克隆,一模一样等含义,
        +   fork后的父子进程的区别,fork的返回值不一样,pid不同,ppid也不同,未决信号和文件锁不继承,资源利用量清0.
        +   init:1号,是所有进程的祖先进程:
        +   **调度器**的调度策略来决定是那个进程先运行.
        +   **在fork之前刷新该刷新的流 fflush(NULL)**. 在终端上打印是\n一个begin 但是再重定向 > 不是行缓冲模式----> 是2个begin.
        +   wc -l 记录输出字节,行数等等.
        +   S:睡眠态(sleep):子进程还没执行完,父进程已经退出,所以系统会把这些没有父亲的进程收回来,给了init总进程.
        +   Z:僵尸态(zombie),不是很占内存,只有一个结构体还保存这退出状态.不会占很多的资源,但是pid没有释放.----> 父亲状态没有正常退出,子进程已经执行完了.
    +   vfork()[快废弃了]:fork成本特别高,vfork创建的内容,只能成功调用exec函数组中的某一个函数,其他的都是未定义行为.
        +   不过现在fork函数已经有加强,有一个写实拷贝技术,在进行fork的时候,我的子进程与父进程用的确实是一个公用数据块,如果是只读不写的模块父子进程都不会进行改变,就会进行拷贝并且写入(谁改谁拷贝).
3.  进程的消亡以及释放资源
    +   wait()
    +   waitpid()
    +   waitid()
    +   wait3()
    +   wait4()
    +   分快法分配:
    +   交叉分配法:
    +   池类算法:抢占式任务分配.
+   补充:进程的状态:
    +   R(TASK_RUNNING),可执行状态:只有在这个该状态下进程才可以在CPU上运行,而同一个时刻可能有多个进程处于执行态,这些进程的task\_struct结构体(进程控制块)被放入对应的CPU可执行序列中分别学则一个进程在一个CPU上面跑.*在很多的参考书上,正在CPU上运行的进程被定义为RUNNING状态,而将可执行但是尚未被调度的执行进程定义为READY状态,但是再LINUX下统一定义为TASK_RUNNING*
    +   S(TASK_INTERRUPTIBLE)可中断的睡眠态:处于这个状态的进程因为等待某某事件的发生(例如等待socket,等待信号量),而被挂起,这些进程的task\_struct结构被放入对应的事件的等待队列中,当这些事件发生时候(由外部中断触发,或者由其他进程出发),对应的等待结构中的一个或者多个进程将被唤醒.通过ps命令我么会看到,一般情况下,进程列表中的绝大对数进程都处于TASK_INTERRUPTIBLE状态(除非机器负载很高),毕竟CPU就4 - 8核,上百个进程必须来回切换这运行.
    +   D(TASK_UNINTERRUPTIBLE),不可中断的睡眠状态:与TASK_INTERRUPTIBLE状态类似,进程处于睡眠状态,但是此刻进程是不可中断的.<此刻的不可中断指的并不是CPU不相应外部硬件的终端,而是指进程不响应**异步信号**>.绝大多数情况下,进程处于睡眠状态时,总是应该能够响应异步信号.否则你将惊奇的发现kill -9竟然杀不死一个正在睡眠的进程.ps也看不到D状态的进程.<br>D状态存在的意义就在于,内核的某些处理流程是不可以被打断的,如果响应异步信号,程序的执行流程中就会被插入一段用于处理异步信号的流程,D状态就是搞一个原子态.使用vfork之后父进程将进入D状态,直到子进程调用exit或者exec
    +   T(TASK_STOPPED or TASK_TRACED).暂停状态或跟踪状态:向进程发送一个SIGSTOP信号,他就会因响应该信号而进入T状态(除非该进程本身处于TASK_UNINTERRUPTIBLE而不响应信号),SIGSTOP和SIGKILL信号一样,是非常强制的,不允许用户进程通过signal系列的系统调用重新设置对应的信号处理函数,向进程发送一个SIGCONT信号,可以让其从T --- > R状态
    +   Z(TASK_DEAD-EXIT_ZOMBIE),退出状态,进程成为僵尸进程:在进程突出的过程中,处于TASK_DEAD状态.在这个退出过程中,进程占有资源将被回收,除了**task_struct(以及极少数的资源)**,于是只剩下了一个task_struct的空壳,故称作僵尸.之所以保留task_struct是因为这个结构里面存储了程序的退出码以及一些统计信息,其父进程很可能会关注这些信息,比如在shell中使用$?变量就保存了前台进程的退出码,而这个退出码往往被作为if语句的判断条件.当然了,内核也可以将这些信息保存在别的地方,而task_struct结构释放掉,以节省一些时间,但是使用task_struct结构更为方便,因为在内核中已经建立了从Pid到task_struct的查找关系,还有进程间的父子进程的关系.<br>父进程通过使用wait系列的函数来等待某个某些子进程的退出,并获得它的退出信息.然后wait系列的系统调用混顺便将子进程的尸体也释放掉.子进程在退出的过程中,内核或给其父亲进程发送一个信号,通知父进程来"收尸",这个信号默认是SIGCHLD.如果父亲进程已经不存在了,那么就是给我们的NO.1进程init.让它来收尸
    +   X(TASK_DEAD-EXIT_DEAD),退出状态,进程即将被销毁:而进程在退出的过程中可能不会保留它的task_struct,比如这个进程是多线程程序中被detach过的进程.或者父进程通过设计SIGCHLD信号的handler为SIG_IGN显示忽略了SIGCHLD信号,此时进程将被置于EXIT_DEAD退出状态,这意味着接下来的代码立即就会将该进程彻底释放掉.所以EXIT_DEAD装备是机器短暂的,几乎不能被ps命令捕捉到.
+   文件权限管理:linux文件有三种权限:用户权限,群组权限,其他用户权限,非别为rwx
    +   u+s用户权限:执行chmod u+s 就是针对某个程序任何永不都有读写这个程序的权限,可以向root用户一样操作,这个指令只对程序有效,如果这个指令放在路径上是无效的.
    +   g+s群组权限:强制将次群组里的目录下文件编入此群组,无论哪个用户创建的文件
4.  exec函数族的使用:
    +   fork进行的是一个分支拷贝
    +   exec是创建一个新的任务,bash-->main 一个意思,我们的bash中并不存在main的代码,但是可以执行
    +   If any
    +    of the exec() functions returns,an error will have occurred. The return value is -1,and errno will be set to indicate the error.
+   shell -> exec -> wait:子进程会继承父进程的0 , 1 , 2设备,文件描述符也可以继承.可以通过文件进行交互,可以说UNIX世界就是fork exec wait搭建起来的.
+   对于自制的bash终端,当我们建立一个动态数组来存储串,使用int glob().GLOB_NOCHECK
+   当我们的机器开机,第一个进程是init进程,之后会产生一个fork+exec产生一个getit进程name: passwd -->exec-->login ---> checkpass 成功---> shell 切换到当前的用户,我的身份目前为止是固定了.
5.  u+x g+x:
+   u+s:让所有的用户都拥有所有人的权限,如果使用chown root 则其他用户也可以使用root权限运行这个文件.
+   g+s:群组权限出现s的情况，执行命令是chmod g+s，它的意思是强制将此群组里的目录下文件编入到此群组中，无论是哪个用户创建的文件。
+   **其他权限出现t的情况，命令是chmod o+t，意思是这个目录只有root和此目录的拥有者可以删除，其他用户全都不可以。**
+   getuid():return real user ID of calling process
+   geteuid():return effective user ID
+   getgid()
+   getegid()
+   setuid() :set the effective user ID of calling process
+   setgid():
+   setreuid():交换ruid euid
+   setregid():交换rgid egid
+   seteuid():更改有效用户ID
+   setegid():更改有效用户组ID
6.  解释器文件:./a.sh chmod u+x ....在装载这个文件的时候发现#!/bin/bash这个标记的时候,然后会装载一个脚本解释器,然后使用指定的解释器来解释所有指令.#!就是一个脚本的注释,shell一看到#!之后马上吧这个脚本解释器装载进来,然后再进行执行.如果#!/bin/cat
7.  system() ->(2,3,4的封装)   ---> system() executes a command specified in command by /bin/sh -c command
8.  进程会计:acct
9.  进程时间:time ./.....
10.  守护进程:开机之后自动启动,脱离控制终端,是一个会话的领导者,也是一个进程组的领导者,守护进程在httpd,qq服务器,wechat服务器,发挥作用,他不会在我登陆QQ的时候才启动.<br>会话:终端--->出现在早期,设备非常昂贵的时候,不可能一个人一台电脑,一个公司就有一台而已,多个人以终端的形式进行访问[例如在银行中,要求安全性特别高的单位才能看到终端],实际上一个终端的用户登录就是一个会话(session)
+   前台进程组:最多只能有一个.
    +   后台进程组:重定向 0 1 2 ---> log.txt
    +   setsid():The setsid function creates a new session.  The calling process is the
     session leader of the new session, is the process group leader of a new
     process group and has no controlling terminal.  The calling process is
     the only process in either the session or the process group.调用的进程如果不是一个会话的leader,则那么就会打开一个新的会话并把他做成这个会话的leader.
    +   getpgrp():
    +   int setpgid(pid_t pid,pid_t pgid)
    +   getpgid():当进程是会话的领头进程时setsid()调用失败并返回（-1）。setsid()调用成功后，返回新的会话的ID，调用setsid函数的进程成为新的会话的领头进程，并与其父进程的会话组和进程组脱离。
        +   open("/dev/null",o_rdwr)作用:在开始一个守护进程的时候都会这样做得到的解释就是(make sure we have stdin/stdout/stderr reserved),写入/dev/null的东西会被系统丢掉,就如同写注释一样,把012分配出去,以后再分配的时候不会将这3个流打开,以达到保护的目的.
由于会话对控制终端的独占性，进程同时与控制终端脱离。
11.  系统日志
+   syslogd:统一由一个守护进程,它来写系统日志.-->对系统的权限分离.
    +   openlog(char * ident,int opt,int facility); --- > 人物字段,选项,从哪里来
    +   syslog(int priority,char *format,...);   //可以当做printf来用.
    +   closelog()
    +   单个实例的进程:锁文件 /var/run/name.pid,每次开启这个进程,就会在这个目录下面生成一个锁文件.
    +   启动脚本文件:/etc/rc*.... 可以在这里面加上.
## 并发
信号与线程很少一块使用.不要大范围的混用信号和线程

---

## 信号来实现并发

Semaphore..

同步:电脑上的都是同步事件.

异步:什么时候到来我们不知道,不可打断的就是一条机器指令,单核机器中每个进程轮流使用机器的世界.
1.  查询法: 一直在扫描,异步时间发生的比较稠密.while 死循环 去读取状态位.开销太大了.
2.  通知法: 如果异步的时间发生的比较稀疏.当某一位触发事件之后,再进行一系列的操作.
####    信号的概念
信号是软件中断,信号的响应依赖于中断机制,底层来讲是中断来实现了软件的信号机制,可以理解为软件中断.
####    signal()
+   kill -l:可以查看所有的信号
```
HUP INT QUIT ILL TRAP ABRT EMT FPE KILL BUS SEGV SYS PIPE ALRM TERM URG STOP TSTP CONT CHLD TTIN TTOU IO XCPU XFSZ VTALRM PROF WINCH INFO USR1 USR2
34 - 64 realtime signal 实时信号
```

+ core 文件是程序的某一个现场,一般是错误的现场:做一个段错误,ulimit -c 1 可以产生一个core文件,可以在选项中加入gdb进行调试,

+   signal():sig_t signal(int sig,sig_t func);第一个为信号第二个为行为,返回之前动作的函数. 在man手册的第三章节中写了对于sig_t的宏定义,void (*signal(int sig,void (\*func)(int)))(int); --> 或者更好阅读的方式可以看到 .... typedef void (\*sig_t)(int).
    +   signal重新定义的handler是一个void func(int)的函数,给这个handler传入的参数就是信号的标记号.
    +   这种函数调用的暴露了C的一个大问题,名空间安排不善,也就是C++中的namespace,C中并没有,如果2个库都倒霉的有一个函数名字,那么就**炸**了
+   当我们使用ctrl+c,发送sigint信号,把我们的进程中断,他的默认功能是终止一个进程.我们可以注册一个信号行为使用signal
    +   使用SIG_IGN --- >IGNORE 忽略这个信号.
    +   使用SIG_DEL ---> default 恢复这个信号
    +   **信号会阻塞的系统调用:例如write & read 系统调用,EINTR,当我们打开一个比较慢的设备的时候,如果发生了信号,那么会打断阻塞进程,所以会出现EINTR,可以理解为假的错误.
####    信号的不可靠
信号的行为是不可靠的, 在我们调用信号的时候,内存执行线程是内核帮我们布置的,当一个信号在处理的时候如果来了第二个信号,那么执行线程就被覆盖了,不能恢复进去了.
+   最后做成了一个链式结构来恢复现场.
####    可重入函数
第一次调用还没结束,但是第二次调用就发生还没有报错,就叫做可重入函数.
+   所有的系统调用都是可重入的(但是不是所有的可重入的都是系统调用),有一部分库函数也是可重入的,memcpy就是一个
    +   rand() :生成随机数是不可能重入的函数,但是rand_r就是可以重入的.int rand_r(int * seedp);非原子操作放在静态区上的时候就会出现可重入现象,这次调用没完成,就遇到了调用,就有可能出现了第二次的调用吧第一次的调用冲掉了.所以一般Reuseful函数就让你传一个你指定的地址,来保存地址,防止被信号冲刷掉

####    信号的响应过程
例子
```
main --> 打印*
interrupt --> 打印!
mask --> 信号屏蔽字 bitmap 11111111111....
pending --> 记录当前的进程收到那些信号 bitmap  10000001000000...
是内核为每一个进程维护这2个位图
```
信号从收到 到响应有一个不可避免的延迟.思考:*如何忽略掉的一个信号的?标准信号为什么要丢失?*
1.  当我们收到了一个信号的时候,抓内核,然后内核让你等待,当轮到你的进程的时候mask & pending ---> 这样才能发现信号(没有中断打断进行之前的过程你就一直看不到信号,需要一个中断打断现场进入内核态的.)
2.  当发现信号以后,我们恢复现场的时候addr位置就被换成了之前已经注册好的信号处理函数...并且执行
3.  当我们处理完了信号处理函数,我们要把mask 恢复全 1,在把地址替换会刚才的中断时刻的现场,继续执行我们的进程.
+   标准信号的缺陷:在接收到多个信号的时候没有严格的顺序.(一般是响应错误,严重.这种级别的.)<br>而且有一定延迟.还会丢失(位图,当来了10000个信号 位图上面只能表现1个...)
+   信号是在kernal -> user 的路上响应的.
+   SIG_IGN  就是把mask位置置0了.&不出来了哈哈哈哈.
#####   不能从信号处理函数中随意的往外跳.所以在我们用setjmp -- longjmp中,就要使用sigsetjmp(),跳转的时候保存信号.

####    kill 并非是杀死一个进程
kill的真正的功能是给一个进程发送一个信号.int kill(pid,sig);

####    raise()
int raise(sig) == kill(getpid(),sig);   多进程.    pthread_kill(pthread_self(),sig);    多进程.

####    alarm() 是一个计时器.
unsigned int alarm(unsigned int seconds);以秒为单位及时,到了时间之后会发出时间信号,SIGALRM默认是杀死这个进程,但是可以使用signal()进行重定位信号
+   alarm是不能实现了多任务的计时器,他只能做一个任务的计时器.

####    pause() 可以让我们程序不进行忙等.
在我们的循环中加入一个pause(),可以让程序进行等待一个信号,而不是忙等,忙等大量占用CPU.
+   sleep --> alarm + pause ,是这样的一种封装.由于alarm只能实现最近一次,所以如果我使用了alarm信号 那么我的时钟信号就有问题.
+   usleep :微秒进行单位从而等待,这个就不是alarm + pause进行封装的了
+   unix中用的nanosleep进行的封装还好一些.因为是纳秒级别的函数所以一定不是alarm + pause进行封装的了,而且nanosleep存在于 time.h中,所以windows上可以移植.

##### 在C当中使用优化,他只会检测到自己内存中的数据,如果有信号那么他会优化错误,循环体中没有用到loop但是没有考虑到信号的影响

####    多任务的时钟定时器 setitimer | alarm
文件描述符就是给一个给定数组的下标.
+   alarm只能用秒来技术,而且每次只能用一个信号,对于时间的控制相对僵硬.
+   setitimer() | getitimer():
    +   int getitimer(int which,struct itimerval * value);
    +   int setitimer(int which,const struct itimerval * restrict value,struct itimerval *restrict ovalue);
        +   存在一个结构体struct itimerval:误差不累计
    +   其中int which的值
        +   ITIMER_REAL,时间的减少率是正常的,当时间耗尽了,一个SIGALRM信号就会被接受.
        +   ITIMER_VIRTUAL,下降率为进程的虚拟时间,这个时间只可以在进程中使用.
        +   ITIMER_PROF,这个时间设置在所有进程和系统级别中生效,重启之后可以使用,会发送SIGPROF信号(少用这玩意.).
```
           struct itimerval {
                   struct  timeval it_interval;    /* timer interval */
                   struct  timeval it_value;       /* current value */
           };

it_interval --> setitimer可以自己循环调用自身的间隔.
it_value --> setitimer 第一次调用的时间.
```
####    abort()
给当前进程发送一个SIGABRT信号,产生一个calldown文件.

####    system()
system() executes a command specified in command by call "/bin/sh -c".**During execution of the command,SIGCHLD will be blocked,and SIGINT and SIGQUIT will be ignored.**

####    sleep()
在某些平台下使用alarm + pause进行封装.
+   在macos下使用的nanosleep进行封装的.可以用nanosleep进行.
+   usleep() --> 使用的微秒为单位进行封装.
+   int select(int nfds,fd_set \* restrict readfds,fd_set \* restrict writefds,fd_set \* errorfds,struct timeval \*timeout);这个是用来IO多路转接的,但是超时设置也是可以进行时间控制的.
####    信号集
man sigemptyset.....

这些函数操作信号集,存储在sigset_t类型中



+   sigaddset
+   sigdelset
+   sigemptyset
+   sigfillset
+   sigismember

####    信号屏蔽字/pending集的处理
sigprocmask() --> 当前信号的状态,可以block信号,是位图的形式.给一种方式进行信号的控制.
```
int sigprocmask(int how,const sigset_t * set,sigset_t * oset);

how-->如何操作 SIG_BLOCK,SIG_UNBLOCK,SIG_SETMASK

```
我们无法控制信号何时发出但是可以控制信号可不可以被收到.

+   进入我们模块之前信号是什么样的,那么出去还是什么样,这是一个编程的思想.要保持调用恢复现场.
+   sigpending(sig_t * set);把我现在收到的信号拿回来(基本没啥用).

####    拓展
+   sigsuspend():atomically release blocked signals and wait for interrupt.
+   sigaction():可以用来替代signal这个函数,是一个可以让多个信号都使用的一个函数.如果使用signal()就会有重入的风险,信号是可以嵌套来调用的.多个信号来了之后就有可能会重复调用.
+   signal并不会区分信号的来源,使用signaction()就可以区分信号的来源.
####    getcontext
+  int getcontext(ucontext * ucp);
+  int setcontext(const ucontext * ucp);

#####   CTRL + \  : QUIT3 是程序退出
####    实时信号
为了弥补标准信号的不足,标准信号先响应,然后再调用实时信号.

信号处理函数,一般就用用系统调用,防止可重入发生.
---

## 多线程来实现并发
线程:说白了就是一个正在运行的函数,一个进程之中至少只有一个线程在进行,多开几个并发的函数就是多线程.<br>

其实我们可以不以main作为入口出口的,main函数也是一个线程,他们之前的关系是兄弟.

多线程的并发是现有的标准再有的实现,相对于更古老的信号,各个系统更加

POSIX线程是一套标准,而不是实现(openmp标准下的线程,也是约定了一种多线程的并发.它的并发不是从语言的角度),线程的标准不是实现,

+   线程的标识:pthread_t (POSIX THREAD)...具体是什么类型不知道它仅仅是一个标准不是一种实现方式.

+   进程就是一个容器,是用来承载线程的,线程号也是用进程号来进行标记的.

### 线程的创建
pthread_t类型具体是什么,我们也不知道,不要使用%d来打印thread!在不同环境下是不同的.
+   int pthread_create(pthread_t \*thread,const pthread_attr_t \*attr,void \*(\*start_routine)(void \*),void \*arg) 创建一个线程:线程的调度取决于调度器的策略,就有可能在创建的新线程还没有执行的时候就结束了.
```
[Running] cd "/Users/qilei/Desktop/APUE/thread/posix/" && gcc create1.c -o create1 && "/Users/qilei/Desktop/APUE/thread/posix/"create1
Begin!

End!


[Done] exited with code=0 in 0.134 seconds

[Running] cd "/Users/qilei/Desktop/APUE/thread/posix/" && gcc create1.c -o create1 && "/Users/qilei/Desktop/APUE/thread/posix/"create1
Begin!

End!

Thread is working!
```

####    线程的终止
+   线程终止的三种方式:
    1.  线程从启动历程返回,返回值就是线程的退出码.
    2.  线程可以被同一个进程中的其他线程取消.(算线程异常终止)
    3.  线程调用pthread_exit()函数. < -- 用这个! 不要*return NULL*,那样不能用清理栈.
    +   pthread_join(pthread_t thread,void \*\*value_ptr); --> 相当于wait(),但是可以指定线程,可以进行收尸.
    
+   栈的清理: 
    +   void pthread_cleanup_push(void (*routine)(void *)); ---> atexit() 挂钩子:
    +   pthread_cleanup_pop(int execute); ---> 取钩子,调用这个钩子函数.
    +   这两个函数必须**成对**出现,在预处理时候会有办个{ ; 后办个半个};在pop中.这一对对的push & pop必须要写,就算写在执行不到的地方也要写上,否则编译是不通过的.

+   线程的取消选项:
    <!--+   在取消的哪一个时刻,如果有线程的释放操作没有做到,那么我们可以用到pthread_cleanup_push | pop 来进行操作-->
    +   thread_cancel():这个函数用来在另一个线程还没有结束的时候发送一个取消请求.
        +   取消的两种状态:允许,不允许.(有点像sigmask设置后对于信号不响应.)--- 允许取消又分为*异步取消*,*推迟取消(默认的)*-->它会推迟到cancel点再取消.POSIX定义的cancel点,都是可能引发阻塞的系统调用.
    +   int pthread_setcancelstate(int state,int \*oldstate);用来设置是否允许取消.
    +   int pthread(int type,int \* oldtype);设置取消方式(异步-->死机,推迟-->一般都是这个)
    +   int pthread_testcancel(void);设置一个取消点.

+   线程的取消:例如一个二叉树10000层,我们在查找一个元素的时候,第一个线程就查找到了.后面的就不用在进行运行了我们就可以用到线程取消
    +   int pthread_cancel(pthread_t thread);从而我们可以先把线程给取消执行,再执行 int pthread_join(pthread_t pthread)来回收我们的多余的线程.

+   线程分离:
    +   int pthread_detach(pthread_t thread);

<small>在产生竞争的时候,多个线程可能会发生正常执行,有几率出错,而且每一次发现的结果是不一样的.</small>

在我们调试程序的时候sleep可以用来加大竞争冲突的概率,在等待的时候等于加长了操作的时间长度,更容易发生冲突

32位机器最多能开启300个左右的进程,因为在user态只能用到3G不到,我们的3G需要在当前内存空间中可以创建多少个10m大小的栈.如果我们要多创建,可以改变栈的大小.

64位环境内存可以128T的取值空间.

### 线程的同步
互斥量:(mutex)
+   int pthread_mutex_destory(pthread_mutex_t * mutex)

posix抽象了一个锁类型的概念,来保证共享数据的操作完整性,每个对象都是对应一个可称之为"互斥锁"的标记,这个标记用来保证在任意时刻只能有一个线程访问该对象.使用互斥锁可以使线程按顺序执行.通常互斥锁通过确保一次只有一个线程执行临界区来同步多个线程,互斥锁还可以保护单线程的代码,要更改互斥锁属性,可以对属性对象进行声明和初始化.**通常互斥锁属性会设置在应用程序开头的某个位置**.

1.   int pthread_mutex_init(pthread_mutex_t * restrict mutex,const pthread_mutexattr_t * restrict attr);
2.   pthread_mutex_t mutex = PTHREAD_MUTEX_INITALIZED

1中的函数是以动态的方式创建一个互斥锁,attr参数指定了互斥锁的属性,如果参数attr为NULL,则使用默认的互斥锁属性,默认属性为快速互斥锁.在LinuxThreads实现中仅有一个锁类型属性,不同的锁类型在试图对一个已经被锁定的锁枷锁的时候反应式不同的,如果初始化成功返回0,并且互斥锁为未锁定状态,2中的行为就是静态初始化.

互斥锁的属性是创建的时候就规定的,在Linux中仅有一个锁的属性,不同的所得类型在视图对一个已经被互斥锁枷锁时候表现不同,道歉有4个值可以选择
+   PTHREAD_MUTEX_TIMED_NP,这是默认值,也就是普通锁,当一个线程枷锁以后其余请求锁的线程将形成一个等待队列,并在解锁后按照优先级获得锁,这种锁的策略保障了资源分配的公平性.
+   PTHREAD_MUTEX_RECURSIVE_NP,嵌套锁,允许同一个线程对同一个锁成功获得几次,并且通过多次unlock解锁,如果是不同线程请求,则在枷锁解锁时候重新竞争.
+   PTHREAD_MUTEX_ERRORCHECK_NP,检错锁,如果同一个线程请求同一个锁,则返回EDEADLK,否则与PTHREAD_MUTEX_TIMED_NP类型动作相同,这样保证当允许多次加锁时不出现最简单情况下死锁
+   PTHREAD_MUTEX_ADAPTIVE_NP,适应所,动作最简单的锁的类型,仅等待解锁后重新竞争.

####    解锁操作
解锁的操作主要包括加锁lock,解锁unlock,尝试加锁trylock,无论哪种锁,都不可能被2个不同的线程同时得到,必须等待解锁,对于普通锁和适应锁类型,解锁线程可以是同进程内任何线程;而检错锁则必须由加锁者解锁材有效,否则会返回EPERM,对于潜逃所,文档和实现要求必须由加锁者解锁,但是实验表明并没有这种限制,在同一个进程中,如果加锁后没有解锁,则任何其他线程都无法获得锁.


在单核机器中,竞争不是很明显,可以使用sleep来放大竞争关系.

我们解决线程之间的竞争的时候需要引入**线程同步**
    +   互斥量: int pthread_mutex_init(pthread_mutex_t \* mutex,const pthread_mutexattr_t \* attr);--->This function creates a new mutex , with attributes specified with attr.If attr is NULL the default attributes are used.在我们初始化锁之后.我们可以使用下面几个函数对我们创建的锁进行操作:
        1.  pthread_mutex_destory
        2.  pthread_mutex_lock  阻塞
        3.  pthread_mutex_trylock   非阻塞
        4.  pthread_mutex_unlock
        5.  pthread_mutexattr
        +   在进入互斥区的时候lock 出互斥区的时候unlock.
    +   lock是阻塞的线程锁,其他的人都会被阻塞.在循环执行多个线程时候可以使用锁链.   在lock住之后,这个线程就不能跑了.之后在unlock之后才可以放行.

在我们的代码中如果有一块文件处理函数,一次只能允许一个线程|进程,进行处理,那么这块区域就叫做临界区,在抢占式的程序中,在我们进入临界区之前,我们要用互斥锁把这个区域的代码给锁上,在出来时候来再进行解锁.

```
/* mutex.c */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

/* 全局变量 */
int gnum = 0;
/* 互斥量 */
pthread_mutex_t mutex;

/* 声明线程运行服务程序. */
static void pthread_func_1(void);
static void pthread_func_2(void);

int main (void)
{
 /*线程的标识符*/
  pthread_t pt_1 = 0;
  pthread_t pt_2 = 0;
  int ret = 0;

  /* 互斥初始化. */
  pthread_mutex_init(&mutex, NULL);
  /*分别创建线程1、2*/
  ret = pthread_create(&pt_1,  //线程标识符指针
                       NULL,  //默认属性
                       (void*)pthread_func_1, //运行函数
                       NULL); //无参数
  if (ret != 0)
  {
     perror ("pthread_1_create");
  }

  ret = pthread_create(&pt_2, //线程标识符指针
                       NULL,  //默认属性
                       (void *)pthread_func_2, //运行函数
                       NULL); //无参数
  if (ret != 0)
  {
     perror ("pthread_2_create");
  }
  /*等待线程1、2的结束*/
  pthread_join(pt_1, NULL);
  pthread_join(pt_2, NULL);

  printf ("main programme exit!/n");
  return 0;
}

/*线程1的服务程序*/
static void pthread_func_1(void)
{
  int i = 0;

  for (i=0; i<3; i++) {
    printf ("This is pthread_1!/n");
    pthread_mutex_lock(&mutex); /* 获取互斥锁 */
    /* 注意，这里以防线程的抢占，以造成一个线程在另一个线程sleep时多次访问互斥资源，所以sleep要在得到互斥锁后调用. */
    sleep(1);
    /*临界资源*/
    gnum++;
    printf ("Thread_1 add one to num:%d/n", gnum);
    pthread_mutex_unlock(&mutex); /* 释放互斥锁. */
  }

  pthread_exit(NULL);
}

/*线程2的服务程序*/
static void pthread_func_2(void)
{
  int i = 0;

  for (i=0; i<5; i++)  {
    printf ("This is pthread_2!/n");
    pthread_mutex_lock(&mutex); /* 获取互斥锁. */
    /* 注意，这里以防线程的抢占，以造成一个线程在另一个线程sleep时多次访问互斥资源，所以sleep要在得到互斥锁后调用. */
    sleep(1);
    /* 临界资源. */
    gnum++;
    printf ("Thread_2 add one to num:%d/n",gnum);
    pthread_mutex_unlock(&mutex); /* 释放互斥锁. */
  }

  pthread_exit (NULL);
}
```

在我们线程函数内部加上一个lock来占用这个锁,通过函数内部占用绑定对应的线程,只有释放之后才可以继续调用
####    互斥的概念
在多线程的编程中,引入了对象互斥锁的概念,来保证共享数据操作的完整性,每个对象都对应于一个可称为"互斥锁"的标记,这个标记用来保证在任意时刻,~~只能有一个线程访问该对象~~

#### 多线程池类算法的竞争
分块,交叉分配
线程之间通信特别方便,所以用一个池类算法
+   sched_yield();出让调度器给别的线程,不能造成进程的颠簸:this function cases the calling thread to relinquish the CPU.The thread is moved to the end of the queue for its static priority abd a new thread gets to run.这个函数会让当前的线程让出CPU占有权,然后把线程放到静态优先队列的尾端,然后一个新的线程会占用CPU.简单的来讲就是让当前线程让出资源通过一定的调度策略让其他的线程也可以有机会运行到.
+   在临界区中一定要注意跳转函数,goto longjmp break [函数调用] ,都算是跳转,会造成死锁一定要解锁之后在进行跳转
```
pthread_mutex_unlock(&mut_num);
sched_yield();
pthread_mutex_lock(&mut_num);
---
先解锁,调度器让出线程.
```

池类算法是一种能者多劳的模式,比较科学的一种算法.


####    高性能并发系统的阻流(令牌桶)
常见的限流算法有两种:(1)漏桶算法,(2)令牌桶算法
+   漏桶算法:思路很简单,请求先进入到漏桶里面,漏桶以一定速度出水,当水流入速度过大只会溢出.
+   令牌桶算法:在写一个分布式服务的框架时候,对于分布式服务的框架来说除了远程调用,还要进行服务的治理,例如当进行双十一促销的时候所有的资源都用来完成总要的业务,主要任务就是让用户查询商品,购买支付,此时金币查询,积分查询等等业务就是次要的,要对这些服务进行降级处理,这就是典型的令牌桶算法,因此在写框架的时候一定要研究好令牌桶算法.在我们实施QOS(Quality of Service:服务质量)策略的时候,当用户的流量超过了额定的带宽的时候,超过的带宽将使用其他的方式进行处理,要衡量流量是否超过额定的带宽,网络设备并不是采用单纯的数字加减法来决定的,当网络设备衡量流量是否超过额定带宽时候需要查看令牌桶,令牌桶中会放置一些令牌,当同种没有令牌的时候任何流量都被视为超过额定带宽,只有当同种有令牌时,数据才可以通过接口,令牌桶中的令牌不仅仅被移除,同样也可以往里添加所以为了保证接口随时有数据通过,就必须不停地往桶里加令牌的速度,就决定了数据通过接口的速度,因此我们通过控制往令牌桶里加令牌的速度从而控制用户流量的带宽,在设置用户传输数据的速率被称作信息速率

####    条件变量(通知法)
pthread_cond_t 控制类型
+   pthread_cond_init(pthread_cond_t \*cond,const pthread_condattr_t \* attr);
```
SEE ALSO
     pthread_cond_broadcast(3), pthread_cond_destroy(3), pthread_cond_signal(3),
     pthread_cond_timedwait(3), pthread_cond_wait(3), pthread_condattr(3)
```

#####   等待和激发
使用pthread_cond_timedwait(3),可以进行非忙等的阻塞状态

使用pthread_cond_wait(3),可以进入非忙等的阻塞状态

无论哪种等待方式,都必须和一个互斥锁配合,以防止多线程请求pthrad_cond_wait()时候的京城条件,mutex必须是普通的互斥锁(NULL),在调用本线程的pthread_cond_wait()之前必须给线程加锁pthread_mutex_lock(),而且在更新条件等待队列之前,mutex必须保持锁定的状态,并在线程被挂起之前mutex解锁..........在条件满足之后可以离开pthread_cond_wait()之前,mutex必须重新进入锁定状态以进入pthread_cond_wait()前的加锁动作对应,**阻塞时候一定是处于解锁状态的**.

当我们要激发条件的时候有两种形式, pthread_cond_signal()激活一个等待该条件的线程,存在多个等待的入队顺序的时候则按照入队顺序逐个进行激活解锁,而broadcast()则会激活所有的线程.

####    信号量(pv)
信号量是区别与之前说到的互斥量mutex,mutex是独立出来的,而信号量并不是互斥的.

token ++ token --;
哲学家吃面问题作为例子

互斥量mutex可以理解为boolean,信号量可以理解为int类型 可以++ --;

信号量并没有单独的机制,在C程序中要用互斥量加信号量来完成这个操作,一切的基础是mutex.

减少资源量,有人需要更多的资源量,有人需要1个,所以要判断val >= 请求的资源量

####    读写锁
相当于互斥量 & 互斥量的相互协同的使用,在读写锁中有读锁|写锁,写是互斥的,读是信号的

### 线程属性
+   pthread_attr_init(pthread_attr_t \* attr);
+   pthread_attr_destroy(pthread_attr_t \* attr);
可以对进程的属性进行设置
```
     int
     pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);

     int
     pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);

     int
     pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);

     int
     pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);

     int
     pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);

    int
         pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
    
     int
     pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
```

对于线程属性pthread库中定义了线程属性的结构体
```
 typedef struct
    {
           int                           detachstate;     线程的分离状态
           int                          schedpolicy;   线程调度策略
           struct sched_param      schedparam;   线程的调度参数
           int                          inheritsched;    线程的继承性
           int                          scope;          线程的作用域
           size_t                      guardsize; 线程栈末尾的警戒缓冲区大小
           int                          stackaddr_set;
           void *                     stackaddr;      线程栈的位置
           size_t                      stacksize;       线程栈的大小
    }pthread_attr_t;
```
1.  分离状态:线程的分离状态主要决定了以什么样的方式来终结自己.在**默认的情况下**,线城市使用非分离状态来终结自己的,只有当pthread_join()函数进行收回之后,线程才算是完全终止并且释放了自己占用的系统资源.**通俗的来说**,join得意义就是我们想知道它的结束状态,如果我们不想知道那么这种无意义的等待回收是毫无意义的,所以我们可以使用"分离"属性,线程无须等待管理,只要线程自己结束那么资源也就释放了
    +   int pthread_attr_getdetachstate(const pthread_attr_t \* attr,int \* detachstate);
    +   int pthread_attr_setdetachstate(pthread_attr_t \* attr,int detachstate);
    +   PTHREAD_CREATE_DETACHED 分离状态
    +   PTHREAD_CREATE_JOINABLE 正常启动
2.  继承状态:使用函数pthread_attr_setinheritsched和pthread_attr_getinheritsched分别用来设置和得到线程的继承性.默认的集成设置里面并不是继承,继承性确定调度的参数是**创建线程中继承**还是在**schedpolicy和scheparam属性**中显示设置的调度信息
    +   调度策略:
        +   SCHED_FIFO:先进先出
        +   SCHED_RR:轮转法
        +   SCHED_OTHER:其他
    +   调度参数:
        +   本身参数就是优先级:policy越大优先级越大
3.  线程的作用域:作用域控制线程是否在进程内或在系统级上竞争资源,PTHREAD_SCOPE_PROCESS(进程内竞争资源),PTHREAD_SCOPE_SYSTEM(系统竞争资源).
    +   int pthread_attr_getscope(const pthread_attr_t \* attr,int \* scope);
    +   int pthread_attr_setscope(pyhread_attr_t \*, int scope);
4.  线程堆栈的大小:stacksize
5.  线程堆栈地址
6.  警戒缓冲区:guardsize控制着线程栈尾以后以避免栈溢出的扩展内存大小,这个属性默认设置为PAGESIZE个字节,可以把guardsize栈属性设置为0,从而不允许这种特征行为发生,在这种情况下不会提供警戒缓冲区,同样的,如果对线程stackaddr做了修改,系统会自己认为我们会管理栈,并使这种警戒缓冲区机制无效,等同于guardsize==0.

####    线程创建
线程的创建比进程的创建要省事儿,进程的创建需要重新创建一个C的环境出来,线程相当于一个进程中的一个函数,相对于进程来说资源的消耗&文件的访问&多并发之间的通信都是很方便的.

####    互斥量的属性
1.  pthread_mutexattr_init()
2.  pthread_mutexattr_destroy()
3.  pthread_mutexattr_getpshared()
4.  pthread_mutexattr_setpshared()
5.  clone()-->mac中没有,只有在linux有,也是创建一个子*进程*,父子进程可以共享一个描述符.

控制互斥量是否可以跨进程来获取.包括**控制量**等等都有这种操作

### 重入
posix规定如果这个库发布出来了,如果不支持多线程并发,就要标记出来.puts 就是支持的,这个函数在操作缓冲区的时候会锁住缓冲区在进行unlock.

getchar就不行,如果我们写的单进程版本就要加上**unlocked版本**.

####    线程与信号
每一个线程都有自己的一个mask 和 pending
+   pthread_sigmask(int how,const sigset_t \* set,sigset \*)
+   pthread_kill
+   sigwait()

####    线程与fork
openmp --> www.OpenMP.org 另一种线程的标准

posix是一个线程的实现标准,定义的是实现的方法,C语言本身不支持并发,所以在函数级别有一定的操作调用.

openmp是一套通过编译器进行的并发的C语言标准,在GCC 4.0 以后可以使用openmp语法标记,是跨语言的比较简单的一套并发标准.
---
+   pthread_equal() 比较2个线程号是不是相同.
+   pthread_self() 返回线程的标识.

##  高级IO
在我们学习了线程,进程,信号等机制之后就可以进一步的理解IO,IO是我们实现的基础.很多东西可以去查man 7章节,这里面的东西是讲的机制.
+   非阻塞IO:try to do 
+   阻塞IO:waiting till we do.

对于我们的IO有2个错误比较坑,ERRINT 由于我们的设备比较慢会告诉你被打断了.EAGIN不是真正意义的出错,尝试去做没有成功.这2个都是假错.

高级IO主要研究就是非阻塞的IO,*补充*:有限状态机的编程思想.

### 非阻塞IO
数据中继,2个设备操作过程为:rl-wr-rr-wl,多任务来完成这种操作,端有数据哪一端就先走.很多数据的拦截模型就是数据中继模型,让数据过一下我的服务端.其实就是之前写的mycpy.

### 有限状态机
简单流程:如果一个程序的自然流程是结构化的,那么就叫做简单流程

复杂流程:如果一个程序的自然流程不是结构化的,那么就是复杂流程.

自然流程:用人类正常的顺序选择循环.

复杂流程往往都是复杂流程.C是逻辑化的编程语言,C++是模块化的编程语言[类的思想是非常适合人类的思想的方式].

####    文件描述符控制函数
fcntl()  F_GETFL & F_GETFL 

### IO多路转接
监视文件描述符的行为,当文件描述符符合行为才进行操作.实现**非忙等**的程序.
+   select();移植性很好,比较古老.int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,fd_set *restrict errorfds, struct timeval *restrict timeout);
    +   可以用select()做一个安全的休眠select(-1,NULL,NULL,NULL,time);没有给这个描述符控制函数具体任务,只是让他等一会.
    +   缺点:监视的现场和监视的任务在一块空间,监视一次之后就会自己释放.文件描述符是int类型所以文件描述符只能是用正数的部分而且不能超过这部分Max(fd1,fd2)+1.监视的时间单一,只有读写
+   int poll(struct pollfd fds[],nfds_t nfds,int timeout);一文件描述符为单位查看是否可以开始这件时间.
+   epoll();是linux的高效优化版本的poll().

网络IO的本质socket的读取,socket在linux系统被抽象为了流的概念,流可以被作为对象操作.对于一次访问IO(以read举例),数据会先被拷贝到操作系统内核缓冲区中,然后才会从操作系统内核缓冲区拷贝到应用程序的地址空间,所以说当一个read法师时候他会经历:
    1.  等待数据准备
    2.  将数据拷贝到进程中
    
对于socket流来说:
    1.  通常涉及等待网络上的数据分组到达,然后复制到内核的某个缓冲区.
    2.  把数据从内核缓冲区复制到应用进程缓冲区.

其实网络应用处理的问题无非两大类:网络IO,数据计算,相对于后者来说,网络IO的延迟给应用带来的性能瓶颈大于后者,网络IO的模型大致有如下特点:
+   同步IO:
    1.  阻塞IO
    2.  非阻塞IO
    3.  多路复用IO
    4.  信号驱动IO
+   异步IO(asynchronous IO)

####    阻塞IO
进程会一直阻塞,直到数据拷贝完成.阻塞的意义就是CPU"被"休息,用户空间的app 执行了系统调用(recvform),这回导致系统应用程序的阻塞什么也不干直到数据准备好,因此从处理的角度来看是非常有效的.
####    非阻塞IO
非阻塞IO通过进程反复调用IO函数(多次系统调用,马上返回);在数据拷贝过程中,进程是阻塞的,同步非阻塞就是"每过一会就瞄一眼进度条(polling)轮询法方式,这种模型中,设备是以非阻塞打开的这意味这IO操作不会立即返回,read操作可能会返回一个错误代码,这说明不能立即满足(EAGAIN EWOULDBLOCK);

在网络IO时候,非阻塞也会进行recvfrom的系统调用,检查数据是否准备好.与阻塞IO不一样,"非阻塞"将大的整片时间的阻塞分解成多个小的阻塞,所以进程有机会不断地被CPU光顾.

####    多路复用IO
主要是select和poll;对一个IO端口,两次调用两次返回,最大的优点就是可以同时监听多个IO端口进行监听

####    信号驱动IO
两次调用,两次返回,允许socket进行信号驱动IO,并安装一个信号处理函数,进程继续运行并不阻塞,当数据准备好了,进程会收到一个SIGIO的信号,可以在信号处理函数中调用IO操作处理数据.
####    异步IO
数据进行拷贝的时候无需阻塞,相对于同步IO,异步IO并不是顺序的,用户进程进行aio_read系统调用之后,无论内核数据是否准备好都会直接返回给用户进程,然后用户态进程可以去做别的事情,等到socket数据准备好了,内核直接复制数据给进程,然后从内核向进程发送通知,IO两个阶段都是非阻塞的:
    
### 其他读写
readv(),writev()读写一些字符向多个地址

## 存储映射IO(挺有用的)
void * mmap(void *addr,size_t len,int prot,int flags,int fd,off_t offset);根据man手册说明,mmap是从addr开始到len个字节结束,向fd描述符输入这些内存的数据,使用mmap可以做一个非常快的共享内存.

munmap() 相当于free()函数.

小结:mmap函数是讲一个文件或其他内存对象映射进内存,文件被映射到多个页上,如果文件大小不是所有页的大小之和,最后一个页不会使用并且清零.
+   补充:内存分页机制.在保护模式中,内存的访问使用的是分段机制--段基地址:段内偏移地址,为了加强段内存的保护和可管理性.还引入了*段描述符*的概念对内存进行管理,但是还是不能满足需求,计算机技术迭代,应用程序越来越多,内存碎片也随之增加,对于内存的换入换出,引出了内存的置换.
+   在大多数情况下,将一些进程的不常用的内存段换入换出来腾出更多的内存空间,虽然硬盘是低速设备,在这两种设备之间交换内存是对时间的一种浪费,但是使用合理的置换算法
---
这个函数做有亲缘关系的进程之间的通信是非常好的..

### 文件锁
限制文件的操作,给文件加锁.
+   fcntl():改变打开文件的性质,针对文件描述符的控制,可以对文件描述符加锁,还可以记录对某一个文件的某一个记录上锁,也就是记录锁.
+   lockf():课程主要讲的是这个函数,函数原型是int lockf(int fildes,int function,off_t size)参数分别为文件描述符 | 功能选项(F_ULOCK F_LOCK F_TLOCK F_TEST) | 从零开始的偏移量是多少[锁 & 解锁]
+   flock()

##  进程间的通信
数组顺序存储的队列来扩大空间,加大上游的任务,这个队列叫做管道.unix内核为我们提供了一个管道,这个管道适合不同进程通信,在我们的线程通信中,还是自定义临界区比较方便而且效率更高..
    
    
### 管道通信方式
####    匿名管道 p
类似[tmpfile 直接给我返回一个file * 我就直接操作就行了],拿着标识符我可以操作,但是如果进程没有血缘关系是不能进行通信的.

pipe(),pipe2()
####    命名管道 p
我们可以在磁盘上找到这个文件.管道是单工通信的,自同步机制.阻塞->等||判断写者是否存在,流速控制

开头为p的文件就是命名管道:int mkfifo() 函数,

terminal 中有一个*mkfifo*创建一个管道文件,必须凑齐读写双方才能进行.

**管道是单工的**,如果希望实现一个双工通信要用至少2个管道
### XSI  ->  SysV 
IPC -> Inter-Process Communicaiton.
```
qilei@bogon APUE % ipcs
IPC status from <running system> as of Tue Jul 21 14:18:10 CST 2020
T     ID     KEY        MODE       OWNER    GROUP
Message Queues:

T     ID     KEY        MODE       OWNER    GROUP
Shared Memory:

T     ID     KEY        MODE       OWNER    GROUP
Semaphores:
s  65536 0x6104a436 --ra-ra-ra-    qilei    staff
```
KEY:没有亲缘关系的进程的通信ftok();~~这一组函数特点就是xxxget 创建对象,xxxop 对对象操作,xxxctl 控制|初始化  shm->share memory ,msg,sem,~~
+   消息队列:msg
+   信号量数组:ftok->semget->semctl->semop,在资源抢占的时候应该合理的分配资源量,不满足的量可以继续向后找是否有可以满足条件的任务.不对其资源进行顺序的独占.

### Socket通信

#### 跨主机的信息传输
字节序:大端低地址放高字节|小端低地址放低字节,因此我们来区分主机host字节序和网络network字节序[_ to _ _ : htons,htonl,ntohs,ntohl,16位s,32位l]

对齐:例如结构体
```
struct {
    int i;
    char ch;
    float f;
};
会占用12字节,需要对齐,0%4[0-3],0%1[4],8%4[8-11],因此出现了567 3个字节空白的空间.
```
我们需要告诉编译器不对齐:gcc中结构体默认是4个字节对齐，即为32的倍数。
```
修改字节对齐：

struct data{
int a;
char b;
char c;
}__attribute__((aligned(8))) data;

或

#pragma pack(8)
struct data{
int a;
char b;
char c;
}data;
#pragma pack() 
```
综上所述：用来对齐的，包括栈对齐，变量地址对齐内存分配对齐的几种方式如下：

+   #pragma pack(n):n的取值可以为1、2、4、8、16，在编译过程中按照n个字结对齐
+   #pragma pack():取消对齐，按照编译器的优化对齐方式对齐
+   __attribute__ ((packed));:是说取消结构在编译过程中的优化对齐。
+   __attribute__ ((aligned (n)));:让所作用的成员对齐在n字节自然边界上,如果结构中有成员的长度大于n，则按照最大成员的长度来对齐

+   __attribute__(())是GNU CC的一大特色,这个机制可以设置函数属性,变量属性,类型属性,==语法格式为 __attribute__((attributr-list))==,放于声明的尾部之前:
    1.  aligned(4) 对齐到4个字节
    2.  packed 不对齐
    3.  format:该属性可以给声明的函数上加上类似printf或者scanf的特征,他可以使编译器检查函数声明和函数实际调用参数之间的格式化字符串是否匹配,* format (archetype, string-index, first-to-check)*


---
C语言在定义字符长度的时候并没有明确规定,int32_t,int64_t,uint32_t,char == int8_t

socket是一个中间层,IPv4协议,IPX协议,协议族来指定,可以用流式传输,包式传输.socket都支持,socket把上下传输都抽象成一个文件描述符.最后可以使用stdio封装成为一个FILE \* 来进行操作.

AF_UNIX,AF_LOCAL 本地协议
AF_PIX Novell-->古董
AF_NETLINK 一个内核接口,做一些系统调用,内核的代码可以使用报式形式进行通信
AF_PACKET 底层协议,抓包器用的就是这个.

#### 报式套接字
被动端:
1.  取得socket
2.  给socket取得地址
3.  收发消息
4.  关闭socket

主动端:
1.  取得socket
2.  给socket取得地址
3.  收发消息
4.  关闭socket

被动端先运行,2步骤给socket绑定一个地址.
[在这其中有一个麻烦,我的macos系统的man手册不是很齐全]我摘抄了一部分网上的资料.

+   sockaddr其定义如下：

```
struct sockaddr {
　　unsigned short sa_family; /* address family, AF_xxx */
　　char sa_data[14]; /* 14 bytes of protocol address */
　　};
```
说明：

sa_family ：是2字节的地址家族，一般都是“AF_xxx”的形式，它的值包括三种：AF_INET，AF_INET6和AF_UNSPEC。

如果指定AF_INET，那么函数就不能返回任何IPV6相关的地址信息；如果仅指定了AF_INET6，则就不能返回任何IPV4地址信息。

AF_UNSPEC则意味着函数返回的是适用于指定主机名和服务名且适合任何协议族的地址。如果某个主机既有AAAA记录(IPV6)地址，同时又有A记录(IPV4)地址，那么AAAA记录将作为sockaddr_in6结构返回，而A记录则作为sockaddr_in结构返回通常用的都是AF_INET。

+   sockaddr_in其定义如下：

```
struct sockaddr_in {
　　short int sin_family; /* Address family */
　　unsigned short int sin_port; /* Port number */
　　struct in_addr sin_addr; /* Internet address */
　　unsigned char sin_zero[8]; /* Same size as struct sockaddr */
　　};
```

+   sin_family：指代协议族，在socket编程中只能是AF_INET
+   sin_port：存储端口号（使用网络字节顺序）
+   sin_addr：存储IP地址，使用in_addr这个数据结构
+   sin_zero：是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。

+   而其中in_addr结构的定义如下：

```
typedef struct in_addr {
　　union {
　　struct{ unsigned char s_b1,s_b2, s_b3,s_b4;} S_un_b;
　　struct{ unsigned short s_w1, s_w2;} S_un_w;
　　unsigned long S_addr;
　　} S_un;
　　} IN_ADDR;
```

struct sockaddr 这种结构体是捏造出来的,本来就没有这种类型.

阐述下in_addr的含义，很显然它是一个存储ip地址的共用体有三种表达方式：
+   第一种用四个字节来表示IP地址的四个数字；
+   第二种用两个双字节来表示IP地址；
+   第三种用一个长整型来表示IP地址。给in_addr赋值的一种最简单方法是使用inet_addr函数，它可以把一个代表IP地址的字符串赋值转换为in_addr类型，如addrto.sin_addr.s_addr=inet_addr("192.168.0.2");

其反函数是inet_ntoa，可以把一个in_addr类型转换为一个字符串。

##### 转换函数 htons() 将整形变量从主机的字节序转换为网络字节序,整数的高位字节存放在低字节的地址空间,也就是小端序

##### 点分式与数值转换 int inet_pton(int af,)

在报文套接字中使用recvfrom来接受,要知道从哪里来的,而流式套接字只需要recv就行了

sendto 中有一个flag:MSG\_OOB"带外数据"将数据放在下一个可用缓冲区中,并且设置紧急指针指向下一个可用的缓冲区空间
+   这个是发送和接收数据时的一个选择值，如果用它则表示需要发送和接收带外数据。带外数据（也称为TCP紧急数据），可以在应用程序中用来处理紧急数据。协议的实现为了提高效率，往往在应用层传来少量的数据时不马上发送，而是等到数据缓冲区里有了一定量的数据时才一起发送，但有些应用本身数据量并不多，而且需要马上发送，这时，就用紧急指针，这样数据就会马上发送，而不需等待有大量的数据

被动端一定要先bind,告诉主机我是谁....

#### 动态报式套接字
UDP数据包的推荐长度是512字节,边长的报文要使用指针来填写结构体,这样可以realloc

#### 多点通信
广播(全网广播,子网广播)|多播|组播

+   全网广播:255.255.255.255(受限的广播地址),默认是关闭的,但是可以打开这个开关SO_BROADCAST,使用int setsockopt(int socket,int level,int option_name,void \*opt_val,socklen_t len)进行设置
    +   SO_BINDTODEVICE:绑定一个网卡
    +   该地址用于主机配置过程中IP数据报bai的目的地址。此时，主机可能还du不知道它所在网络的网络掩码，甚至连它的IP地址也不知道。在任何情况下，路由器都不转发目的地址为受限的广播地址的数据报，这样的数据报仅出现在本地网络中。

防止内存数据泄露.memset calloc

```
//sockaddr_in 解决了linux原生的地址信息的问题,sockaddr结构体把地址和端口分开保存

struct sockaddr_in{
    sa_family_t sin_family; //8
    uint16_t sin_port;  //16
    struct in_addr sin_addr; //32
    char sin_zero[8]; //8
}__attribute__((packed));
```


#####   UDP的特点
可以实现多点通信

+   socket()
+   bind()
+   sendto()
+   rcvfrom()
+   inet_pton() : 点分式 -> 网络数 proto -> num
+   inet_ntop():    网络数 -> 点分式 num -> proto
+   getsockopt()
+   setsockopt()


TTL(Time TO Live):linux64 windows128,可以跳转多少个路由器

#### 流式套接字TCP
主动端C:
1.  获得SOCKET:socket()
2.  给SOCKET取得地址:connect()
3.  发送链接:send()
4.  收发消息:recv
5.  关闭

S段(被动端):
1.  获得SOCKET:socket()
2.  给SOCKET取得地址:bind()
3.  SOCKET设置为监听模式:listen()
4.  接受链接:accept()
5.  收发消息
6.  关闭

