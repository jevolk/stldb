/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class iterator : public base::iterator,
                 public std::iterator<std::bidirectional_iterator_tag, T>
{
	mutable T t;

  public:
	const T &operator*() const;
	const T *operator->() const;
	T &operator*();
	T *operator->();

	iterator operator+(const size_t &n) const;
	iterator operator-(const size_t &n) const;

	using base::iterator::operator++;
	using base::iterator::operator--;
	iterator operator++(int);
	iterator operator--(int);

	template<class... Args> iterator(Args&&... args);
};


template<class T>
template<class... Args>
iterator<T>::iterator(Args&&... args):
base::iterator(std::forward<Args>(args)...)
{
}


template<class T>
iterator<T> iterator<T>::operator--(int)
{
	const auto ret(*this);
	--(*this);
	return ret;
}


template<class T>
iterator<T> iterator<T>::operator++(int)
{
	const auto ret(*this);
	++(*this);
	return ret;
}


template<class T>
iterator<T> iterator<T>::operator-(const size_t &n)
const
{
	auto ret(*this);
	ret -= n;
	return ret;
}


template<class T>
iterator<T> iterator<T>::operator+(const size_t &n)
const
{
	auto ret(*this);
	ret += n;
	return ret;
}


template<class T>
T *iterator<T>::operator->()
{
	operator*();
	return &t;
}


template<class T>
T &iterator<T>::operator*()
{
	t =
	{
		{ static_cast<base::iterator *>(this), it->key()    },
		{ static_cast<base::iterator *>(this), it->value()  },
	};

	return t;
}


template<class T>
const T *iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
const T &iterator<T>::operator*()
const
{
	const auto &base(static_cast<const base::iterator *>(this));

	t =
	{
		{ const_cast<base::iterator *>(base), it->key()    },
		{ const_cast<base::iterator *>(base), it->value()  },
	};

	return t;
}
