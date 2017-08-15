class _Ref_count_base            //指针引用类型的基类
{
	typedef unsigned long  _Atomic_counter_t;
private:
	virtual void _Destroy() = 0;
	virtual void _Delete_this() = 0;
private:
	_Atomic_counter_t _Uses;      //智能指针的引用次数
	_Atomic_counter_t _Weaks;     //观察的weak_ptr的数目，在没有观察者时此值为1,当_Users减为零时，会销毁资源并将_Weaks减一，如果没有观察者，才会销毁引用次数对象本身
protected:
	_Ref_count_base()
	{
		_Uses = 1;
		_Weaks = 1;

		//_Init_atomic_counter(_Uses, 1);
		//_Init_atomic_counter(_Weaks, 1);
	}
public:
	virtual ~_Ref_count_base() throw()
	{
		//
	}
	bool _Incref_nz()
	{

	}
	unsigned int _Get_uses()const
	{
		//return (_Get_atomic_count(_Uses));
		return _Uses;
	}
	void _Incref()
	{
		//_MT_INCR(_Mtx, _Uses);
		_Uses++;

	}
	void _Incwref(){
		//_MT_INCR(_MTX, _Weaks);
		_Weaks++;
	}
	void _Decref()
	{
		//if (_MT_DECR(_Mtx,_Uses)==0)
		if (--Uses==0)
		{
			_Destroy();
			_Decwref();
		}
	}
	void _Decwref()
	{
		//if (_MT_DECR(_Mtx, _Weaks) == 0)
		if (--Weaks==0)
			_Delete_this();
	}
	long _Use_count()const
	{
		return (_Get_uses());
	}
	bool _Expired()const
	{
		return (_Get_uses() == 0);
	}
	virtual void* _Get_deleter(const _XSTD2 type_info&)const
	{
		return (0);
	}
};
template<class _Ty>
class _Ref_count :                  //智能指针引用次数，无删除操作的实现类
	public _Ref_count_base
{
public:
	_Ref_count(_Ty * _Px)
		: _Ref_count_base(), _Ptr(_Px)
	{

	}
private:
	virtual void _Destroy()
	{
		delete _Ptr;
	}
	virtual void _Delete_this()
	{
		delete this;
	}
	_Ty* _Ptr;
};
template<class _Ty,
	class _Dx>
class _Ref_count_del           //智能指针的引用次数，带删除操作的实现类 
	:public _Ref_count_base
{
public:
	_Ref_count_del(_Ty* _Px, _Dx _Dt)
		: _Ref_count_base(), _Ptr(_Px), _Dtor(_Dt)
	{

	}
	virtual void * _Get_deleter(const _XSTD2 type_info& _Typeid)const
	{
		return ((void*)(_Typeid == typeid(_Dx) ? &_Dtor : 0));
	}
private:
	virtual void _Destroy()
	{
		_Dtor(_Ptr);
	}
	virtual void _Delete_this()
	{
		delete this;
	}
	_Ty * _Ptr;
	_Dx _Dtor;
};
                                                 //weak_ptr和shared_ptr的基类		
template<class _Ty>
class _Ptr_base
{
private:
	_Ty* _Ptr;                 //封装的资源指针
	_Ref_count_base * _Rep;    //引用计数对象的指针
	template<class _Ty0>
	friend class _Ptr_base;
public:
	typedef _Ptr_base<_Ty> _Myt;
	typedef _Ty element_type;
	
