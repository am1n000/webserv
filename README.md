# webserv

## Example config

```conf
  server {
        root /Users/hchakoub/cursus/webserv/ 
        index index.php index.html skfkf
        server_name test
        listen      127.0.0.1:8080 
        error_page page
        client_body_limit 100




        location  / { 
            root /Users/hchakoub/cursus/webserv/ressources/
            upload_dir /Users/hchakoub/cursus/webserv/ressources/uploads/
            index index.php hhhhhhhh
            allowed_methods GET POST DELETE
            cgi php /Users/hchakoub/.local/bin/php-cgi
            redirection https://otossa.com
            auto_indexing on
           }

        location  /shop-website/ { 
            root /Users/hchakoub/cursus/webserv/ressources/shop-website
            index index.html hhhhhh
            allowed_methods GET POST DELETE
            cgi php /Users/hchakoub/.local/bin/php-cgi
            redirection https://otossa.com
            auto_indexing on
           }

  }
```
