# -d 启动一个守护进程
# -m 分配给memcache的内存，单位是MB，默认是64MB
# -u 用户
# -l 监听的服务器IP
# -p 监听的服务器端口，默认是11211
# -c 允许的最大并发连接数，默认是1024
# -P PID文件

memcached -m 64 -u root -l 127.0.0.1 -p 11211 -c 128 -P /tmp/memcached.pid
