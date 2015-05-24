/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


#ifndef STLDB_INCLUDED
	#define STLDB_INCLUDED


// std
#include <type_traits>

// boost
#include <boost/lexical_cast.hpp>

// leveldb
#include <leveldb/filter_policy.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <leveldb/db.h>

// stldb
namespace stldb
{
#include "exception.h"
#include "less.h"
#include "options.h"
#include "iterator_base.h"
#include "slice.h"
#include "const_iterator.h"
#include "iterator.h"
#include "ldb.h"
}


#endif  // STLDB_INCLUDED
