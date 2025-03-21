#include "globals.h"
#include <sys/epoll.h>

#define MAX_EVENTS	256	/* 一次处理的最大的事件 */

/* epoll structs */
static int kdpfd;
static struct epoll_event events[MAX_EVENTS];
static int epoll_fds = 0;
static unsigned *epoll_state;	/* 保存每个epoll 的事件状态 */

static const char *
epolltype_atoi(int x)
{
	//将控制类型输出为字符串，方便阅读
    switch (x) {

    case EPOLL_CTL_ADD:
	return "EPOLL_CTL_ADD";

    case EPOLL_CTL_DEL:
	return "EPOLL_CTL_DEL";

    case EPOLL_CTL_MOD:
	return "EPOLL_CTL_MOD";

    default:
	return "UNKNOWN_EPOLLCTL_OP";
    }
}

void do_epoll_init(int max_fd)
{

    
    kdpfd = epoll_create(max_fd);
    if (kdpfd < 0)
	  fprintf(stderr,"do_epoll_init: epoll_create(): %s\n", xstrerror());
    //fd_open(kdpfd, FD_UNKNOWN, "epoll ctl");
    //commSetCloseOnExec(kdpfd);

    epoll_state = calloc(max_fd, sizeof(*epoll_state));
}


void do_epoll_shutdown()
{
    
    close(kdpfd);
    kdpfd = -1;
    safe_free(epoll_state);
}



void epollSetEvents(int fd, int need_read, int need_write)
{
    int epoll_ctl_type = 0;
    struct epoll_event ev;

    assert(fd >= 0);
    debug(5, 8) ("commSetEvents(fd=%d)\n", fd);

	memset(&ev, 0, sizeof(ev));
    
    ev.events = 0;
    ev.data.fd = fd;

    if (need_read)
	ev.events |= EPOLLIN;

    if (need_write)
	ev.events |= EPOLLOUT;

    if (ev.events)
	ev.events |= EPOLLHUP | EPOLLERR;

    if (ev.events != epoll_state[fd]) {
	/* If the struct is already in epoll MOD or DEL, else ADD */
	if (!ev.events) {
	    epoll_ctl_type = EPOLL_CTL_DEL;   ///Epoll事件为0，需要删除
	} else if (epoll_state[fd]) {          //之前已经存在
	    epoll_ctl_type = EPOLL_CTL_MOD;    
	} else {              //是新增事件
	    epoll_ctl_type = EPOLL_CTL_ADD;
	}

	/* Update the state */
	epoll_state[fd] = ev.events;

	if (epoll_ctl(kdpfd, epoll_ctl_type, fd, &ev) < 0) {
	    debug(5, 1) ("commSetEvents: epoll_ctl(%s): failed on fd=%d: %s\n",
		epolltype_atoi(epoll_ctl_type), fd, xstrerror());
	}
	switch (epoll_ctl_type) {
	case EPOLL_CTL_ADD:
	    epoll_fds++;
	    break;
	case EPOLL_CTL_DEL:
	    epoll_fds--;
	    break;
	default:
	    break;
	}
    }
}

int do_epoll_select(int msec)
{
    int i;
    int num;
    int fd;
    struct epoll_event *cevents;

    /*if (epoll_fds == 0) {
	assert(shutting_down);
	return COMM_SHUTDOWN;
    }
    statCounter.syscalls.polls++;
    */
    num = epoll_wait(kdpfd, events, MAX_EVENTS, msec);
    if (num < 0) {
	getCurrentTime();
	if (ignoreErrno(errno))//忽略一些不严重的错误
	    return COMM_OK;

	debug(5, 1) ("comm_select: epoll failure: %s\n", xstrerror());
	return COMM_ERROR;
    }
    //statHistCount(&statCounter.select_fds_hist, num);

    if (num == 0)
	return COMM_TIMEOUT;

    for (i = 0, cevents = events; i < num; i++, cevents++) {
		fd = cevents->data.fd;
		comm_call_handlers(fd, cevents->events & ~EPOLLOUT, cevents->events & ~EPOLLIN);
    }

    return COMM_OK;
}
