#include "largemalloc.h"
#include "cJSON.h"
#include <pthread.h>
#include <map>

using namespace std;

typedef struct _pLargeMemBlock
{
		int		nMemSize;
		int		nUseSize;
		char*	paddr;
		int		flag;
		struct _pLargeMemBlock*		pre;
		struct _pLargeMemBlock*		next;
}pLargeMemBlock;


pthread_mutex_t			mem_lock;
map<unsigned long, void*>		mem_map;
static	unsigned long	 init_largeblocksize;

static pLargeMemBlock*	get_block( unsigned long tid );
static void set_block( unsigned long tid, pLargeMemBlock* pBlock );
static void free_block( unsigned long tid );

pLargeMemBlock*
get_block( unsigned long tid )
{
	map<unsigned long, void*>::iterator	it;
	pLargeMemBlock*					pBlock;
	
	pthread_mutex_lock( &mem_lock );
	it = mem_map.find( tid );
	if ( it != mem_map.end() )
		pBlock = (pLargeMemBlock*)(*it).second;
	else
		pBlock = NULL;
	pthread_mutex_unlock( &mem_lock );
	
	return pBlock;
}

void
set_block( unsigned long tid, pLargeMemBlock* pBlock )
{
	map<unsigned long, void*>::iterator		it;
	pthread_mutex_lock( &mem_lock );
	it = mem_map.find( tid );
	if ( it == mem_map.end() )
	{
		mem_map.insert( make_pair( tid, pBlock ) );
	}
	pthread_mutex_unlock( &mem_lock );
}

void
free_block( unsigned long tid )
{
	map<unsigned long, void*>::iterator		it;
	pthread_mutex_lock( &mem_lock );
	it = mem_map.find( tid );
	if ( it != mem_map.end() )
		mem_map.erase( it );
	pthread_mutex_unlock( &mem_lock );
}

static pLargeMemBlock*
_malloc_mem_block( int size )
{
	char*				p			= new char[sizeof(pLargeMemBlock) + size];
	pLargeMemBlock*		pnewBlock	= (pLargeMemBlock*)p;

	if ( NULL == p ) return NULL;

	pnewBlock->nMemSize		= size;
	pnewBlock->paddr		= (char*)(pnewBlock + 1);
	pnewBlock->nUseSize		= 0;
	pnewBlock->flag			= 0;
	pnewBlock->pre			= NULL;
	pnewBlock->next			= NULL;

	return pnewBlock;
}

void*
largeblock_alloc(size_t size)
{
	unsigned long		tid = pthread_self();
	pLargeMemBlock*		pBlock;
	void*				outp;

	pBlock = get_block( tid );
	if ( NULL == pBlock )
	{
		pBlock = _malloc_mem_block( init_largeblocksize );
		set_block( tid, pBlock );
	}

	while ( pBlock->flag )
	{
		pBlock = pBlock->next;
	}

	while ( (size_t)(pBlock->nMemSize - pBlock->nUseSize) < size )
	{
		if ( pBlock->next )
		{
			pBlock->flag	= 1;
			pBlock			= pBlock->next;
			continue;
		}

		int maxsize =  ((size/init_largeblocksize)+1 )*init_largeblocksize;
		pLargeMemBlock*		pnewBlock = _malloc_mem_block( maxsize );

		if ( NULL == pnewBlock)
		{
			return NULL;
		}

		pnewBlock->flag		= 0;
		pnewBlock->pre		= pBlock;
		pnewBlock->next		= NULL;
		pBlock->next		= pnewBlock;
		pBlock->flag		= 1;
		pBlock				= pnewBlock;
		break;
	}

	outp = (pBlock->paddr + pBlock->nUseSize);
	pBlock->nUseSize += size;

	return outp;
}

void
cJSON_Reset()
{
    unsigned long		tid = pthread_self();
	pLargeMemBlock*		pBlock;

	pBlock = get_block( tid );

	while( pBlock )
	{
		pBlock->flag		= 0;
		pBlock->nUseSize	= 0;
		pBlock				= pBlock->next;
	}
}

void
cJSON_Free( )
{
	unsigned long		tid = pthread_self();
	pLargeMemBlock		*pBlock, *pNext;

	pBlock = get_block( tid );

	while ( pBlock )
	{
		pNext	= pBlock->next;
		char*p = (char*)pBlock;
		delete []p;
		pBlock = NULL;
		pBlock	= pNext;
	}

	free_block( tid );
}

void
largeblock_pfree( void *ptr )
{
    return;
}

void
cJSON_Init( unsigned long blocksize )
{
	pthread_mutex_init( &mem_lock , NULL);
	init_largeblocksize = blocksize;
	
	cJSON_Hooks		hooks;    
	hooks.malloc_fn = largeblock_alloc;
	hooks.free_fn	= largeblock_pfree;
	cJSON_InitHooks(&hooks);
}

void
cJSON_Release()
{
	cJSON_Hooks		hooks;    
	hooks.malloc_fn = NULL;
	hooks.free_fn	= NULL;
	cJSON_InitHooks(&hooks);
	pthread_mutex_destroy( &mem_lock );
}

