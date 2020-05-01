#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include <list>
#include <map>
#include "pointers.h"
#include "log.h"

namespace stdex{

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

};
/*
template <typename Type> class array{
	std::vector<Type> elements;
public:
	array(){}

	Type& operator[](uint i){
		if(i >= elements.size())
			LOG_ERR("i >= elements.size()!");
		return elements[i];
	}

	uint add(Type& e){
		elements.emplace_back(e);
		return (uint)elements.size() - 1;
	}
	Type& add(){
		Type e();
		uint i = add(e);
		return elements[i];
	}
	void resize(uint newSize){
		elements.resize(newSize);
	}
	uint size(){ return (uint)elements.size(); }

	void clear(){
		elements.clear();
	}
	~array(){
		clear();
	}
};

template <typename Type> class list{
	std::list<Type> elements;
public:
	list(){}

	uint add(Type& e){
		elements.emplace_back(e);
		return (uint)elements.size()-1;
	}
	Type& add(){
		return *elements.emplace();
	}
	uint size(){ return (uint)elements.size(); }

	auto begin(){ return elements.begin(); }
	auto end(){ return elements.end(); }

	void clear(){
		elements.clear();
	}
	~list(){
		clear();
	}
};

template <typename Key, typename Type> class map{
	std::map<Key, Type> elements;
public:
	map(){}

	auto operator[](Key key){
		return elements[key]; }

	auto begin(){ return elements.begin(); }
	auto end(){ return elements.end(); }

	uint size(){ return (uint)elements.size(); }

	uint add(Key key, Type value){
		elements[key] = value; return size();
	}
	auto insert(Key key, Type value){
		return elements.insert({ key, value });
	}

	auto find(Key key){ return elements.find(key); }
};
*/

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