/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


enum Seek
{
	NEXT,       // Next() - next element
	PREV,       // Prev() - previous element
	FIRST,      // SeekToFirst() - first element
	LAST,       // SeekToLast() - last element
	END,        // SeekToLast() + Next() - past the end (invalid)
};

class iterator_base
{
	template<class T> friend class Slice;             // Accesses it/db for write-backs to iterator

  protected:
	leveldb::DB *db;
	std::shared_ptr<const leveldb::Snapshot> snap;
	std::unique_ptr<leveldb::Iterator> it;
	Flag flags;

	bool valid() const                                { return it->Valid();               }
	int cmp(const iterator_base &o) const;

	template<class T> void seek(const T &t);
	void seek(const Seek &seek);
	void flush();

  public:
	operator bool() const                             { return valid();                   }
	bool operator!() const                            { return !valid();                  }
	bool operator==(const iterator_base &o) const     { return cmp(o) == 0;               }
	bool operator!=(const iterator_base &o) const     { return cmp(o) != 0;               }
	bool operator<=(const iterator_base &o) const     { return cmp(o) <= 0;               }
	bool operator>=(const iterator_base &o) const     { return cmp(o) >= 0;               }
	bool operator<(const iterator_base &o) const      { return cmp(o) < 0;                }
	bool operator>(const iterator_base &o) const      { return cmp(o) > 0;                }

	iterator_base &operator++();
	iterator_base &operator--();
	iterator_base operator++(int);
	iterator_base operator--(int);

	iterator_base &operator+=(const size_t &n);
	iterator_base &operator-=(const size_t &n);
	iterator_base operator+(const size_t &n);
	iterator_base operator-(const size_t &n);

	template<class Seek> iterator_base(leveldb::DB *const &db, const Seek &seek, const Flag &flags = Flag(0));
	iterator_base(const iterator_base &other);
	iterator_base &operator=(const iterator_base &other) &;
	virtual ~iterator_base() = default;
};


template<class Seek>
iterator_base::iterator_base(leveldb::DB *const &db,
                             const Seek &seek,
                             const Flag &flags):
db(db),
snap
({
	flags & SNAPSHOT? db->GetSnapshot() : nullptr, [db]
	(const leveldb::Snapshot *const &s)
	{
		if(s)
			db->ReleaseSnapshot(s);
	}
}),
it(db->NewIterator(ReadOptions(flags,this->snap.get()))),
flags(flags)
{
	this->seek(seek);
}


inline
iterator_base::iterator_base(const iterator_base &o):
db(o.db),
snap(o.snap),
it(db->NewIterator(ReadOptions(o.flags,this->snap.get()))),
flags(o.flags)
{
	if(o.valid())
		it->Seek(o.it->key());
	else
		seek(END);
}


inline
iterator_base &iterator_base::operator=(const iterator_base &o)
&
{
	db = o.db;
	snap = o.snap;
	flags = o.flags;
	it.reset(db->NewIterator(ReadOptions(o.flags,this->snap.get())));

	if(o.valid())
		it->Seek(o.it->key());
	else
		seek(END);

	return *this;
}


inline
iterator_base iterator_base::operator+(const size_t &n)
{
	auto ret(*this);
	ret += n;
	return ret;
}


inline
iterator_base iterator_base::operator-(const size_t &n)
{
	auto ret(*this);
	ret -= n;
	return ret;
}


inline
iterator_base &iterator_base::operator+=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(NEXT);

	return *this;
}


inline
iterator_base &iterator_base::operator-=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(PREV);

	return *this;
}


inline
iterator_base iterator_base::operator++(int)
{
	auto ret(*this);
	++(*this);
	return ret;
}


inline
iterator_base iterator_base::operator--(int)
{
	auto ret(*this);
	--(*this);
	return ret;
}


inline
iterator_base &iterator_base::operator++()
{
	this->seek(NEXT);
	return *this;
}


inline
iterator_base &iterator_base::operator--()
{
	this->seek(PREV);
	return *this;
}


inline
void iterator_base::flush()
{
	const auto old = std::move(it);
	it.reset(db->NewIterator(ReadOptions(flags,snap.get())));
	seek(old->key());
}


template<class T>
void iterator_base::seek(const T &t)
{
	it->Seek(t);

	if(!valid())
		return;

	if(it->key() != t)
	{
		if(~flags & (UPPER|LOWER))
			seek(END);
	}
	else if(flags & UPPER)
		seek(NEXT);
}


inline
void iterator_base::seek(const Seek &s)
{
	switch(s)
	{
		case NEXT:     it->Next();         break;
		case PREV:     it->Prev();         break;
		case FIRST:    it->SeekToFirst();  break;
		case LAST:     it->SeekToLast();   break;
		case END:
		default:
			seek(LAST);
			if(valid())
				seek(NEXT);
	};
}


inline
int iterator_base::cmp(const iterator_base &o)
const
{
	return !valid() && !o.valid()?  0:                               // equally invalid
	       !o.valid()?              -1:                              // valid always less than invalid
	       !valid()?                1:                               // invalid always greater than valid
	                                it->key().compare(o.it->key());  // both valid, leveldb's turn
}
