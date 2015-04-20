#include <stdio.h>
#include "hiredis/hiredis.h"


int main()
{
  redisContext *c;
  redisReply * reply; 

  c = redisConnect("127.0.0.1", 6379);
  if (c->err) {
    printf("Error Connecting to Redis Server: %s\n", c->errstr);
    redisFree(c);
    return 1;
  }
  
  reply = redisCommand(c, "GET vision:image");
  if (reply->type == REDIS_REPLY_STRING) {
    puts("Content-Type: image/jpeg");
    puts("Expires: 0");
    printf("Content-Length: %i\n\n", reply->len);

    fwrite(reply->str, reply->len, 1, stdout);
  }

  redisFree(c);

  return 0;
}
