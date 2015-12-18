/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
*    All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Helper functions for DNS resolution.
*               Loosely based on code available at:
*               http://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/ 
*
* Author: J. Armstrong
*
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include "_resolve.h"
#include "variety.h"

/* Our send/receive buffer size */
#define DNS_BUFFER_SIZE 65535

struct __dns_header
{
    uint16_t id;            /* identification number */
 
    uint8_t  recursion		: 1; 
    uint8_t  truncated		: 1;	
    uint8_t  authoritative	: 1;
    uint8_t  opcode		: 4;
    uint8_t  qr			: 1;
    uint8_t  respcode		: 4;
    uint8_t  checking_disabled	: 1;
    uint8_t  authenticated	: 1;
    uint8_t  reserved		: 1;
    uint8_t  recursion_avail	: 1;

    uint16_t n_questions;   /* number of question entries */
    uint16_t n_answers;     /* number of answer entries */
    uint16_t n_auth;        /* number of authority entries */
    uint16_t n_res;         /* number of resource entries */
    
};

struct __dns_question
{
    uint16_t qtype;
    uint16_t qclass;
};

#pragma pack(push, 1)
struct __dns_response_data
{
    uint16_t type;
    uint16_t klass;
    uint32_t ttl;
    uint16_t data_length;
};
#pragma pack(pop)

struct __dns_resource_record
{
    unsigned char *name;
    struct __dns_response_data *resource;
    unsigned char *rdata;
};


struct __dns_query
{
    unsigned char *name;
    struct __dns_question *ques;
};

static int _from_dns_name_format(unsigned char *dest, const unsigned char *reader, 
                                  const unsigned char *buffer)
{
int p, jumped, offset;
int i , j;
int count;
    
    count = 1;
    p = 0;
    jumped = 0;
    
    dest[0]='\0';
 
    /* read the names in 3www6google3com format */
    while(*reader != '\0') {
    
        if(*reader >= 192) {
            
            /* 49152 = 11000000 00000000 ;) */
            offset = (*reader)*256 + *(reader+1) - 49152; 
            
            reader = buffer + offset - 1;
            jumped = 1; /* Stop counting... */
        
        } else {
            
            dest[p++]=*reader;
            
        }
 
        reader = reader+1;
 
        /* If we havent jumped to another location then we can count up */
        if(jumped==0)
            count++; 

    }
    
    dest[p] = '\0';

    if(jumped==1)
        count++; 
    
    for(i=0; i < (int)strlen((char *)dest); i++) {
        
        p = dest[i];
        for(j=0; j < (int)p; j++) {
            dest[i] = dest[i+1];
            i++;
        }
        
        dest[i]='.';
        
    }
    
    dest[i-1]='\0'; 
    
    return count;
}

static int _to_dns_name_format(char *dest, const char *src)
{
const char *dot;
const char *segment;
int i, j, p;

    *dest = '\0';

    p = 0;
    dot = src;
    segment = src;
    do {

        dot = strchr(segment, '.');
        if(dot == NULL)
            i = strlen(segment);
        else
            i = dot-segment;
        
        dest[p++] = i;
        for(j=0;j<i;j++)
            dest[p++] = segment[j];
        
        if(dot != NULL)
            segment = dot+1;
        
    } while(dot != NULL && *segment != '\0');

    dest[p++] = '\0';
    return p;
}

static char **_add_string_to_list(char **addr_list, char *text)
{
int i;
char **lptr;
    
    /* Find the last entry */
    lptr = addr_list;
    i = 0;
    while(lptr[i] != NULL) i++;
    
    addr_list = (char **)realloc(addr_list, (i+2)*sizeof(char *));
    if(addr_list != NULL) {
        addr_list[i+1] = NULL;
        addr_list[i] = text;
    }
    
    return addr_list;
}

static char **_add_address_to_list(char **addr_list, struct in_addr addr)
{
char *allocated_address;
    
    allocated_address = (char *)malloc(64*sizeof(char));
    if(allocated_address != NULL)
        strcpy(allocated_address, inet_ntoa(addr));
    
    return _add_string_to_list(addr_list, allocated_address);
}

