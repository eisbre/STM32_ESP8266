#ifndef _ONENET_H_
#define _ONENET_H_

_Bool EMQX_DevLink(void);

void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt);

void EMQX_Publish(const char *topic, const char *msg);

void EMQX_RevPro(unsigned char *cmd);

#endif
