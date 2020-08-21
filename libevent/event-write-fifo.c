#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>

#define BUF_LEN (1024)

static void
fifo_write(evutil_socket_t fd, short event, void *arg)
{
	char buf[BUF_LEN] = {0};
	struct event *ev = (struct event*)arg;

	printf("fifo_write called with fd: %d, event: %d, arg: %p\n",(int)fd, event, arg);
    
    static int time = 0;
    sprintf( buf, "This is No.%d time write!\n", ++time);

	int len = write( fd, buf, strlen(buf));

	if (len <= 0) {
		if (len == -1)
			perror("write");
		else if (len == 0)
			printf("Connection closed\n");
		event_del(ev);
		event_base_loopbreak(event_get_base(ev));
		return;
	}

	fprintf(stdout, "Write: %s\n", buf);

    sleep(1);
}

int
main(int argc, char **argv)
{
	const char *fifo = "event.fifo";
    
    int fd = open( fifo, O_WRONLY | O_NONBLOCK);
	if ( fd == -1) {
		perror("open error");
		exit(1);
	}
    
	/* Initialize the event library */
	struct event_base *base = event_base_new();

	/* Initialize one event */
    struct event *event = event_new( base, fd, EV_WRITE|EV_PERSIST, fifo_write, event);

	/* Add it to the active events, without a timeout */
	event_add( event, NULL);

    /* event base loop */
	event_base_dispatch(base);
    
    /* exit loop free */
	event_base_free(base);
    event_free(event);
	close(fd);

    /* close evet base */
	libevent_global_shutdown();
	return (0);
}