_WCRTLINK int _dns_query(const char *name, int query_type, in_addr_t dnsaddr, struct hostent *res)
{
unsigned char *buf, *query_name, *reader;
int query_socket;
int i, j;
int name_length;
int result;
size_t query_size;
 
struct __dns_resource_record *answers; //the replies from the DNS server
struct sockaddr_in dest;
 
struct __dns_header *dns;
struct __dns_question *qinfo;
    
int ret;
    
struct in_addr *lptr;
 
    ret = 0;
 
    if(res == NULL) return -EINVAL;
    if(name == NULL) return -EINVAL;
 
    buf = (unsigned char *)malloc(DNS_BUFFER_SIZE*sizeof(unsigned char));
    if(buf == NULL) return -ENOMEM;
    
    answers = NULL;

    query_socket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); //UDP packet for DNS queries
 
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = dnsaddr; //dns servers
 
    //Set the DNS structure to standard queries
    dns = (struct __dns_header *)buf;
    memset(dns, 0, sizeof(struct __dns_header));
 
    dns->id = (uint16_t)htons(getpid());
    dns->recursion = 1;      /* request recursion */
    dns->n_questions = htons(1);
    query_size = sizeof(struct __dns_header);

    query_name = (unsigned char*)&buf[(int)query_size];
    query_size += _to_dns_name_format((char *)query_name, (const char *)name);
    
    qinfo = (struct __dns_question *)&buf[query_size];
 
    /* type of the query, A, MX, CNAME, NS etc */
    qinfo->qclass = htons(1); 
    qinfo->qtype = htons(query_type); 
    qinfo->qclass = htons(1); 
    query_size += sizeof(struct __dns_question);
    
    result = sendto(query_socket,
                    (char *)buf,
                    query_size,
                    0,
                    (struct sockaddr *)&dest,
                    sizeof(dest));
    
    if(result < 0) {
        ret = -ENOMSG;
        goto dns_cleanup;
    }
    
    i = sizeof(dest);
    result = recvfrom(query_socket,
                      (char *)buf,
                      DNS_BUFFER_SIZE,
                      0,
                      (struct sockaddr *)&dest,
                      (socklen_t *)&i);
    if(result < 0) {
        ret = -ENOMSG;
        goto dns_cleanup;
    }
    
    dns = (struct __dns_header *)buf;
    reader = &buf[query_size];
    
    if(ntohs(dns->n_answers) == 0) {
        ret = -ENOENT;
        goto dns_cleanup;
    }
    
    answers = (struct __dns_resource_record *)malloc(ntohs(dns->n_answers)*sizeof(struct __dns_resource_record));
    if(answers == NULL)
        goto dns_cleanup;
    memset(answers, 0, ntohs(dns->n_answers)*sizeof(struct __dns_resource_record));
    
    /* If we've reached this point, we have answers (or none, but
     * a valid lack of answers), so begin filling in the result
     * structure appropriately
     */
    if(query_type == DNSQ_TYPE_A) {
        res->h_name = (char *)malloc((strlen(name)+1)*sizeof(char));
        if(res->h_name != NULL)
            strcpy(res->h_name, name);
    }
    
    res->h_addr_list = (char **)malloc(sizeof(char *));
    if(res->h_addr_list != NULL)
        res->h_addr_list[0] = NULL;
    
    res->h_aliases = (char **)malloc(sizeof(char *));
    if(res->h_aliases != NULL)
        res->h_aliases[0] = NULL;
    
    /* We only support IPv4 right now. */
    res->h_addrtype = AF_INET;
    
    for(i=0;i<ntohs(dns->n_answers);i++) {
        answers[i].name = (unsigned char *)malloc(256*sizeof(unsigned char));
        name_length = _from_dns_name_format(answers[i].name, reader, buf);
        reader += name_length;
 
        answers[i].resource = (struct __dns_response_data *)(reader);
        reader = reader + sizeof(struct __dns_response_data);
 
        if(ntohs(answers[i].resource->type) == DNSQ_TYPE_A) { /* IPv4 encountered */
            
            answers[i].rdata = (unsigned char*)malloc((ntohs(answers[i].resource->data_length)+1)*sizeof(char));
            if(answers[i].rdata != NULL) {
                for(j=0; j < ntohs(answers[i].resource->data_length); j++)
                    answers[i].rdata[j]=reader[j];
            }
 
            answers[i].rdata[ntohs(answers[i].resource->data_length)] = '\0';

            reader = reader + ntohs(answers[i].resource->data_length);

            lptr = (struct in_addr *)answers[i].rdata;
            
            res->h_addr_list = _add_address_to_list(res->h_addr_list, *lptr);
            ret = 1;
            
        } else {
        
            /* Answer should just be a string */
            answers[i].rdata = (unsigned char *)malloc(256*sizeof(unsigned char));
            if(answers[i].rdata != NULL) {
                name_length = _from_dns_name_format(answers[i].name, reader, buf);
                
                if(query_type == ntohs(answers[i].resource->type)) {
                    
                    if(res->h_name == NULL) {
                        
                        res->h_name = answers[i].name;
                        answers[i].name = NULL;
                        
                    } else {
                        
                        res->h_aliases = _add_string_to_list(res->h_aliases, answers[i].name);
                        answers[i].name = NULL;
                        
                    }
                    
                }
                
                reader += name_length;
            }
        }
    }
    
dns_cleanup:

    if(answers != NULL && dns != NULL) {
        for(i=0;i<ntohs(dns->n_answers);i++) {
            if(answers[i].name  != NULL) free(answers[i].name);
            if(answers[i].rdata != NULL) free(answers[i].rdata);
        }
        free(answers);
    }
    
    free(buf);
    
    return ret;
}