	_Ptr_base()
		: _Ptr(0), _Rep(0)           //两个指针值被初始化为nullptr
	{
		//construct
	}
	_Ptr_base(_Myt&& _Right)
		: _Ptr(0), _Rep(0)
	{
		_Assign_rv(_STD forward <Myt>(_Right));
	}
	void _Swap(_Ptr_base& _Right)
	{
		_STD swap(_Rep, _Right._Rep);
		_STD swap(_Ptr, _Right._Ptr);
	}
	_Ty * _Get()const
	{
		return (_Ptr);
	}
	long use_count() const noexcept
	{
		return (_Rep ? _Rep->Use_count() : 0);
	}
	bool _Expired() const
	{
		return (!_Rep || _Rep->_Expired());
	}
	void _Decref()
	{
		if (_Rep!=0)
		{
			_Rep->_Decref();
		}
	}
	void _Reset()
	{
		_Reset(0, 0);
	}
	template <class _Ty2>
	void _Reset(const _Ptr_base<_Ty2> & _Other)
	{
		_Reset(_Other._Ptr, _Other._Rep);
	}
	template<class _Ty2>
	void _Reset(const _Ptr_Base<_Ty2>& Other, bool _Throw)
	{
		_Reset(_Other._Ptr, _Other._Rep, _Throw);
	}
	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, const _Const_tag&)
	{
		_Reset(const_cast<_Ty*>(_Other.Ptr), _Other._Rep);
	}
	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, const _Dynamic_tag&)
	{
		_Ty * _Pty = dynamic_cast<_Ty*>(_Other._Ptr);
		if (_Ptr)
		{
			_Reset(_Ptr, _Other._Rep);
		}
		else
			_Reset();
	}
	template <class _Ty2>
	void _Reset(_Ty* _Ptr, const _Ptr_base<_Ty2> & _Other)
	{
		_Reset(_Ptr, _Other._Rep);
	}
	void _Reset(_Ty*_Other_ptr, _Ref_count_base * _Other_rep)       //核心函数，在复制构造函数中被调用，

	{
		if (_Other_rep)
		{
			_Other_rep->_Incref();                                  //增加被复制对象的引用计数
		}
		_Reset0(_Other_ptr, _Other_rep);                            //减少被赋值对象的引用计数，并将资源指针和引用计数指针指向被复制对象中的值。
	}
	void _Reset(_Ty* _Other_ptr, _Ref_count_base*_Other_rep, bool _Throw)
	{
		if (_Other_rep&&_Other_rep->_Incref_nz())
		{
			_Reset0(_Other_ptr, _Other_rep);
		}
		else if (_Throw)
		{
			_THROW_NCEE(bad_weak_ptr, 0);
		}
	}
	void _Reset0(_Ty*_Other_ptr, _Ref_count_base* _Other_rep)
	{
		if (_Rep!=0)
		{
			_Rep->_Decref();
		}
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
	}
	void _Decwref()
	{	
		if (_Rep != 0)
			_Rep->_Decwref();
	}

	void _Resetw()
	{	
		_Resetw((_Ty *)0, 0);
	}

	template<class _Ty2>
	void _Resetw(const _Ptr_base<_Ty2>& _Other)
	{	
		_Resetw(_Other._Ptr, _Other._Rep);
	}

	template<class _Ty2>
	void _Resetw(const _Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)
	{	
		_Resetw(const_cast<_Ty2*>(_Other_ptr), _Other_rep);
	}

	template<class _Ty2>
	void _Resetw(_Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)  //weak_ptr和shared_ptr一视同仁，都是增加赋值式右边的w引用数目，减少赋值式左边的w引用次数，
		                                                         //并将赋值式左边智能指针对象的资源指针与引用对象指针指向赋值式右边对象的相应指针。
	{	
		if (_Other_rep)
			_Other_rep->_Incwref();
		if (_Rep != 0)
			_Rep->_Decwref();
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
	}

};

