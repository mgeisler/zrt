/*
 *
 * Copyright (c) 2012-2013, LiteStack, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <string.h> //memset
#include <stdlib.h> //calloc, qsort
#include <stdio.h>
#include <stddef.h>
#include <assert.h>

#include "zrtlog.h"
#include "helpers/dyn_array.h"
#include "channels_conf.h"

void DebugPrint( struct UserChannel *channel, FILE* out ){
    fprintf( out, "channel nodetype=%d, nodeid=%d, fd=%d, mode=%s\n",
	     channel->nodetype, channel->nodeid, channel->fd,
	     channel->mode == EChannelModeRead ? "EChannelModeRead" : "EChannelModeWrite" );
}

static int
cmp_by_node_type_and_id(const void *p1, const void *p2)
{
    struct UserChannel** c1 = (struct UserChannel**)p1;
    struct UserChannel** c2 = (struct UserChannel**)p2;

    if      ( (*c1)->nodetype < (*c2)->nodetype ) return -1;
    else if ( (*c1)->nodetype > (*c2)->nodetype ) return  1;
    else{
        if      ( (*c1)->nodeid < (*c2)->nodeid ) return -1;
        else if ( (*c1)->nodeid > (*c2)->nodeid ) return  1;
        else return 0;
    }
    return 0;
}


void Free(struct ChannelsConfigInterface *ch_if){
    struct UserChannel *channel = NULL;
    for( int i=0; i < ch_if->channels->num_entries; i++ ){
        channel = DynArrayGet(ch_if->channels, i);
        assert(channel);
        free(channel);
    }
    DynArrayDtor( ch_if->channels );
}

struct UserChannel *AddChannel(struct ChannelsConfigInterface *ch_if,
			       int nodetype, int nodeid, int channelfd, ChannelMode mode )
{
    struct UserChannel *channel = NULL;
    int res=0;
    if ( !ch_if->channels ){
        ch_if->channels = calloc( 1, sizeof(struct DynArray) );
        res = DynArrayCtor(ch_if->channels, 10 /*granularity*/ );
        assert( res != 0 );
    }

    channel = calloc(1, sizeof(struct UserChannel));
    channel->nodetype = nodetype;
    channel->nodeid = nodeid;
    channel->fd = channelfd;
    channel->mode = mode;
    /*all channel heap data should be deleted */
    res = DynArraySet( ch_if->channels, ch_if->channels->num_entries, channel );
    assert( res != 0 );

    return channel;
}

int GetNodesListByType( const struct ChannelsConfigInterface *ch_if, 
			int nodetype, 
			int **nodes_array ){
    int count_rchan = 0;
    int count_wchan = 0;
    int i=0;

    /*calculate nodes count for two groups (read,write) of nodes and select maximum */
    for ( i=0; i < ch_if->channels->num_entries; i++  ){
        struct UserChannel *channel = DynArrayGet(ch_if->channels, i );
        if ( channel->nodetype == nodetype && channel->mode == EChannelModeRead )
            count_rchan++;
        else if ( channel->nodetype == nodetype && channel->mode == EChannelModeWrite )
            count_wchan++;
    }

    *nodes_array = calloc( count_rchan > count_wchan ? count_rchan : count_wchan, sizeof(int) );
    int mode=0;
    if ( count_rchan > count_wchan ){
        /*alloc array of node ids*/
        mode=EChannelModeRead;
    }
    else{
        mode=EChannelModeWrite;
    }

    /*sort to get ascending nodeid array*/
    qsort( ch_if->channels->ptr_array, 
	   ch_if->channels->num_entries,sizeof(struct UserChannel*), 
	   cmp_by_node_type_and_id );

    count_rchan = 0;
    for ( i=0; i < ch_if->channels->num_entries; i++  ){
        struct UserChannel *channel = DynArrayGet(ch_if->channels, i );
        if ( channel->nodetype == nodetype && channel->mode == mode ){
            (*nodes_array)[count_rchan++] = channel->nodeid;
        }
    }
    return count_rchan;
}


struct UserChannel *Channel(struct ChannelsConfigInterface *ch_if, int nodetype, int nodeid, int8_t channelmode){
    struct UserChannel *ch = NULL;
    for ( int i=0; i < ch_if->channels->num_entries; i++ ){
        ch = DynArrayGet(ch_if->channels, i);
        if( ch && ch->nodetype == nodetype && ch->nodeid == nodeid && ch->mode == channelmode ){
            ch->DebugPrint = DebugPrint;
            return ch;
        }
    }
    return NULL;
}


void SetupChannelsConfigInterface( struct ChannelsConfigInterface *ch_if, int ownnodeid, int ownnodetype  ){
    memset( ch_if, '\0', sizeof(*ch_if) );
    ch_if->AddChannel = AddChannel;
    ch_if->Channel = Channel;
    ch_if->GetNodesListByType = GetNodesListByType;
    ch_if->Free = Free;
    ch_if->ownnodeid = ownnodeid;
    ch_if->ownnodetype = ownnodetype;
}

