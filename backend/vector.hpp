#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP
 
#include "exception.hpp"
 
#include <climits>
#include <cstddef>
 
namespace sjtu {
	/**
	 * a data container like std::vector
	 * store data in a successive memory and support random access.
	 */
	template<typename T>
	class vector {
	private:
		T **data;
		size_t len, maxsize;
 
		void doublespace() {
			maxsize *= 2;
			T **temp = new T *[maxsize];
			for (size_t i = 0; i < maxsize; ++i)
				temp[i] = nullptr;
			for (size_t i = 0; i < len; ++i)
				temp[i] = new T(*data[i]);
 
			for (size_t i = 0; i < len; ++i) {
				if (data[i] != NULL) delete data[i];
			}
			delete []data;
 
			data = temp;
		}
 
	public:
		/**
		 * TODO
		 * a type for actions of the elements of a vector, and you should write
		 *   a class named const_iterator with same interfaces.
		 */
		 /**
		  * you can see RandomAccessIterator at CppReference for help.
		  */
		class const_iterator;
		class iterator {
			friend class vector;
		private:
			T **me;
			size_t pos;
			
		public:
			iterator() {}
			iterator(T **_me, size_t _pos) {
				me = _me;
				pos = _pos;
			}
			iterator(const iterator &other) {
				me = other.me;
				pos = other.pos;
			}
 
