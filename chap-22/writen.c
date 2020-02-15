
// 实战中，可以不用区分阻塞和非阻塞IO
// 使用循环的方式写入数据就好，只不过在阻塞IO的情况下，循环只执行一次就结束


// 向文件描述符 fd 写入n字节数
ssize_t writen(int fd, const void *data, size_t n)
{
	size_t   nleft;
	ssize_t  nwritten;
	const char  *ptr;
	
	ptr = data;
	nleft = n;
	
	// 如果还有数据没被拷贝完成，就一直循环
	while(nleft > 0)
	{
		if( (nwritten = write(fd, ptr, nleft)) <=0)
		{
			 /* 这里EAGAIN是非阻塞non-blocking情况下，通知我们再次调用write() */
			 if(nwritten < 0 && errno == EAGAIN)
			 {
				 nwritten = 0;
			 }
			 else
			 {
				 return -1;    // 出错退出
			 }
		}
		
		// 指针增大，剩余字节数减少
		nleft -= nwritten;
		ptr += nwritten;
	}
	
	return n;
		
}



