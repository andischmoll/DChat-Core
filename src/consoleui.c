/** @file consoleui.c
 *  
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dchat_h/consoleui.h"

 // log level
static int level_ = LOG_DEBUG;
static const char* flty_[8] = {"emerg", "alert", "crit", "err", "warning", "notice", "info", "debug"};

/**
 * Initializes input, output and log filedescriptor.
 * @param cnf Pointer to global config
 * @return 0 on success, -1 in case of error
 */
int
init_ui(dchat_conf_t* cnf){
    /*cnf->in_fd            = 0;    // use stdin as input source
    cnf->out_fd           = 1;    // use stdout as output target
    cnf->log_fd           = 1;    // use stdout as log target*/
    cnf->in_fd = 0;

    if((cnf->out_fd = init_socket("dchat_out")) == -1)
        return -1;

    if((cnf->log_fd = init_socket("dchat_log")) == -1)
        return -1;

    return 0;
}

int
init_socket(char* file){
    int len, sock, fd;
    struct sockaddr_un server;

    char path[strlen(TMP_PATH)+strlen(file)+1];

    sprintf(path, "%s%s", TMP_PATH, file);

    unlink(path);

    sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if(sock < 0){
        perror("opening socket");
        return -1;
    }

    server.sun_family = PF_LOCAL;
    strcpy(server.sun_path, path);
    if(bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))){
        perror("binding socket");
        return -1;
    }

    printf("waiting %s\n", server.sun_path);
    fflush(stdout);

    if(listen(sock, 1)){
        perror("listening socket");
        return -1;
    }
    if((fd = accept(sock, 0, 0)) == -1){
        perror("acepting socket");
        return -1;
    }

    return fd;
}

/**
 * Write recived message to UI.
 * @param fd File descriptor where the message will be written to
 * @nickname Nickname of the client from whom we received the message
 * @msg Text message to print
 * @return number of of characters printed. Negative value if on output error occurs
*/
 int
 ui_write(int fd, char* nickname, char* msg){
    return dprintf(fd, "%s;%s\n", nickname, msg);
 }




 /**
 *  Log a message to a filedescriptor.
 *  @param fd File descritpor where the log will be written to
 *  @param lf Logging priority (equal to syslog)
 *  @param fmt Format string
 *  @param ap Variable parameter list
 *  @param with_errno Flag if errno should be printed too
 */
void
vlog_msgf(int fd, int lf, const char* fmt, va_list ap, int with_errno)
{
    int level = LOG_PRI(lf);
    char buf[1024];

    if (level_ < level)
    {
        return;
    }

    if (fd > -1)
    {
        dprintf(fd, "%s;", flty_[level]);
        vdprintf(fd, fmt, ap);

        if (with_errno)
        {
            dprintf(fd, " (%s)", strerror(errno));
        }

        dprintf(fd, "\n");
    }
    else
    {
        vsnprintf(buf, sizeof(buf), fmt, ap);
        syslog(level | LOG_DAEMON, "%s", buf);
    }
}


/**
 *  Log a message to a filedescriptor.
 *  @param fd File descriptor where the log will be written to
 *  @param lf Log priority
 *  @param fmt Format string
 *  @param ... arguments
 */
void
ui_log(int fd, int lf,const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vlog_msgf(fd, lf, fmt, ap, 0);
    va_end(ap);
}

/**
 *  Log a message together with a string representation of errno as error.
 *  @param fd Filedescriptor where the log will be written to
 *  @param lf Log priority
 *  @param fmt Format string
 *  @param ... arguments
 */
void
ui_log_errno(int fd, int lf, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vlog_msgf(fd, lf, fmt, ap, 1);
    va_end(ap);
}

/**
 * Prints an error message and terminates this program.
 * @param fd File descriptor where the error will be written to
 * @param fmt Format string
 * @param ... Arguments
*/
void
ui_fatal(int fd, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlog_msgf(fd, LOG_ERR, fmt, args, 0);
    va_end(args);
    exit(EXIT_FAILURE);
}

/**
 * Prints usage of the program to stdout and log filedescriptor
 * @param fd Log file descriptor
 * @param exit_status Status of termination
 * @param options Array of options supported
 * @param Format string
 * @param ... arguments
 */
void
usage(int fd, int exit_status, cli_options_t* options, const char* fmt, ...)
{
    if (strlen(fmt))
    {
        va_list args;
        va_start(args, fmt);
        vlog_msgf(STDOUT_FILENO, LOG_ERR, fmt, args, 0);
        vlog_msgf(fd, LOG_ERR, fmt, args, 0);
        va_end(args);
    }
    print_usage(STDOUT_FILENO, exit_status, options);
    print_usage(fd, exit_status, options);
}

/**
 * Prints usage of this program.
 * @param fd File descriptor where the log will be written to
 * @param exit_status Status of termination
 * @param options Array of options supported
 */
void
print_usage(int fd, int exit_status, cli_options_t* options)
{
    dprintf(fd, "\n");
    dprintf(fd, " %s", PACKAGE_NAME);

    for (int i=0; i < CLI_OPT_AMOUNT; i++)
    {
        if (options->option[i].mandatory_option)
        {
            if (options->option[i].mandatory_argument)
            {
                dprintf(fd, " -%c %s", options->option[i].opt, options->option[i].arg);
            }
            else
            {
                dprintf(fd, " -%c", options->option[i].opt);
            }
        }
        else
        {
            if (options->option[i].mandatory_argument)
            {
                dprintf(fd, " [-%c %s]", options->option[i].opt, options->option[i].arg);
            }
            else
            {
                dprintf(fd, " [-%c]", options->option[i].opt);
            }
        }
    }

    dprintf(fd, "\n\n");
    dprintf(fd, " Options:\n");

    for (int i=0; i < CLI_OPT_AMOUNT; i++)
    {
        dprintf(fd, "%s\n\n", options->option[i].description);
    }

    dprintf(fd,
            " More detailed information can be found in the man page. See %s(1).\n",
            PACKAGE_NAME);
    exit(exit_status);
}