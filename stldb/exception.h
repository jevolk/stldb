/**
 *	PUBLIC DOMAIN 2014 (C) Jason Volk
 */


struct Exception : leveldb::Status,
                   std::runtime_error
{
	operator std::string() const  { return what(); }

	Exception(const std::string &what = {}):
	          std::runtime_error(what) {}

	Exception(const leveldb::Status &status):
	          leveldb::Status(status), std::runtime_error(this->ToString()) {}

	friend std::ostream &operator<<(std::ostream &s, const Exception &e)
	{
		return (s << e.what());
	}
};


#define STLDB_EXCEPTION(Name) \
struct Name : Exception \
{ \
	template<class... A> Name(A&&... a): Exception(std::forward<A>(a)...) {} \
};


STLDB_EXCEPTION(IOError)
STLDB_EXCEPTION(NotFound)
STLDB_EXCEPTION(Corruption)
STLDB_EXCEPTION(NotSupported)
STLDB_EXCEPTION(InvalidArgument)


inline
void throw_on_error(const leveldb::Status &status)
{
	// leveldb::Status hides a switchable code :-/

	if(status.ok())
		return;
	else if(status.IsNotFound())
		throw NotFound(status);
	else if(status.IsCorruption())
		throw Corruption(status);
	else if(status.IsIOError())
		throw IOError(status);
	else
		throw Exception(status);
}
