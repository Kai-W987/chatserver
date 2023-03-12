#配置stream块
stream {
    upstream MyServer {
        server 127.0.0.1:5100 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:5200 weight=1 max_fails=3 fail_timeout=30s;
    }
    server {
        proxy_connect_timeout 1s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}