			/**
			 * return a new iterator which pointer n-next elements
			 *   even if there are not enough elements, just return the answer.
			 * as well as operator-
			 */
			iterator operator+(const int &n) const {
				return iterator(me, pos + n);
			}
			iterator operator-(const int &n) const {
				return iterator(me, pos - n);
			}
			// return th distance between two iterator,
			// if these two iterators points to different vectors, throw invaild_iterator.
			int operator-(const iterator &rhs) const {
				if (me != rhs.me) throw(invalid_iterator());
				return pos - rhs.pos;
			}
			iterator operator+=(const int &n) {
				pos += n;
			}
			iterator operator-=(const int &n) {
				pos -= n;
			}
			/**
			 * TODO iter++
			 */
			iterator operator++(int) {
				iterator temp(*this);
				++pos;
				return temp;
			}
			/**
			 * TODO ++iter
			 */
			iterator& operator++() {
				++pos;
				return *this;
			}
			/**
			 * TODO iter--
			 */
			iterator operator--(int) {
				iterator temp(*this);
				--temp.pos;
				return temp;
			}
			/**
			 * TODO --iter
			 */
			iterator& operator--() {
				--pos;
				return *this;
			}
			/**
			 * TODO *it
			 */
			T& operator*() const {
				return *me[pos];
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const {
				return (pos == rhs.pos && me == rhs.me);
			}
			bool operator==(const const_iterator &rhs) const {
				return (*this == rhs.citr);
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
		};
		/**
		 * TODO
		 * has same function as iterator, just for a const object.
		 */
		class const_iterator {
			friend class vector;
		private:
			iterator citr;
 
		public:
			const_iterator() {}
			const_iterator (const iterator &other) {
				citr = other;
			}
 
			const_iterator (T **_me, size_t _pos) {
				citr = iterator(_me, _pos);
			}
 
			const_iterator (const const_iterator &other) {
				citr = other.citr;
			}
 
			const_iterator operator+(const int &n) const {
				return (citr + n);
			}
 
			const_iterator operator-(const int &n) const {
				return (citr - n);
			}
 
			int operator-(const const_iterator &rhs) const {
				return citr - rhs.citr;
			}
 
			const_iterator operator+=(const int &n) {
				citr += n;
			}
 
			const_iterator operator-=(const int &n) {
				citr -= n;
			}
 
			const_iterator operator++(int) {
				const_iterator temp(*this);
				++temp.citr;
				return temp;
			}
			const_iterator& operator++() {
				++citr;
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator temp(*this);
				--temp.citr;
				return temp;
			}
			const_iterator& operator--() {
				--citr;
				return *this;
			}
			const T& operator*() const {
				return *citr;
			}
			bool operator==(const iterator &rhs) const {
				return (citr == rhs.itr);
			}
			bool operator==(const const_iterator &rhs) const {
				return (citr == rhs.citr);
			}
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
 
		};
		/**
		 * TODO Constructs
		 * Atleast three: default constructor, copy constructor and a constructor for std::vector
		 */
		vector() {
			maxsize = 10;
			data = new T *[maxsize];
			for (size_t i = 0; i < maxsize; ++i)
				data[i] = nullptr;
			len = 0;
		}
		vector(const vector &other) {
			maxsize = other.maxsize;
			data = new T*[maxsize];
			len = other.len;
			for (size_t i = 0; i < maxsize; ++i)
				data[i] = nullptr;
			for (size_t i = 0; i < len; ++i)
				data[i] = new T(*other.data[i]);
		}
		/**
		 * TODO Destructor
		 */
		~vector() {
			for (size_t i = 0; i < maxsize; ++i) {
				if (data[i] != NULL) delete data[i];
			}
			delete[] data;
		}
		/**
		 * TODO Assignment operator
		 */
		vector &operator=(const vector &other) {
			if (this == &other) return *this;
			for (size_t i = 0; i < len; ++i)
				delete data[i];
			delete []data;
			maxsize = other.maxsize;
 
			data = new T* [maxsize];
			for (size_t i = 0; i < maxsize; ++i)
				data[i] = nullptr;
			len = other.len;
			for (size_t i = 0; i < len; ++i)
				data[i] = new T(*other.data[i]);
			return *this;
		}
		/**
		 * assigns specified element with bounds checking
		 * throw index_out_of_bound if pos is not in [0, size)
		 */
		T & at(const size_t &pos) {
			if (pos < 0 || pos >= len) throw(index_out_of_bound());
			return *data[pos];
		}
		const T & at(const size_t &pos) const {
			if (pos < 0 || pos >= len) throw(index_out_of_bound());
			return *data[pos];
		}
		/**
		 * assigns specified element with bounds checking
		 * throw index_out_of_bound if pos is not in [0, size)
		 * !!! Pay attentions
		 *   In STL this operator does not check the boundary but I want you to do.
		 */
		T & operator[](const size_t &pos) {
			if (pos < 0 || pos >= len) throw(index_out_of_bound());
			return *data[pos];
		}
		const T & operator[](const size_t &pos) const {
			if (pos < 0 || pos >= len) throw(index_out_of_bound());
			return *data[pos];
		}
		/**
		 * access the first element.
		 * throw container_is_empty if size == 0
		 */
		const T & front() const {
			if (len == 0) throw(container_is_empty());
			return *data[0];
		}
		/**
		 * access the last element.
		 * throw container_is_empty if size == 0
		 */
		const T & back() const {
			if (len == 0) throw(container_is_empty());
			return *data[len - 1];
		}
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin() {
			return iterator(data, 0);
		}
		const_iterator cbegin() const {
			return const_iterator(data, 0);
		}
		/**
		 * returns an iterator to the end.
		 */
		iterator end() {
			return iterator(data, len);
		}
		const_iterator cend() const {
			return const_iterator(data, len);
		}
		/**
		 * checks whether the container is empty
		 */
		bool empty() const {
			return len == 0;
		}
		/**
		 * returns the number of elements
		 */
		size_t size() const {
			return len;
		}
		/**
		 * returns the number of elements that can be held in currently allocated storage.
		 */
		size_t capacity() const {
			return maxsize;
		}
		/**
		 * clears the contents
		 */
		void clear() {
			for (size_t i = 0; i < maxsize; ++i){
				if (data[i]) delete data[i];
				data[i] = nullptr;
			}
			len = 0;			
		}
		/**
		 * inserts value before pos
		 * returns an iterator pointing to the inserted value.
		 */
		iterator insert(iterator pos, const T &value) {
			return (insert(pos.pos, value));
		}
		/**
		 * inserts value at index ind.
		 * after inserting, this->at(ind) == value is true
		 * returns an iterator pointing to the inserted value.
		 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
		 */
		iterator insert(const size_t &ind, const T &value) {
			if (ind > len || ind < 0) throw(index_out_of_bound());
 
			if (len + 1 == maxsize) doublespace();
			++len;
			for (size_t i = len - 1; i > ind; --i)
				data[i] = data[i - 1];
 
			data[ind] = new T(value);
			return iterator(data, ind);
		}
		/**
		 * removes the element at pos.
		 * return an iterator pointing to the following element.
		 * If the iterator pos refers the last element, the end() iterator is returned.
		 */
		iterator erase(iterator pos) {
			return (erase(pos.pos));
		}
		/**
		 * removes the element with index ind.
		 * return an iterator pointing to the following element.
		 * throw index_out_of_bound if ind >= size
		 */
		iterator erase(const size_t &ind) {
			if (ind >= len || ind < 0) throw(index_out_of_bound());
			delete data[ind];
			for (size_t i = ind; i < len - 1; ++i)
				data[i] = data[i + 1];
			data[len - 1] = nullptr;
			len--;
			return iterator(data, ind);
		}
		/**
		 * adds an element to the end.
		 */
		void push_back(const T &value) {
			if (len + 1 == maxsize) doublespace();
			data[len++] = new T(value);
		}
		/**
		 * remove the last element from the end.
		 * throw container_is_empty if size() == 0
		 */
		void pop_back() {
			if (size() == 0) throw(container_is_empty());
			delete data[len - 1];
			data[len - 1] = nullptr;
			len--;
		}
	};
 
 
}
 
#endif