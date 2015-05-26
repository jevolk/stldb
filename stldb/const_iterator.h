/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class const_iterator : public base::iterator,
                       public std::iterator<std::bidirectional_iterator_tag, T>
{
	mutable T t;

  public:
	const T &operator*() const;
	const T *operator->() const;
	T &operator*();
	T *operator->();

	const_iterator operator+(const size_t &n) const;
	const_iterator operator-(const size_t &n) const;

	using base::iterator::operator++;
	using base::iterator::operator--;
	const_iterator operator++(int);
	const_iterator operator--(int);

	template<class... Args> const_iterator(const leveldb::DB *const &db, Args&&... args);
};


template<class T>
template<class... Args>
const_iterator<T>::const_iterator(const leveldb::DB *const &db,
                                  Args&&... args):
base::iterator(const_cast<leveldb::DB *>(db),std::forward<Args>(args)...)
{
}


template<class T>
const_iterator<T> const_iterator<T>::operator--(int)
{
	const auto ret(*this);
	--(*this);
	return ret;
}


template<class T>
const_iterator<T> const_iterator<T>::operator++(int)
{
	const auto ret(*this);
	++(*this);
	return ret;
}


template<class T>
const_iterator<T> const_iterator<T>::operator-(const size_t &n)
const
{
	auto ret(*this);
	ret -= n;
	return ret;
}


template<class T>
const_iterator<T> const_iterator<T>::operator+(const size_t &n)
const
{
	auto ret(*this);
	ret += n;
	return ret;
}


template<class T>
T *const_iterator<T>::operator->()
{
	operator*();
	return &t;
}


template<class T>
T &const_iterator<T>::operator*()
{
	t =
	{
		{ nullptr, it->key()   },
		{ nullptr, it->value() },
	};

	return t;
}


template<class T>
const T *const_iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
const T &const_iterator<T>::operator*()
const
{
	t =
	{
		{ nullptr, it->key()   },
		{ nullptr, it->value() },
	};

	return t;
}
