/*
 * enum_strings.c
 * string constants, for debugging purposes
 *
 *  Created on: 22.10.2012
 *      Author: YaroslavLitvinov
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "enum_strings.h"
#include "zrt_helper_macros.h"
#include "zrtlog.h"

/****************************************************************
 *flags count should be equal to flag texts count for every pair
 *of flags and texts 
 ****************************************************************/

/*these pair of arrays intended to use as file flags logging*/
static const int 
s_file_open_flags[] = {O_CREAT, O_EXCL, O_TRUNC, O_DIRECT, O_DIRECTORY,
		       O_NOATIME, O_APPEND, O_ASYNC, O_SYNC, O_NONBLOCK, 
		       O_NDELAY, O_NOCTTY };
static const char*
s_file_open_texts[] = {"O_CREAT", "O_EXCL", "O_TRUNC", "O_DIRECT", "O_DIRECTORY",
		       "O_NOATIME", "O_APPEND", "O_ASYNC", "O_SYNC", "O_NONBLOCK", 
		       "O_NDELAY", "O_NOCTTY" };

/*these pair of arrays intended to use as mmap prot flags logging*/
static const int
s_mmap_prot_flags[] = {PROT_EXEC, PROT_READ, PROT_WRITE, PROT_NONE};
static const char*
s_mmap_prot_texts[] = {"PROT_EXEC", "PROT_READ", "PROT_WRITE", "PROT_NONE"};


/*these pair of arrays intended to use as mmap flags logging*/
static const int
s_mmap_flags[] = {MAP_SHARED, MAP_PRIVATE, MAP_32BIT, MAP_ANON, MAP_ANONYMOUS, 
		       MAP_DENYWRITE, MAP_EXECUTABLE, MAP_FILE, MAP_FIXED, 
		       MAP_GROWSDOWN, MAP_LOCKED, MAP_NONBLOCK, 
		       MAP_NORESERVE, MAP_POPULATE, MAP_STACK};
static const char*
s_mmap_flags_texts[] = {"MAP_SHARED", "MAP_PRIVATE", "MAP_32BIT", "MAP_ANON", "MAP_ANONYMOUS", 
		       "MAP_DENYWRITE", "MAP_EXECUTABLE", "MAP_FILE", "MAP_FIXED", 
		       "MAP_GROWSDOWN", "MAP_LOCKED", "MAP_NONBLOCK", 
		       "MAP_NORESERVE", "MAP_POPULATE", "MAP_STACK"};

/*these pair of arrays intended to use as seek flags logging*/
static const int
s_seek_whence_flags[] = {SEEK_SET, SEEK_CUR, SEEK_END };
static const char*
s_seek_whence_texts[] = {"SEEK_SET", "SEEK_CUR", "SEEK_END"};




static void log_flags(char* name, int flags, int array_len, 
	       const int flags_array[], const char* flags_texts[] )
{
    int output_space=0;
    const int space_len = 1000;
    char s[space_len];
    /*empty_flag_index is used to save empty flag and after flags processing not found
     * setted flags then output flag at index; */
    int empty_flag_index =-1; 
    int i;
    char delim_char = '|';

    for ( i=0; i < array_len; i++ ){
	/*save empty flag index*/
	if ( flags_array[i] == 0 ) empty_flag_index = i; 
	/*check every flag and if flag is set add it to s buffer for further logging*/
        if ( CHECK_FLAG(flags, flags_array[i]) && flags_array[i] != 0 ){
	    if ( output_space > 0 ) delim_char = '|';
	    else delim_char = ' ';
	    output_space += snprintf( s+output_space, space_len-output_space, 
				      "%c%s", delim_char, flags_texts[i] );
	    if ( output_space > space_len ){
		zrt_log_str("Flags output truncated, insufficient buffer size.");
		break;
	    }
        }
    }
    /*display empty flag if not found setted flags*/
    if ( output_space == 0 ){
	if ( empty_flag_index != -1 ){
	    strcpy(s, flags_texts[empty_flag_index]);
	}
	else{
	    strcpy(s, "none");
	}
    }
    zrt_log("%s 0x%X [%s]", name, flags, s );
}

void log_file_open_flags( int flags ){
    /*array size of flags and their textual representation shoul be equal*/
    assert( sizeof(s_file_open_flags)/sizeof(*s_file_open_flags) == 
	    sizeof(s_file_open_texts)/sizeof(*s_file_open_texts) );

    /* pass array length as dedicated paramenter for statically constructed array
    *  because array passed as parameter can't determine own size correctly.*/
    log_flags( "file open flags", flags, sizeof(s_file_open_flags)/sizeof(*s_file_open_flags),
	       s_file_open_flags, s_file_open_texts );
}

void log_mmap_prot( int prot ){
    /*array size of flags and their textual representation shoul be equal*/
    assert( sizeof(s_mmap_prot_flags)/sizeof(*s_mmap_prot_flags) == 
	    sizeof(s_mmap_prot_texts)/sizeof(*s_mmap_prot_texts) );

    /* pass array length as dedicated paramenter for statically constructed array
    *  because array passed as parameter can't determine own size correctly.*/
    log_flags( "mmap prot flags", prot, sizeof(s_mmap_prot_flags)/sizeof(*s_mmap_prot_flags),
	       s_mmap_prot_flags, s_mmap_prot_texts );
}


void log_mmap_flags( int flags ){
    /*array size of flags and their textual representation shoul be equal*/
    assert( sizeof(s_mmap_flags)/sizeof(*s_mmap_flags) == 
	    sizeof(s_mmap_flags_texts)/sizeof(*s_mmap_flags_texts) );

    /* pass array length as dedicated paramenter for statically constructed array
    *  because array passed as parameter can't determine own size correctly.*/
    log_flags( "mmap flags", flags, sizeof(s_mmap_flags)/sizeof(*s_mmap_flags),
	       s_mmap_flags, s_mmap_flags_texts );
}


/*log to zrt debug seek whence */
void log_seek_whence( int whence ){
    /*array size of flags and their textual representation shoul be equal*/
    assert( sizeof(s_seek_whence_flags)/sizeof(*s_seek_whence_flags) == 
	    sizeof(s_seek_whence_texts)/sizeof(*s_seek_whence_texts) );

    /* pass array length as dedicated paramenter for statically constructed array
    *  because array passed as parameter can't determine own size correctly.*/
    log_flags( "whence flags", whence, 
	       sizeof(s_seek_whence_flags)/sizeof(*s_seek_whence_flags),
	       s_seek_whence_flags, s_seek_whence_texts );
}
