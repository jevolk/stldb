/** 
 *  PUBLIC DOMAIN 2014 (C) Jason Volk
 */


template<class T>
class iterator : public iterator_base<T>
{
	mutable T t;

  public:
	T &operator*() const;
	T *operator->() const;

	iterator(leveldb::DB *const &db,
	         const bool &snap = false,
	         const bool &cache = false);
};


template<class T>
iterator<T>::iterator(leveldb::DB *const &db,
                      const bool &snap,
                      const bool &cache):
iterator_base<T>(db,snap,cache)
{

}


template<class T>
T *iterator<T>::operator->()
const
{
	operator*();
	return &t;
}


template<class T>
T &iterator<T>::operator*()
const
{
	this->t = {this->it->key(),this->it->value()};
	return t;
}
