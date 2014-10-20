/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class const_iterator : public iterator_base<T>
{
	mutable T t;

  public:
	const_iterator &operator++();
	const_iterator &operator--();
	const_iterator operator++(int);
	const_iterator operator--(int);

	const_iterator &operator+=(const size_t &n);
	const_iterator &operator-=(const size_t &n);
	const_iterator operator+(const size_t &n);
	const_iterator operator-(const size_t &n);

	T &operator*() const;
	T *operator->() const;

	const_iterator(const leveldb::DB *const &db,
	               const bool &snap = false,
	               const bool &cache = false);
};


template<class T> using iterator = const_iterator<T>;


template<class T>
const_iterator<T>::const_iterator(const leveldb::DB *const &db,
                                  const bool &snap,
                                  const bool &cache):
iterator_base<T>(const_cast<leveldb::DB *>(db),snap,cache)
{

}


template<class T>
T *const_iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
T &const_iterator<T>::operator*()
const
{
	this->t = {this->it->key(),this->it->value()};
	return t;
}


template<class T>
const_iterator<T> const_iterator<T>::operator+(const size_t &n)
{
	auto ret(*this);
	ret += n;
	return ret;
}


template<class T>
const_iterator<T> const_iterator<T>::operator-(const size_t &n)
{
	auto ret(*this);
	ret -= n;
	return ret;
}


template<class T>
const_iterator<T> &const_iterator<T>::operator+=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(NEXT);

	return *this;
}


template<class T>
const_iterator<T> &const_iterator<T>::operator-=(const size_t &n)
{
	for(size_t i = 0; i < n; i++)
		this->seek(PREV);

	return *this;
}


template<class T>
const_iterator<T> const_iterator<T>::operator++(int)
{
	auto ret(*this);
	++(*this);
	return ret;
}


template<class T>
const_iterator<T> const_iterator<T>::operator--(int)
{
	auto ret(*this);
	--(*this);
	return ret;
}


template<class T>
const_iterator<T> &const_iterator<T>::operator++()
{
	this->seek(NEXT);
	return *this;
}


template<class T>
const_iterator<T> &const_iterator<T>::operator--()
{
	this->seek(PREV);
	return *this;
}
