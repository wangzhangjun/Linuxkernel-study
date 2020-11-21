allofit:  userprogs threadprogs

userprogs:
	@$(MAKE) \
		CFLAGS="-O2 -Wall -pedantic" \
	 lab53_client lab53_server_epoll lab53_server_fork lab53_server_poll lab53_server_select

threadprogs:
	@$(MAKE) \
		CFLAGS="-O2 -Wall -pedantic -pthread" \
	 lab53_server_pthread

clean:
	rm -rf  lab53_client lab53_server_epoll lab53_server_fork lab53_server_poll lab53_server_select  lab53_server_pthread