//shared_ptr 智能指针在将派生类指针复制给基类指针的时候，将基类智能指针的对象引用数减一，派生类智能指针的引用数加一，
//再将派生类的引用对象指针和资源的指针赋给基类,这个过程通过成员模板来完成
template<class _Ty>
class share_ptr
	:public _Ptr_base<_Ty>
{
public:
	typedef shared_ptr<_Ty> _Myt;
	typedef _Ptr_base<_Ty> _Mybase;
	shared_ptr() _NOEXCEPT
	{

	}
	template<class _Ux>
	explicit shared_ptr(_Ux*_Px)
	{
		_Resetp(_Px);
	}
	template<class _Ux,class _Dx>
	shared_ptr(_Ux *_Px, _Dx _Dt)
	{
		_Resetp(_Px, _Dt);
	}
	shared_ptr(nullptr_t)
	{

	}
	template<class _Dx>
	shared_ptr(nullptr_t, _Dx _Dt)
	{
		_Resetp((_Ty*)0, _Dt);
	}
	template<class _Dx,class _Alloc>
	shared_ptr(nullptr_t, _Dx _Dt, _Alloc _Ax)
	{
		_Resetp((_Ty*)0, _Dt, _Ax);
	}
	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Right, _Ty *_Px) _NOEXCEPT
	{	
		this->_Reset(_Px, _Right);
	}

	shared_ptr(const _Myt& _Other) _NOEXCEPT
	{	// construct shared_ptr object that owns same resource as _Other
		this->_Reset(_Other);
	}
	// template<bool _Test,
	// class _Ty = void>
	// struct enable_if
	// {	// type is undefined for assumed !_Test
	// };

	// template<class _Ty>
	// struct enable_if<true, _Ty>
	// {	// type is _Ty for _Test
	//	typedef _Ty type;
	// };
	template<class _Ty2,class = typename enable_if<is_convertible<_Ty2*,_Ty*>::value,
		        void>::type>
				shared_ptr(const shared_ptr<_Ty2>& _Other) _NOEXCEPT
	{
		this->_Reset(_Other);
	}

	template<class _Ty2>
	explicit shared_ptr(const weak_ptr<_Ty2> & _Other, bool _Throw = true)
	{
		this->_Reset(_Other, _Throw);
	}
    
	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>&_Other, const _Static_tag& _Tag)
	{
		this->_Reset(_Other, _Tag);
	}
	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>&_Other, const _Const_tag&_Tag)
	{
		this->_Reset(_Other, _Tag);
	}
	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Other, const _Dynamic_tag&_Tag)
	{
		this->_Reset(_Other, _Tag);
	}
	~shared_ptr() _NOEXCEPT
	{
		this->_Decref();
	}
	_Myt& operator=(const _Myt&_Right) _NOEXCEPT
	{
		shared_ptr(_Right).swap(*this);
		return (*this);
	}
	template<class _Ty2>
	_Myt&operator=(const shared_Ptr<_Ty2>& _Right) _NOEXCEPT
	{
		shared_ptr(_Right).swap(*this);
		return (*this);
	}
	void reset() _NOEXCEPT
	{
		shared_ptr().swap(*this);
	}
	template<class _Ux>
	void reset(_Ux *_Px)
	{
		shared_ptr(_Px).swap(*this);
	}
	template<class _Ux,class _Dx>
	void reset(_Ux* _Px, _Dx _Dt)
	{
		shared_ptr(_Px, _Dt).swap(*this);
	}
	template<class _Ux,class _Dx,class _Alloc>
	void reset(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
	{
		shared_ptr(_Px, _Dt, _Ax).swap(*this);
	}
	void swap(_Myt& _Other) _NOEXCEPT
	{
		this->_Swap(_Other);
	}
	_Ty* get()const _NOEXCEPT
	{
		return (this->_Get());
	}
	//	template<class _Ty>
	//	struct add_reference
	//	{	// add reference
	//		typedef _Ty& type;
	//	};
	typename add_reference<_Ty>::type operator*()const _NOEXCEPT
	{
		return (*this->_Get());
	}
	_Ty*operator->()const _NOEXCEPT
	{
		return (this->_Get())
	}
	explicit operator bool() const _NOEXCEPT
	{
		return (this->_Get() != 0);    //可将类型转换运算符定义为显示的避免隐式转换，但是如果表达式用于条件时，显示的操作转换符也会被隐式执行
	}
private:
	template<class _Ux>
	void _Resetp(_Ux * _Px)              //重要，给资源指针和引用计数对象指针设置值
	{
		//_TRY_BEGIN
		try
		{
		_Resetp0(_Px, new _Ref_count<_Ux>(_Px));
		
		}
		catch (...)
		{
			delete _Px;
			throw;
		}	
	}
	template<class _Ux,class _Dx>
	void _Reset(_Ux *_Px, _Dx _Dt)        //重要，给资源指针和引用计数对象指针设置值
	{
		try{
			_Resetp0(_Px, new _Ref_count_del<_Ux, _Dx>(_Px, _Dt));
		}
		catch (...)
		{
			_Dt(_Px);
			throw;
		}
	}
	template<class _Ux,
		class _Dx,
		class _Alloc>
			void _Resetp(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
		{
				typedef _Ref_count_del_alloc<_Ux, _Dx, _Alloc> _Refd;
				typedef _Alloc::template rebind<_Refd>::other _Al = Ax;
				try{
					_Refd * _Ptr = _Al.allocate(1);
					::new (_Ptr)_Refd(_Px, _Dt, _Al);
					_Reset0(_Px, _Ptr);
				}
				catch (...)
				{
					_Dt(_Px);
					throw;
				}
			}
public:
	template<class _Ux>
	void _Resetp0(_Ux * _Px, _Ref_count_base * _Rx)
	{
		this->_Reset0(_Px, _Rx);        //减少被赋值对象的引用计数，并将资源指针和引用计数指针指向被复制对象中的值。   
		_Enable_shared(_Px, _Rx);
	}
};
template<class _Ty1,
	class _Ty2>
		bool operator==(const shared_ptr<Ty1> & _Left,
		const shared_ptr<_Ty2>& _Right) _NOEXCEPT
	{
		return (_Left.get() == _Right.get());
	}
template <class _Ty1,
	class _Ty2>
		bool operator!=(const shared_ptr<_Ty1> & _Left,
		const shared_ptr<_Ty2>& _Right) _NOEXCEPT
	{
		return (!(_Left == _Right));
	}
template<class _Ty1,
	class _Ty2>
		bool operator<(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right) _NOEXCEPT
	{
		return (less<decltype(_Always_false<_Ty1>::value
		? _Left.get() : _Right.get())>()(
		_Left.get(), _Right.get()));
	}
template<class _Elem,
	class _Traits,
	class _Ty>
	basic_ostream<_Elem,_Traits>&
	operator <<(basic_ostream<_Elem, _Traits>&_Out,
	const shared_ptr<_Ty>& _Px)
	{
		return _Out << _Px.get();
	}


template<class _Ty,class... _Types> inline
shared_ptr<_Ty> make_shared(_Types&&... _Args)
{
	_Ref_count_obj<_Ty>* _Rx =  new _Ref_count_obj<_Ty>(_STD forward<_Types>(_Args)...);
	shared_ptr<_Ty> _Ret;
	_Ret._Resetp0(_Rx->_Getptr(), _Rx);
	return (_Ret);
}


//weak_ptr类
template<class _Ty>
class weak_ptr
	:public _Ptr_base<_Ty>
{
public:
	weak_ptr() _NOEXCEPT
	{

	}
	weak_ptr(const weak_ptr& _Other) _NOEXCEPT
	{
		this->_Resetw(_Other);
	}
	template<class _Ty2,
		class = typename enable_if<is_convertible<_Ty2*,_Ty *> ::value,
			void>::type>
			weak_ptr(const shared_ptr <_Ty2>&_Other) _NOEXCEPT
		{
			this->_Resetw(_Other);
		}
	template<class _Ty2,
		class = typename enable_if<is_convertible<_Ty2 *,_Ty *>::value,
			void>::type>
			weak_ptr(const weak_ptr<_Ty2>& _Other) _NOEXCEPT
		{
			this->_Resetw(_Other.lock());
		}
		~weak_ptr() _NOEXCEPT
		{
			this->_Decwref();
		}
		weak_ptr & operator=(const weak_ptr& _Right) _NOEXCEPT
		{
			this->_Reset(_Right);
			return (*this);
		}
		template<class _Ty2>
		weak_ptr & operator = (const weak_ptr<_Ty2> &Right) _NOEXCEPT
		{
			this->_Resetw(_Right.lock());
			return (*this);
		}
		template<class _Ty2>
		weak_ptr & operator= (const shared_ptr<_Ty2>& _Right) _NOEXCEPT
		{
			this->_Resetw(_Right);
			return (*this);
		}
		void reset() _NOEXCEPT
		{
			this->_Resetw();                                //将资源指针与引用对象指针都设为空。
		}
		void swap(weak_ptr& _Other) _NOEXCEPT
		{
			this->_Swap(_Other);
		}
		bool expired()const _NOEXCEPT
		{
			return (this->_Expired());
		}
		shared_ptr<_Ty> lock()const _NOEXCEPT
		{
			return (shared_ptr<_Ty>(*this, false));
		}
};
template<class _Ty>
void swap(weak_ptr<_Ty>& _W1, weak_ptr<_Ty>& _W2) _NOEXCEPT
{
	_W1.swap(_W2);
}