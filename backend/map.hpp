#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include <cassert>
#include "pair.hpp"
#include "exception.hpp"
#include <iostream>

namespace sjtu {
enum colorT { RED, BLACK };

template<
	class Key,
	class T,		
	class Compare = std::less<Key>
> class map {
public:
	typedef pair<const Key, T> value_type;
private:
	struct node {
		value_type *data;
		node *leftChild, *rightChild, *father, *next, *prev;
		colorT color;
		node() : data(NULL), leftChild(NULL), rightChild(NULL), father(NULL), prev(NULL), next(NULL), color(BLACK) {}
		node(const value_type &element, node *_leftChild = NULL, node *_rightChild = NULL, node *_father = NULL, node *_prev = NULL, node *_next = NULL, colorT _color = RED) :
			leftChild(_leftChild), rightChild(_rightChild), father(_father), prev(_prev), next(_next), color(_color) {
			data = new value_type(element);
		}
		~node() {
			if (data != NULL) delete data;
		}
	};
	node *root, *nil, *leftMost, *rightMost;
	size_t currentSize;
	Compare comparator;
public:
	int compare(const Key &a, const Key &b) const {
		return comparator(a, b) ? -1 : (comparator(b, a) ? 1 : 0);
	}
	node* findNode(node *p, const Key &key) const {
		if (p == nil) return p;
		int tmp = compare(key, p->data->first);
		if (tmp == 0) return p;
		if (tmp < 0) return findNode(p->leftChild, key);
		return findNode(p->rightChild, key);
	}
	node* findMax(node *p) {
		while (p->rightChild != nil) {
			p = p->rightChild;
		}
		return p;
	}
	node* findMin(node *p) {
		while (p->leftChild != nil) {
			p = p->leftChild;
		}
		return p;
	}
	node* findPrev(node *p) {
		if (p->leftChild != nil) return findMax(p->leftChild);
		while (p != root && p->father->leftChild == p) {
			p = p->father;
		} 
		return p->father;
	}
	node *findNext(node *p) {
		if(p->rightChild != nil) return findMin(p->rightChild);
		while (p != root && p->father->rightChild == p) {
			p = p->father;
		}
		return p->father;
	}
	class const_iterator;
	class iterator {
		friend class map;
	private:
		const map *own;
		node *pointer;
	public:
		iterator(map *_own = NULL, node *_pointer = NULL) : own(_own), pointer(_pointer) {}
		iterator(const iterator &other) : own(other.own), pointer(other.pointer) {}
		iterator(const const_iterator &other) : own(other.own), pointer(other.pointer) {}
		iterator & operator++() {
			if (pointer == own->nil) throw invalid_iterator();
			pointer = pointer->next;
			return *this;
		}
		iterator operator++(int) {
			if (pointer == own->nil) throw invalid_iterator();
			iterator tmp = *this;
			pointer = pointer->next;
			return tmp;
		}
		iterator & operator--() {			
			if (pointer == own->nil) {
				if (own->rightMost == own->nil) throw invalid_iterator();
				pointer = own->rightMost;
				return *this;
			}			
			if (pointer->prev == own->nil) throw invalid_iterator();
			pointer = pointer->prev;
			return *this;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			if (pointer == own->nil) {
				if (own->rightMost == own->nil) throw invalid_iterator();
				pointer = own->rightMost;
				return tmp;
			}
			if (pointer->prev == own->nil) throw invalid_iterator();
			pointer = pointer->prev;
			return tmp;
		}
		value_type & operator*() const {
			return *(pointer->data);
		}
		value_type* operator->() const noexcept {
			return pointer->data;
		}
		bool operator==(const iterator &rhs) const {
			return own == rhs.own && pointer == rhs.pointer;
		}
		bool operator==(const const_iterator &rhs) const {
			return own == rhs.own && pointer == rhs.pointer;
		}
		bool operator!=(const iterator &rhs) const {
			return own != rhs.own || pointer != rhs.pointer;
		}
		bool operator!=(const const_iterator &rhs) const {
			return own != rhs.own || pointer != rhs.pointer;
		}
	};
	class const_iterator {
		friend class map;
	private:
		const map *own;
		node *pointer;
	public:
		const_iterator(const map *_own = NULL, node *_pointer = NULL) : own(_own), pointer(_pointer) {}
		const_iterator(const iterator &other) : own(other.own), pointer(other.pointer) {}
		const_iterator(const const_iterator &other) : own(other.own), pointer(other.pointer) {}
		const_iterator & operator++() {
			if (pointer == own->nil) throw invalid_iterator();
			pointer = pointer->next;
			return *this;
		}
		const_iterator operator++(int) {
			if (pointer == own->nil) throw invalid_iterator();
			const_iterator tmp = *this;
			pointer = pointer->next;
			return tmp;
		}
		const_iterator & operator--() {
			if (pointer == own->nil) {
				if (own->rightMost == own->nil) throw invalid_iterator();
				pointer = own->rightMost;
				return *this;
			}
			if (pointer->prev == own->nil) throw invalid_iterator();
			pointer = pointer->prev;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			if (pointer == own->nil) {
				if (own->rightMost == own->nil) throw invalid_iterator();
				pointer = own->rightMost;
				return tmp;
			}
			if (pointer->prev == own->nil) throw invalid_iterator();
			pointer = pointer->prev;
			return tmp;
		}
		value_type & operator*() const {
			return *(pointer->data);
		}
		value_type* operator->() const noexcept {			
			return pointer->data;
		}
		bool operator==(const iterator &rhs) const {
			return own == rhs.own && pointer == rhs.pointer;
		}
		bool operator==(const const_iterator &rhs) const {
			return own == rhs.own && pointer == rhs.pointer;
		}
		bool operator!=(const iterator &rhs) const {
			return own != rhs.own || pointer != rhs.pointer;
		}
		bool operator!=(const const_iterator &rhs) const {
			return own != rhs.own || pointer != rhs.pointer;
		}
	};
	void copy(node *p, node* &q, node *father, node *leftFather, node *rightFather, node* &leftmost, node* &rightmost) {
		if (p->data == NULL) return;
		q = new node(*(p->data), nil, nil, father, nil, nil, p->color);
		node *leftMin = nil, *leftMax = nil, *rightMin = nil, *rightMax = nil;
		copy(p->leftChild, q->leftChild, q, q, rightFather, leftMin, leftMax);
		copy(p->rightChild, q->rightChild, q, leftFather, q, rightMin, rightMax);
		q->prev = leftMax == nil ? rightFather : leftMax;
		q->next = rightMin == nil ? leftFather : rightMin;
		leftmost = leftMin == nil ? q : leftMin;
		rightmost = rightMax == nil ? q : rightMax;
	}
	void destory(node *p) {
		if (p == nil) return;
		destory(p->leftChild);
		destory(p->rightChild);
		delete p;
	}
	map() : currentSize(0) {
		root = leftMost = rightMost = nil = new node();
		nil->leftChild = nil->rightChild = nil->father = nil->prev = nil->next = nil;
	}
	map(const map &other) : currentSize(other.currentSize) {
		root = leftMost = rightMost = nil = new node();
		nil->leftChild = nil->rightChild = nil->father = nil->prev = nil->next = nil;
		copy(other.root, root, nil, nil, nil, leftMost, rightMost);
	}
	map & operator=(const map &other) {
		if (this == &other) return *this;
		currentSize = other.currentSize;
		if (root != nil) destory(root);
		nil->color = BLACK;
		nil->leftChild = nil->rightChild = nil->father = nil->prev = nil->next = nil;
		root = leftMost = rightMost = nil;
		copy(other.root, root, nil, nil, nil, leftMost, rightMost);
		return *this;
	}
	~map() {
		if (root != nil) destory(root);
		delete nil;
	}
	void clear() {
		currentSize = 0;
		if (root != nil) destory(root);
		root = leftMost = rightMost = nil;
		nil->leftChild = nil->rightChild = nil->father = nil->prev = nil->next = nil;
		nil->color = BLACK;
	}
	T & at(const Key &key) {
		node *p = findNode(root, key);
		if (p == nil) throw index_out_of_bound();
		return p->data->second;
	}
	const T & at(const Key &key) const {
		node *p = findNode(root, key);
		if (p == nil) throw index_out_of_bound();
		return p->data->second;
	}
	T & operator[](const Key &key) {
		node *p = findNode(root, key);
		if (p != nil) return p->data->second;
		pair<iterator, bool> res = insert(value_type(key, T()));
		return (res.first.pointer)->data->second;
	}
	const T & operator[](const Key &key) const {
		node *p = findNode(root, key);
		if (p == nil) throw index_out_of_bound();
		return p->data->second;
	}
	iterator begin() {
		return iterator(this, leftMost);
	}
	const_iterator cbegin() const {		
		return const_iterator(this, leftMost);
	}
	iterator end() {
		return iterator(this, nil);
	}
	const_iterator cend() const {
		return const_iterator(this, nil);
	}
	bool empty() const {
		return currentSize == 0;
	}
	size_t size() const {
		return currentSize;
	}

	void rotateLeft(node *x) {
		node *y = x->rightChild;
		if (y == nil) return;
		x->rightChild = y->leftChild;
		if (y->leftChild != nil) y->leftChild->father = x;

		if (x->father == nil) root = y;
		else if (x->father->leftChild == x) x->father->leftChild = y;
		else x->father->rightChild = y;
		y->father = x->father;

		x->father = y;
		y->leftChild = x;
	}
	void rotateRight(node *x) {
		node *y = x->leftChild;
		if (y == nil) return;
		x->leftChild = y->rightChild;
		if (y->rightChild != nil) y->rightChild->father = x;

		if (x->father == nil) root = y;
		else if (x->father->leftChild == x) x->father->leftChild = y;
		else x->father->rightChild = y;
		y->father = x->father;

		x->father = y;
		y->rightChild = x;
	}
	void insertFixUp(node *self) {
		while (self != root && self->father->color == RED) {
			node *fa = self->father;
			if (fa == fa->father->leftChild) {
				node *uncle = fa->father->rightChild;
				if (uncle->color == RED) {
					fa->color = uncle->color = BLACK;
					fa->father->color = RED;
					self = fa->father;
				}
				else {
					if (self == fa->rightChild) {
						self = fa;
						rotateLeft(self);
						fa = self->father;
					}
					fa->color = BLACK;
					fa->father->color = RED;
					rotateRight(fa->father);
				}
			}
			else {
				node *uncle = fa->father->leftChild;
				if (uncle->color == RED) {
					fa->color = uncle->color = BLACK;
					fa->father->color = RED;
					self = fa->father;
				}
				else {
					if (self == fa->leftChild) {
						self = fa;
						rotateRight(self);
						fa = self->father;
					}
					fa->color = BLACK;
					fa->father->color = RED;
					rotateLeft(fa->father);
				}
			}
		}
	}
	pair<iterator, bool> insert(const value_type &value) {
		Key key = value.first;
		if (root == nil) {
			currentSize++;
			root = new node(value, nil, nil, nil, nil, nil, BLACK);
			leftMost = rightMost = root;
			return pair<iterator, bool>(iterator(this, root), true);
		}
		node *fa = root;
		while (fa != nil) {			
			if (comparator(key, fa->data->first)) {
				if (fa->leftChild == nil) break;
				fa = fa->leftChild;
			}
			else if (comparator(fa->data->first, key)) {
				if (fa->rightChild == nil) break;
				fa = fa->rightChild;
			}
			else return pair<iterator, bool>(iterator(this, fa), false);
		}
		currentSize++;
		node *self = new node(value, nil, nil, fa, nil, nil, RED);
		if (comparator(key, fa->data->first)) fa->leftChild = self;
		else fa->rightChild = self;

		insertFixUp(self);		
		self->prev = findPrev(self);
		self->next = findNext(self);
		if (self->prev != nil) self->prev->next = self;
		else leftMost = self;
		if (self->next != nil) self->next->prev = self;
		else rightMost = self;
		return pair<iterator, bool>(iterator(this, self), true);
	}
	void eraseFixUp(node *self) {
		while (self != root && self->color == BLACK) {
			node *fa = self->father;
			if (self == fa->leftChild) {
				node *brother = fa->rightChild;
				if (brother->color == RED) {
					brother->color = BLACK;
					fa->color = RED;
					rotateLeft(fa);
					brother = fa->rightChild;
				}
				if (brother->leftChild->color == BLACK && brother->rightChild->color == BLACK) {
					brother->color = RED;
					self = self->father;
				}
				else {
					if (brother->rightChild->color == BLACK) {
						brother->leftChild->color = BLACK;
						brother->color = RED;
						rotateRight(brother);
						brother = fa->rightChild;
					}
					brother->color = fa->color;
					fa->color = BLACK;
					brother->rightChild->color = BLACK;
					rotateLeft(fa);
					self = root;
				}
			}
			else {
				node *brother = fa->leftChild;
				if (brother->color == RED) {
					brother->color = BLACK;
					fa->color = RED;
					rotateRight(fa);
					brother = fa->leftChild;
				}
				if (brother->leftChild->color == BLACK && brother->rightChild->color == BLACK) {
					brother->color = RED;
					self = self->father;
				}
				else {
					if (brother->leftChild->color == BLACK) {
						brother->rightChild->color = BLACK;
						brother->color = RED;
						rotateLeft(brother);
						brother = fa->leftChild;
					}
					brother->color = fa->color;
					fa->color = BLACK;
					brother->leftChild->color = BLACK;
					rotateRight(fa);
					self = root;
				}
			}
		}
		self->color = BLACK;
	}
	void erase(iterator iter) {
		if (iter.pointer == nil || iter.own != this) throw invalid_iterator();
		node *p = iter.pointer, *q;

		currentSize--;

		if (currentSize == 0) {
			clear();
			return;
		}

		if (p->leftChild != nil && p->rightChild != nil) {
			node* real = p->next;

			if (p == root) root = real;
			else {
				if (p->father->leftChild == p) p->father->leftChild = real;
				else p->father->rightChild = real;
			}
			if (real->father->leftChild == real) real->father->leftChild = p;
			else real->father->rightChild = p;

			node *tmp = real->father;
			real->father = p->father;
			p->father = tmp;

			tmp = p->leftChild;
			tmp->father = real;
			p->leftChild = real->leftChild;
			real->leftChild = tmp;
			
			tmp = p->rightChild;
			tmp->father = real;
			p->rightChild = real->rightChild;
			real->rightChild = tmp;
		
			real->prev = p->prev;
			if (p->prev != nil) p->prev->next = real;
			else leftMost = real;
		}
		else {
			if (p->prev != nil) p->prev->next = p->next;
			else leftMost = p->next;
			if (p->next != nil) p->next->prev = p->prev;
			else rightMost = p->prev;
		}

		q = p->leftChild == nil ? p->rightChild : p->leftChild;

		q->father = p->father;
		if (p->father == nil) root = q;
		else if (p == p->father->leftChild) p->father->leftChild = q;
		else p->father->rightChild = q;

		if (p->color == BLACK) eraseFixUp(q);
		nil->color = BLACK;
		nil->leftChild = nil->rightChild = nil->father = nil->prev = nil->next = nil;
		delete p;
	}
	size_t count(const Key &key) const {
		return findNode(root, key) != nil;
	}
	iterator find(const Key &key) {
		return iterator(this, findNode(root, key));
	}
	const_iterator find(const Key &key) const {
		return const_iterator(this, findNode(root, key));
	}
};

}

#endif
