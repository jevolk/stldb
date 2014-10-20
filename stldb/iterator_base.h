/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


enum Seek
{
	NEXT,
	PREV,
	FIRST,
	LAST,
	END,
};


template<class T>
class iterator_base : public std::iterator<std::bidirectional_iterator_tag,
                                           T>
{
	leveldb::DB *db;
	std::shared_ptr<const leveldb::Snapshot> snap;
	ReadOptions ropt;

  protected:
	std::unique_ptr<leveldb::Iterator> it;

  public:
	// Utils
	bool valid() const                                { return it->Valid();               }
	operator bool() const                             { return valid();                   }
	bool operator!() const                            { return !valid();                  }

	int cmp(const iterator_base &o) const;
	bool operator==(const iterator_base &o) const     { return cmp(o) == 0;               }
	bool operator!=(const iterator_base &o) const     { return cmp(o) != 0;               }
	bool operator<=(const iterator_base &o) const     { return cmp(o) <= 0;               }
	bool operator>=(const iterator_base &o) const     { return cmp(o) >= 0;               }
	bool operator<(const iterator_base &o) const      { return cmp(o) < 0;                }
	bool operator>(const iterator_base &o) const      { return cmp(o) > 0;                }

	void seek(const Seek &seek);
	void seek(const typename T::first_type &slice)    { it->Seek(slice);                  }
	void seek(const T &t)                             { it->Seek(t.first);                }

	iterator_base &operator++();
	iterator_base &operator--();
	iterator_base operator++(int);
	iterator_base operator--(int);

	iterator_base &operator+=(const size_t &n);
	iterator_base &operator-=(const size_t &n);
	iterator_base operator+(const size_t &n);
	iterator_base operator-(const size_t &n);

	iterator_base(leveldb::DB *const &db,
	              const bool &snap        = false,
	              const bool &cache       = false);

	iterator_base(const iterator_base &other);
	iterator_base &operator=(const iterator_base &other) &;
};


template<class T>
iterator_base<T>::iterator_base(leveldb::DB *const &db,
                                const bool &snap,
                                const bool &cache):
db(db),
snap({snap? db->GetSnapshot() : nullptr, [db](const leveldb::Snapshot *const &s) { if(s) db->ReleaseSnapshot(s); }}),
ropt(cache,false,this->snap.get()),
it(db->NewIterator(ropt))
{

}


template<class T>
iterator_base<T>::iterator_base(const iterator_base &o):
db(o.db),
snap(o.snap),
ropt(o.ropt),
it(db->NewIterator(ropt))
{
	if(valid())
		it->Seek(o.it->key());
	else
		seek(END);
}


template<class T>
iterator_base<T> &iterator_base<T>::operator=(const iterator_base &o)
&
{
	db = o.db;
	snap = o.snap;
	ropt = o.ropt;
	it.reset(db->NewIterator(ropt));

	if(valid())
		it->Seek(o.it->key());
	else
		seek(END);

	return *this;
}


template<class T>
iterator_base<T> iterator_base<T>::operator+(const size_t &n)
{
	auto ret(*this);
	ret += n;
	return ret;
}


template<class T>
iterator_base<T> iterator_base<T>::operator-(const size_t &n)
{
	auto ret(*this);
	ret -= n;
	return ret;
}


template<class T>
iterator_base<T> &iterator_base<T>::operator+=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(NEXT);

	return *this;
}


template<class T>
iterator_base<T> &iterator_base<T>::operator-=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(PREV);

	return *this;
}


template<class T>
iterator_base<T> iterator_base<T>::operator++(int)
{
	auto ret(*this);
	++(*this);
	return ret;
}


template<class T>
iterator_base<T> iterator_base<T>::operator--(int)
{
	auto ret(*this);
	--(*this);
	return ret;
}


template<class T>
iterator_base<T> &iterator_base<T>::operator++()
{
	this->seek(NEXT);
	return *this;
}


template<class T>
iterator_base<T> &iterator_base<T>::operator--()
{
	this->seek(PREV);
	return *this;
}


template<class T>
void iterator_base<T>::seek(const Seek &seek)
{
	switch(seek)
	{
		case NEXT:     it->Next();                      break;
		case PREV:     it->Prev();                      break;
		case FIRST:    it->SeekToFirst();               break;
		case LAST:     it->SeekToLast();                break;
		case END:      it->SeekToLast();  it->Next();   break;
	};
}


template<class T>
int iterator_base<T>::cmp(const iterator_base &o)
const
{
	return !valid() && !o.valid()?  0:                               // equally invalid
	       !o.valid()?              -1:                              // valid always less than invalid
	       !valid()?                1:                               // invalid always greater than valid
	                                it->key().compare(o.it->key());  // both valid, leveldb's turn
}
