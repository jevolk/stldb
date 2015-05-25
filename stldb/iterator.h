/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class iterator : public base::iterator,
                 public std::iterator<std::bidirectional_iterator_tag,T>
{
	mutable T t;

  public:
	const T &operator*() const;
	const T *operator->() const;

	T &operator*();
	T *operator->();

	template<class... Args> iterator(Args&&... args): base::iterator(std::forward<Args>(args)...) {}
};


template<class T>
T *iterator<T>::operator->()
{
	operator*();
	return &t;
}


template<class T>
T &iterator<T>::operator*()
{
	const auto &base(static_cast<const base::iterator *>(this));

	t =
	{
		{ const_cast<base::iterator *>(base), this->it->key()    },
		{ const_cast<base::iterator *>(base), this->it->value()  },
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
	t =
	{
		{ nullptr, this->it->key()    },
		{ nullptr, this->it->value()  },
	};

	return t;
}
