#include <string.h>
#include "repsheet.h"
#include "common.h"
#include "cidr.h"
#include "check_cidr.h"


/**
 * @file whitelist.c
 * @author Aaron Bedra
 * @date 12/09/2014
 */


int initial_whitelist_size = 1000;

void set_initial_whitelist_size(int new_size) 
{
  initial_whitelist_size = new_size;
}


/**
 * Adds the actor to the Repsheet whitelist
 *
 * @param context the Redis connection
 * @param actor the actors user data or ip address
 * @param type IP or USER
 * @reason string to describe the reason for whitelisting
 *
 * @returns an integer result
 */
int whitelist(redisContext *context, const char *actor, int type, const char *reason)
{
  redisReply *reply;

  switch(type) {
  case IP:
    return set_list(context, actor, "ip", "whitelisted", reason);
    break;
  case USER:
    return set_list(context, actor, "user", "whitelisted", reason);
    break;
  case BLOCK:
    return set_block(context, actor, "whitelisted", reason);
    break;
  default:
    return UNSUPPORTED;
    break;
  }
}


/**
 * Checks to see if an ip is on the Repsheet whitelist
 *
 * @param context the Redis connection
 * @param actor the IP address of the actor
 * @param reason returns the reason for whitelisting.
 *
 * @returns TRUE if yes, FALSE if no, DISCONNECTED if error
 */
int is_ip_whitelisted(redisContext *context, const char *actor, char *reason)
{
  static expanding_vector *cidr_cache = NULL;
  static time_t cache_update_time = 0;

  redisReply *ip = redisCommand(context, "GET %s:repsheet:ip:whitelisted", actor);
  if (ip) {
    if (ip->type == REDIS_REPLY_STRING) {
      populate_reason(ip, reason);
      freeReplyObject(ip);
      return TRUE;
    } else {
      freeReplyObject(ip);
    }
  } else {
    return DISCONNECTED;
  }

  if (cidr_cache == NULL) {
    cidr_cache = create_expanding_vector(10000);
  }
  return checkCIDR(context, actor, reason, "whitelisted", cidr_cache, &cache_update_time);
}

/**
 * Checks to see if a user is on the Repsheet whitelist
 *
 * @param context the Redis connection
 * @param actor the user
 * @param reason returns the reason for whitelisting.
 *
 * @returns TRUE if yes, FALSE if no, DISCONNECTED if error
 */
int is_user_whitelisted(redisContext *context, const char *actor, char *reason)
{
  redisReply *reply;

  reply = redisCommand(context, "GET %s:repsheet:user:whitelisted", actor);
  if (reply) {
    if (reply->type == REDIS_REPLY_STRING) {
      populate_reason(reply, reason);
      freeReplyObject(reply);
      return TRUE;
    } else {
      freeReplyObject(reply);
      return FALSE;
    }
  } else {
    return DISCONNECTED;
  }
}
