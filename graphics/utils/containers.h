#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include <list>
#include <map>
#include "pointers.h"
#include "log.h"

namespace stdex{

template <typename classname, typename returntype>
class iterator{
	sizetype position = 0;
	const classname* pcontainer = nullptr;
	returntype(*getelement)(const classname*, sizetype) = nullptr;
public:
	iterator(const classname* cnt, sizetype pos, returntype(*pfnc)(const classname*,sizetype))
		: pcontainer(cnt), position(pos), getelement(pfnc)
	{ }

	bool operator != (const iterator& other){
		return position != other.position;
	}

	returntype operator*() const{
		return getelement(pcontainer, position);
	}

	const iterator& operator++(){
		++position; return *this;
	}
};

template <typename Type> class container{
	std::vector<Type*> elements;
public:
	container(){}

	Type& operator[](uint i) const{
		if(i >= elements.size())
			LOG_ERR("i >= elements.size()!");
		return *elements[i];
	}

	uint add(Type* e){
		elements.emplace_back(e);
		return (uint)elements.size()-1;
	}
	uint add(Type e){
		uint i = add(__new Type());
		*elements[i] = e;
		return i;
	}
	Type& add(){
		uint i = add(__new Type());
		return *elements[i];
	}
	void remove(uint i, uint count = 1){
		if(i >= elements.size())
			LOG_ERR("i >= elements.size()!");

		for(uint p = 0; p < count; ++p){
			auto ptr = elements[i+p];
			__release_ptr(ptr);
		}

		for(; i < elements.size()-count; ++i){
			elements[i] = elements[i+count];
		}

		elements.resize(elements.size()-count);
	}
	void swap(uint i, uint j){
		auto tmp = elements[i];
		elements[i] = elements[j];
		elements[j] = tmp;
	}
	uint size() const{ return (uint)elements.size(); }

	container<Type>& operator += (const container<Type>& other){
		for(uint i = 0; i < other.size(); ++i){
			this->add() = other[i];
		}
		return *this;
	}
	container<Type>& operator = (const container<Type>& other){
		this->clear();
		return this->operator+=(other);
	}

	void clear(){
		for(auto it = elements.begin(); it != elements.end(); ++it)
			__release_ptr(*it);
	}
	~container(){
		clear();
	}
	
	friend class iterator<container<Type>, Type*>;
	iterator<container<Type>, Type*> begin() const{ return iterator<container<Type>, Type*>(this, 0, [](const container<Type>* ptr, sizetype pos){ return ptr->elements[pos]; }); }
	iterator<container<Type>, Type*> end() const{ return iterator<container<Type>, Type*>(this, elements.size(), [](const container<Type>* ptr, sizetype pos){ return ptr->elements[pos]; }); }
};

template <typename type>
bool CheckNotInList(type pt, std::list<type>& points){
	for(auto it = points.begin(); it != points.end(); ++it)
		if(pt == *it) return false;
	return true;
}
template <typename type, typename containerType>
bool CheckInContainer(const type& pt, containerType& points, bool(*check)(const type& a, type& b)){
	for(auto it = points.begin(); it != points.end(); ++it)
		if(check(pt, *it) == false) return false;
	return true;
}

}

#endif //CONTAINER_H