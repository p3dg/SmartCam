

copy_to_dir: all
	cp status_image.cgi /var/www/cgi
	chmod +x /var/www/cgi/status_image.cgi
	cp camera_command.php /var/www
	cp index.php /var/www
	cp poll_info.php /var/www
	cp -r css /var/www
	cp -r js /var/www

all: status_image.cgi

status_image.cgi: status_image.c
	$(CC) $< -o $@ -lhiredis

clean:
	rm -rf status_image.cgi
