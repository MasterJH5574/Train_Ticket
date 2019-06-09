#include "exception.hpp"
#include <iostream>
#include <functional>
#include "disk_manager.hpp"
#include "pair.hpp"
#include "vector.hpp"
#include <cstring>

#include <assert.h>

#ifndef _BP_TREE
#define _BP_TREE

#define DEB
#define RELEASE

using namespace std;

template<class Key, class value_t, size_t node_size = 4096, class Compare = std::less<Key>>
class BP_Tree {
	struct node;
	typedef char buffer[node_size - sizeof(node)];
	typedef char* _buffer;

private:
	const size_t Node_max;
	const size_t Leaf_max;

	int sz;
	off_t head, root, tail;
	char* filename, * index_file;
	disk_manager disk_manage;
	//buffer buf;
	Compare cmp = Compare();
    inline bool equal(const Key& l, const Key& r) {
        return (!cmp(l, r) && !cmp(r, l));
    }

	// filename: File which store data && index
	// index-file: File which store the information of filename
	//             including end of file, those erased blocks

	FILE* file;
private:
    struct node {
		// least key in the Node || Leaf
		Key key;
		off_t pos, prev, next;
		size_t num;
		int type; // 0 for node 1 for leaf
		node(off_t _pos, off_t _prev, off_t _next, int _type = 1) :
			pos(_pos), prev(_prev), next(_next), type(_type)
		{
			key = Key();
			num = 0;
		}
		node(off_t _pos = invalid_off, int _type = 1) :
			pos(_pos), type(_type)
		{
			key = Key();
			num = 0;
			prev = next = invalid_off;
		}
		bool operator==(const node& other) {
			return pos == other.pos;
		}
		//node(node &&other) : pos(other.pos), num(other.num), type(other.type), key(other.key), prev(other.prev), next(other.next) {}
	};

	struct Node_data {
		Key key;
		off_t son;

		Node_data() {}
		Node_data(Key _key, off_t _son) : key(_key), son(_son) {}
		Node_data& operator = (const Node_data& other) {
			if (this == &other) return *this;
			key = other.key;
			son = other.son;
		}
	};
	struct Leaf_data {
		Key key;
		value_t data;
		Leaf_data() {}
		Leaf_data(Key _key, value_t _data) : key(_key), data(_data) {}
		Leaf_data(Leaf_data& other) : key(other.key), data(other.data) {}
		//Leaf_data(Leaf_data &&other) : key(other.key), data(other.data) {}
	};

	inline void save_info() {
		fseek(file, 0, SEEK_SET);
		fwrite(&root, sizeof(off_t), 1, file);
		fwrite(&head, sizeof(off_t), 1, file);
		fwrite(&tail, sizeof(off_t), 1, file);
		fflush(file);
	}

	inline node read_node(off_t pos) {
		node temp_node(invalid_off, 1);
		fseek(file, pos, SEEK_SET);
		fread(&temp_node, sizeof(node), 1, file);
		//assert(temp_node.key >= 0 && temp_node.key <= 20000);
		return temp_node;
	}

	inline void save_node(const node& now) {
		fseek(file, now.pos, SEEK_SET);
		fwrite(&now, sizeof(node), 1, file);
		fflush(file);
	}

	inline void save_buf_Leaf(_buffer _buf, const node& now) {
		fseek(file, now.pos + sizeof(node), SEEK_SET);
		fwrite(_buf, 1, (sizeof(Key) + sizeof(value_t)) * now.num, file);
		fflush(file);
	}

	inline void save_buf_Node(_buffer _buf, const node & now) {
		fseek(file, now.pos + sizeof(node), SEEK_SET);
		fwrite(_buf, 1, (sizeof(Key) + sizeof(off_t)) * (now.num), file);
		fflush(file);
	}

	inline void load_Leaf_buf(_buffer _buf, const node & p) {
		fseek(file, p.pos + sizeof(node), SEEK_SET);
		fread(_buf, 1, (sizeof(Key) + sizeof(value_t)) * p.num, file);
	}

	inline void load_Node_buf(_buffer _buf, const node & p) {
		fseek(file, p.pos + sizeof(node), SEEK_SET);
		fread(_buf, 1, (sizeof(Key) + sizeof(off_t)) * (p.num), file);
	}

	// get_key in leaf
	inline Key* get_key_leaf(_buffer _buf, int pos) {
		return (Key*)(_buf + (sizeof(Key) + sizeof(value_t)) * pos);
	}

	inline Key* get_key_Node(_buffer _buf, int pos) {
		return (Key*)(_buf + (sizeof(Key) + sizeof(off_t)) * pos);
	}

	inline Leaf_data* get_Leaf_data(_buffer _buf, int pos) {
		return (Leaf_data*)(_buf + (sizeof(Key) + sizeof(value_t)) * pos);
	}

	inline Node_data* get_Node_data(_buffer _buf, int pos) {
		return (Node_data*)(_buf + (sizeof(Key) + sizeof(off_t)) * pos);
	}

	inline value_t* get_val_leaf(_buffer _buf, int pos) {
		return (value_t*)(_buf + (sizeof(Key) + sizeof(value_t)) * pos + sizeof(Key));
	}

	inline off_t * get_Node_son(_buffer _buf, int pos) {
		/*cout << sizeof(Node_data) << endl;
		cout << sizeof(Key) + sizeof(off_t) << endl;*/
		return (off_t*)(_buf + (sizeof(Key) + sizeof(off_t)) * pos + sizeof(Key));
	}

	/*--------------------------------------------------------------*
	 *																*
	 *				            insert								*
	 *																*
	 *--------------------------------------------------------------*/
#ifndef DEBUG1
	void buf_traverse_Node(_buffer _buf, const node & p) {
		cout << "traversing buf_Node " << p.pos << endl;
		Node_data node;
		for (int i = 0; i < p.num; ++i) {
			node = *get_Node_data(_buf, i);
			cout << node.key << " : " << node.son << endl;
		}
		cout << "end traversing" << "\n\n";
	}
#endif

	void insert_start(const Key & key, const value_t & val) {
		if (root == invalid_off) {
			node p(disk_manage.new_pos(), invalid_off, invalid_off, 1);
            assert((p.pos - 3 * sizeof(off_t)) % node_size == 0 && p.pos >= 3 * sizeof(off_t));
			buffer buf;
			buf_leaf_insert(buf, key, val, p);
			root = head = tail = p.pos;
			save_info();
			save_node(p);
			save_buf_Leaf(buf, p);
			return;
		}
		else {
			node rt = read_node(root);
			node result = insert_recursive(rt, key, val);
			if (result.pos != rt.pos) {
				buffer buf;
				node new_rt(disk_manage.new_pos(), invalid_off, invalid_off, 0);
				root = new_rt.pos;
				save_info();
				*get_key_Node(buf, 0) = rt.key;
				*get_Node_son(buf, 0) = rt.pos;
				*get_key_Node(buf, 1) = result.key;
				*get_Node_son(buf, 1) = result.pos;
				//*get_Node_data(buf, 0) = Node_data(rt.key, rt.pos);
				//*get_Node_data(buf, 1) = Node_data(result.key, result.pos);
				new_rt.num = 2;
				save_node(new_rt);
				save_buf_Node(buf, new_rt);

				//buf_traverse_Node(buf, new_rt);
				return;
			}
			else {
				save_node(rt);
			}
		}
	}

	node insert_recursive(node & now, const Key & key, const value_t & val) {
		node result(invalid_off, 1);
		if (now.type == 1) {
			buffer buf;
			load_Leaf_buf(buf, now);
			node result = buf_leaf_insert(buf, key, val, now);
			// already saved things in insert
			// including splited Leaf
			return result;
		}
		else {
			buffer buf;
			load_Node_buf(buf, now);
			int pos = findPos_Node(buf, key, now);
			if (pos == now.num) --pos;
			//Node_data *oldson_data = get_Node_data(buf, pos);
			off_t oldson_pos = *get_Node_son(buf, pos);
            assert((oldson_pos - 3 * sizeof(off_t)) % node_size == 0 && oldson_pos >= 3 * sizeof(off_t));
			node oldson = read_node(oldson_pos);
			result = insert_recursive(oldson, key, val);
			if (result.pos != oldson.pos) {
				// insert new son next to oldson
				//*get_Node_data(buf, pos) = Node_data(oldson.key, oldson.pos);
				*get_key_Node(buf, pos) = oldson.key;
				*get_Node_son(buf, pos) = oldson.pos;
				node newresult = buf_Node_insert(buf, result.key, result.pos, now, pos + 1);
                assert((newresult.pos - 3 * sizeof(off_t)) % node_size == 0 && newresult.pos >= 3 * sizeof(off_t));
				// already saved things in insert
				// including splited Node
				return newresult;
			}
			else {
				//*get_Node_data(buf, pos) = Node_data(result.key, result.pos);
				*get_key_Node(buf, pos) = result.key;
                assert((result.pos - 3 * sizeof(off_t)) % node_size == 0 && result.pos >= 3 * sizeof(off_t));
				*get_Node_son(buf, pos) = result.pos;
				now.key = *get_key_Node(buf, now.num - 1);
				save_node(now);
				save_buf_Node(buf, now);
				return now;
			}
		}
	}

	// binary_search of Leaf in buf
	size_t findPos_leaf(_buffer _buf, const Key & key, node & p) {
		int l = 0, r = p.num, m;
		Key* k = NULL;
		while (l < r) {
			m = (l + r) >> 1;
			k = get_key_leaf(_buf, m);
			if (cmp(*k, key)) l = m + 1;
			else r = m;
		}
		return r;
	}

	size_t findPos_Node(_buffer _buf, const Key & key, node & p) {
		int l = 0, r = p.num, m;
		Key* k;
		while (l < r) {
			m = (l + r) >> 1;
			k = get_key_Node(_buf, m);
			if (cmp(*k, key)) l = m + 1;
			else r = m;
		}
		return r;
	}

	node buf_leaf_insert(_buffer _buf, const Key & key, const value_t & val, node & p) {
		size_t pos = findPos_leaf(_buf, key, p);
		if (pos < p.num&& equal(key, *get_key_leaf(_buf, pos))) {
#ifdef DEBUG
			throw(sjtu::duplicated_Key());
#endif
			//return p
		}
		for (int i = p.num; i > pos; --i) {
			*get_key_leaf(_buf, i) = *get_key_leaf(_buf, i - 1);
			*get_val_leaf(_buf, i) = *get_val_leaf(_buf, i - 1);
			//*get_Leaf_data(_buf, i) = *get_Leaf_data(_buf, i - 1);
		}
		*get_key_leaf(_buf, pos) = key;
		*get_val_leaf(_buf, pos) = val;
		++p.num;
		p.key = *get_key_leaf(_buf, p.num - 1);
		if (p.num == Leaf_max) {
			node q = buf_Leaf_split(_buf, p);
			return q;
		}
		else {
			save_node(p);
			save_buf_Leaf(_buf, p);
			return p;
		}
	}

	node buf_Node_insert(_buffer _buf, const Key & key, const off_t & son, node & now, const size_t pos) {
		for (size_t i = now.num; i > pos; --i) {
			//*get_Node_data(_buf, i) = *get_Node_data(_buf, i - 1);
			*get_key_Node(_buf, i) = *get_key_Node(_buf, i - 1);
			*get_Node_son(_buf, i) = *get_Node_son(_buf, i - 1);
		}
		//*get_Node_data(_buf, pos) = Node_data(key, son);
		*get_key_Node(_buf, pos) = key;
		*get_Node_son(_buf, pos) = son;
		++now.num;
		now.key = *get_key_Node(_buf, now.num - 1);

		if (now.num == Node_max) {
			node new_Node = buf_Node_split(_buf, now);
			return new_Node;
		}
		else {
			save_node(now);
			save_buf_Node(_buf, now);
			return now;
		}
	}

	node buf_Node_split(_buffer _buf, node & now) {
		int mid = (now.num >> 1);
		node newNode(disk_manage.new_pos(), 0);

        assert((newNode.pos - 3 * sizeof(off_t)) % node_size == 0 && newNode.pos >= 3 * sizeof(off_t));

		newNode.num = now.num - mid;
		now.key = *get_key_Node(_buf, mid - 1);
		newNode.key = *get_key_Node(_buf, now.num - 1);
		now.num = mid;
		save_node(now);
		save_node(newNode);
		save_buf_Node(_buf, now);
		save_buf_Node(_buf + (sizeof(Key) + sizeof(off_t)) * now.num, newNode);
		return newNode;
	}

	node buf_Leaf_split(_buffer _buf, node & now) {
		size_t now_new_sz = now.num >> 1, new_Leaf_sz = now.num - now_new_sz;
		node new_Leaf(disk_manage.new_pos(), now.pos, now.next, 1);

        assert((new_Leaf.pos - 3 * sizeof(off_t)) % node_size == 0 && new_Leaf.pos >= 3 * sizeof(off_t));

		now.key = *get_key_leaf(_buf, now_new_sz - 1);
		new_Leaf.key = *get_key_leaf(_buf, now.num - 1);
		new_Leaf.num = new_Leaf_sz;
		if (new_Leaf.next == invalid_off) {
			tail = new_Leaf.pos;
			save_info();
		}
		else {
			node old_nxt = read_node(now.next);
			old_nxt.prev = new_Leaf.pos;
			save_node(old_nxt);
		}
		now.num = now_new_sz;
        now.next = new_Leaf.pos;
		save_node(now);
		save_node(new_Leaf);
		save_buf_Leaf(_buf, now);
		save_buf_Leaf(_buf + (sizeof(Key) + sizeof(value_t)) * now.num, new_Leaf);
		return new_Leaf;
	}

	/*--------------------------------------------------------------*
	*																*
	*				            erase 								*
	*																*
	*--------------------------------------------------------------*/
	// return -2 : didn't merge, borrowed from left
	// return 0  : didn't merge, borrowed from right
	// return -1 : merged with left
	// return 1  : merged with right
	sjtu::pair<int, Key> erase_recursive(node & now, const Key & key, off_t left, off_t right) {
		if (now.type) {
			buffer buf;
			load_Leaf_buf(buf, now);
			int pos = findPos_leaf(buf, key, now);
			buf_leaf_erase(buf, now, pos);

			if (now.num == 0 && now.pos == root) {
				root = head = tail = invalid_off;
				save_info();
				return sjtu::pair<int, Key>(0, Key());
			}

			save_node(now);
			save_buf_Leaf(buf, now);
			if (now.num >= (Leaf_max / 2) || now.pos == root) {
				return sjtu::pair<int, Key>(0, now.key);
			}
			else {
				if (left == invalid_off && right == invalid_off) return sjtu::pair<int, Key>(0, Key());
				if (left == invalid_off) {
					node R_leaf = read_node(right);
					return leaf_balance_right(buf, now, R_leaf);
				}
				else if (right == invalid_off) {
					node L_leaf = read_node(left);
					return leaf_balance_left(buf, now, L_leaf);
				}
				else {
					node R_leaf = read_node(right); node L_leaf = read_node(left);
					if (L_leaf.num <= R_leaf.num)
						return leaf_balance_right(buf, now, R_leaf);
					else
						return leaf_balance_left(buf, now, L_leaf);
				}
			}
		}
		else {
			buffer buf;
			load_Node_buf(buf, now);
			int pos = findPos_Node(buf, key, now);
			//Node_data *oldson_data = get_Node_data(buf, pos);
			off_t oldson_pos = *get_Node_son(buf, pos);
			node oldson = read_node(oldson_pos);
			off_t leftson = (pos == 0) ? invalid_off : *get_Node_son(buf, pos - 1),
				rightson = (pos == now.num - 1) ? invalid_off : *get_Node_son(buf, pos + 1);
			sjtu::pair<int, Key> result = erase_recursive(oldson, key, leftson, rightson);

			//if (now.num >= (Node_max / 2) || now.pos == root) return sjtu::pair<int, Key> (0, Key());
			if (result.first == 0) {
				//*get_Node_data(buf, pos) = Node_data(result.second, oldson.pos);
				*get_key_Node(buf, pos) = result.second;
				*get_Node_son(buf, pos) = oldson.pos;
			}
			if (result.first == -2) {
				*get_key_Node(buf, pos - 1) = (result.second);
			}
			if (result.first == 1) {
				*get_key_Node(buf, pos) = result.second;
				for (int i = pos + 1; i < now.num - 1; ++i) {
					//*get_Node_data(buf, i) = *get_Node_data(buf, i + 1);
					*get_key_Node(buf, i) = *get_key_Node(buf, i + 1);
					*get_Node_son(buf, i) = *get_Node_son(buf, i + 1);
				}
				/*fseek(file, now.pos + sizeof(node), SEEK_SET);
				fwrite(buf, 1, (sizeof(Key) + sizeof(off_t)) * (pos - 1), file);
				if (pos < now.num - 1)
					fwrite(buf + (sizeof(Key) + sizeof(off_t)) * (pos + 1), 1, (sizeof(Key) + sizeof(off_t)) * (now.num - pos - 1), file);*/
				--now.num;
			}
			if (result.first == -1) {
				*get_key_Node(buf, pos - 1) = result.second;
				for (int i = pos; i < now.num; ++i) {
					//*get_Node_data(buf, i) = *get_Node_data(buf, i + 1);
					*get_key_Node(buf, i) = *get_key_Node(buf, i + 1);
					*get_Node_son(buf, i) = *get_Node_son(buf, i + 1);
				}
				--now.num;
			}

			// for root
			if (now.pos == root && now.num == 1) {
				root = *get_Node_son(buf, 0);
				save_info();
				return sjtu::pair<int, Key>(0, Key());
			}

			now.key = *get_key_Node(buf, now.num - 1);
			save_node(now);
			save_buf_Node(buf, now);
			if (now.num >= (Node_max / 2) || now.pos == root) {
				return sjtu::pair<int, Key>(0, now.key);
			}
			else {
				if (left == invalid_off) {
					node R_Node = read_node(right);
					return Node_balance_right(buf, now, R_Node);
				}
				else if (right == invalid_off) {
					node L_Node = read_node(left);
					return leaf_balance_left(buf, now, L_Node);
				}
				else {
					node R_Node = read_node(right); node L_Node = read_node(left);
					if (L_Node.num <= R_Node.num)
						return leaf_balance_right(buf, now, R_Node);
					else
						return leaf_balance_left(buf, now, L_Node);
				}
			}
		}
	}

	sjtu::pair<int, Key> Node_balance_right(_buffer _buf, node & now, node & right) {
		if (right.num >= (Node_max / 2) + 1) {
			buffer right_buf;
			load_Node_buf(right_buf, right);
			//*get_Node_data(_buf, now.num) = *get_Node_data(right_buf, 0);
			*get_key_Node(_buf, now.num) = *get_key_Node(right_buf, 0);
			*get_Node_son(_buf, now.num) = *get_Node_son(right_buf, 0);
			now.key = *get_key_Node(right_buf, 0);
			--right.num;
			++now.num;
			save_node(now);
			save_buf_Node(_buf, now);
			save_node(right);
			save_buf_Node(right_buf + (sizeof(Key) + sizeof(off_t)), right);
			return sjtu::pair<int, Key>(0, now.key);
		}
		else {
			//buffer right_buf;
			//load_Leaf_buf(right_buf, right);
			merge_Node(now, right);
			save_node(now);
			return sjtu::pair<int, Key>(1, now.key);
		}
	}

	sjtu::pair<int, Key> Node_balance_left(_buffer _buf, node & now, node & left) {
		if (left.num >= (Node_max / 2) + 1) {
			buffer left_buf;
			load_Node_buf(left_buf, left);
			fseek(file, now.pos, SEEK_SET);
			++now.num;
			fwrite(&now, 1, sizeof(node), file);
			fwrite(left_buf + (sizeof(Key) + sizeof(off_t)) * (left.num - 1), 1, (sizeof(Key) + sizeof(off_t)), file);
			fwrite(_buf, 1, (sizeof(Key) + sizeof(off_t)) * now.num, file);
			fflush(file);
			left.key = *get_key_Node(left_buf, left.num - 2);
			--left.num;
			save_node(left);
			return sjtu::pair<int, Key>(-2, left.key);
		}
		else {
			merge_Node(left, now);
			save_node(left);
			return sjtu::pair<int, Key>(-1, left.key);
		}
	}

	void merge_Node(node & now, node & right) {
		buffer right_buf;
		load_Node_buf(right_buf, right);
		disk_manage.delete_pos(right.pos);
		fseek(file, now.pos + sizeof(node) + (sizeof(Key) + sizeof(off_t)) * now.num, SEEK_SET);
		fwrite(right_buf, 1, (sizeof(Key) + sizeof(off_t)) * right.num, file);
		fflush(file);
		now.num += right.num;
		now.key = right.key;
	}

	void buf_leaf_erase(_buffer _buf, node & now, size_t pos) {
		for (int i = pos; i < now.num - 1; ++i) {
			*get_key_leaf(_buf, i) = *get_key_leaf(_buf, i + 1);
			*get_val_leaf(_buf, i) = *get_val_leaf(_buf, i + 1);
		}
		--now.num;
		if (pos == now.num) now.key = *get_key_leaf(_buf, now.num - 1);
	}

	sjtu::pair<int, Key> leaf_balance_right(_buffer _buf, node & now, node & right) {
		if (right.num >= (Leaf_max / 2) + 1) {
			buffer right_buf;
			load_Leaf_buf(right_buf, right);
			//*get_Leaf_data(_buf, now.num) = *get_Leaf_data(right_buf, 0);
			*get_key_leaf(_buf, now.num) = *get_key_leaf(right_buf, 0);
			*get_val_leaf(_buf, now.num) = *get_val_leaf(right_buf, 0);
			now.key = *get_key_leaf(right_buf, 0);
			--right.num;
			++now.num;
			save_node(now);
			save_buf_Leaf(_buf, now);
			save_node(right);
			save_buf_Leaf(right_buf + (sizeof(Key) + sizeof(value_t)), right);
			return sjtu::pair<int, Key>(0, now.key);
		}
		else {
			//buffer right_buf;
			//load_Leaf_buf(right_buf, right);
			merge_leaf(now, right);
			save_node(now);
			return sjtu::pair<int, Key>(1, now.key);
		}
	}

	sjtu::pair<int, Key> leaf_balance_left(_buffer _buf, node & now, node & left) {
		if (left.num >= (Leaf_max / 2) + 1) {
			buffer left_buf;
			load_Leaf_buf(left_buf, left);
			fseek(file, now.pos, SEEK_SET);
			++now.num;
			fwrite(&now, 1, sizeof(node), file);
			fwrite(left_buf + (sizeof(Key) + sizeof(value_t)) * (left.num - 1), 1, (sizeof(Key) + sizeof(value_t)), file);
			fwrite(_buf, 1, (sizeof(Key) + sizeof(value_t)) * now.num, file);
			fflush(file);
			left.key = *get_key_leaf(left_buf, left.num - 2);
			--left.num;
			save_node(left);
			return sjtu::pair<int, Key>(-2, left.key);
		}
		else {
			merge_leaf(left, now);
			save_node(left);
			return sjtu::pair<int, Key>(-1, left.key);
		}
	}

	void merge_leaf(node & now, node & right) {
		now.next = right.next;
		if (right.next != invalid_off) {
			node nxt = read_node(right.next);
			nxt.prev = now.pos;
			save_node(nxt);
		}
		else {
			tail = now.pos;
			save_info();
		}
		buffer right_buf;
		load_Leaf_buf(right_buf, right);
		disk_manage.delete_pos(right.pos);
		fseek(file, now.pos + sizeof(node) + (sizeof(Key) + sizeof(value_t)) * now.num, SEEK_SET);
		fwrite(right_buf, 1, (sizeof(Key) + sizeof(value_t)) * right.num, file);
		fflush(file);
		now.num += right.num;
		now.key = right.key;
	}

	sjtu::pair<node, int> get_key_pos(const Key & key) {
		if (root == invalid_off) throw(sjtu::nonexistent_Key());
		node p = read_node(root);
		buffer buf; int pos;
		do {
			if (p.type == 0) {
				load_Node_buf(buf, p);
				pos = findPos_Node(buf, key, p);
				if (pos == p.num) throw(sjtu::nonexistent_Key());
				p = read_node(*get_Node_son(buf, pos));
			}
			else {
				load_Leaf_buf(buf, p);
				pos = findPos_leaf(buf, key, p);
				if (pos > p.num || !equal(key, *get_key_leaf(buf, pos))) throw(sjtu::nonexistent_Key());
				else return sjtu::pair<node, int>(p, pos);
			}
		} while (1);
	}

	sjtu::pair<node, int> get_bigger_key_pos(const Key &key){
        if (root == invalid_off) throw(sjtu::nonexistent_Key());
        node p = read_node(root);
        buffer buf; int pos;
        do {
            if (p.type == 0) {
                load_Node_buf(buf, p);
                pos = findPos_Node(buf, key, p);
                if (pos == p.num) throw(sjtu::nonexistent_Key());
                p = read_node(*get_Node_son(buf, pos));
            }
            else {
                load_Leaf_buf(buf, p);
                pos = findPos_leaf(buf, key, p);
                if (pos >= p.num) throw(sjtu::nonexistent_Key());
                else return sjtu::pair<node, int>(p, pos);
            }
        } while (1);
	}

public:
	BP_Tree(const char* _filename, const char* _index_file) :
#ifdef RELEASE
		Node_max((node_size - sizeof(node)) / (sizeof(Key) + sizeof(off_t))),
		Leaf_max((node_size - sizeof(node)) / (sizeof(Key) + sizeof(value_t)))
#endif
#ifdef DEBUG
		Node_max(5), Leaf_max(5)
#endif
	{
		filename = new char[strlen(_filename) + 1];
		strcpy(filename, _filename);
		index_file = new char[strlen(_index_file) + 1];
		strcpy(index_file, _index_file);
		disk_manage.initialize(index_file);

		file = fopen(filename, "rb+");
		if (!file) initialize();
		else {
			fseek(file, 0, SEEK_SET);
			fread(&root, sizeof(off_t), 1, file);
			fread(&head, sizeof(off_t), 1, file);
			fread(&tail, sizeof(off_t), 1, file);
		}
	}

	~BP_Tree() {
		save_info();
		if (file) fclose(file);
		delete filename;
		delete index_file;
	}

	void initialize() {
		file = fopen(filename, "wb+");
		head = root = tail = invalid_off;
		save_info();
	}

	void insert(const Key & key, const value_t & val) {
		insert_start(key, val);
	}

	void erase(const Key & key) {
		node rt = read_node(root);
		erase_recursive(rt, key, invalid_off, invalid_off);
	}

	int count(const Key & key) {
		if (root == invalid_off) return 0;
		node p = read_node(root);
		buffer buf; int pos;
		do {
			if (p.type == 0) {
				load_Node_buf(buf, p);
				pos = findPos_Node(buf, key, p);
				if (pos == p.num) return 0;
				p = read_node(*get_Node_son(buf, pos));
			}
			else {
				load_Leaf_buf(buf, p);
				pos = findPos_leaf(buf, key, p);
				if (pos >= p.num || !equal(key, *get_key_leaf(buf, pos))) return 0;
				else return 1;
			}
		} while (1);
	}

	value_t zhoumo(const Key & key) {
		if (root == invalid_off) {
		    //std:cout <<"throw" << std::endl;
		    return value_t();
		}
		node p = read_node(root);
		buffer buf; int pos;
		do {
			if (p.type == 0) {
				load_Node_buf(buf, p);
				pos = findPos_Node(buf, key, p);
				if (pos == p.num) return value_t();
				p = read_node(*get_Node_son(buf, pos));
			}
			else {
				load_Leaf_buf(buf, p);
				pos = findPos_leaf(buf, key, p);
				if (pos >= p.num || !equal(key, *get_key_leaf(buf, pos))) return value_t();
				else return *get_val_leaf(buf, pos);
			}
		} while (1);
	}

	void get_range(sjtu::vector<value_t> & vec, const Key & lkey, std::function<bool(const Key&, const Key&)> func) {
		sjtu::pair<node, int> beg = get_bigger_key_pos(lkey);
		node p = beg.first; int pos = beg.second;
		buffer buf;
		load_Leaf_buf(buf, p);
		do {
            Key k = *get_key_leaf(buf, pos);
            if (!func(lkey, k)) return;
			vec.push_back(*get_val_leaf(buf,pos));
            ++pos;
			if (pos == p.num) {
			    if (p.next == invalid_off) return;
			    p = read_node(p.next);
				pos = 0;
				load_Leaf_buf(buf, p);
			}
		} while (1);
	}

    void get_range(sjtu::vector < sjtu::pair < Key, value_t> > & vec, const Key & lkey, std::function<bool(const Key&, const Key&)> func) {
        sjtu::pair<node, int> beg = get_bigger_key_pos(lkey);
        node p = beg.first; int pos = beg.second;
        buffer buf;
        load_Leaf_buf(buf, p);
        do {
            Key k = *get_key_leaf(buf, pos);
            if (!func(k, lkey)) return;
            vec.push_back(sjtu::pair<Key,value_t>(k, *get_val_leaf(buf, pos)));
            ++pos;
            if (pos == p.num) {
                if (p.next == invalid_off) return;
                p = read_node(p.next);
                pos = 0;
                load_Leaf_buf(buf, p);
            }
        } while (1);
    }

	int modify(const Key & key, const value_t & val) {
		sjtu::pair<node, int> temp = get_key_pos(key);
		buffer buf;
		load_Leaf_buf(buf, temp.first);
		*get_val_leaf(buf, temp.second) = val;
		save_buf_Leaf(buf, temp.first);
		return 1;
	}

	void range_search(const Key & lkey, const Key & rkey, ostream & os) {
		sjtu::pair<node, int> beg = get_key_pos(lkey);
		node p = beg.first; int pos = beg.second;
		buffer buf;
		load_Leaf_buf(buf, p);
		do {
			os << *get_val_leaf(buf, pos) << endl;
			if (equal(*get_key_leaf(buf, pos), rkey)) return;
			++pos;
			if (pos == p.num) {
				p = read_node(p.next);
				pos = 0;
				load_Leaf_buf(buf, p);
			}
		} while (1);
	}

	void print_info() {
		cout << Node_max << " " << Leaf_max << endl;
	}


	class iterator {
		friend BP_Tree;
	private:
		node now;
		size_t pos;
		BP_Tree* tree;
	public:
		iterator() {
			now = node(invalid_off, 1);
			pos = 0;
			tree = NULL;
		}
		iterator(node& _now, int _pos, BP_Tree* _tree) : now(_now), pos(_pos), tree(_tree) {}
		iterator(const iterator& other) {
			now = other.now;
			pos = other.pos;
			tree = other.tree;
		}
		iterator(Key& key, BP_Tree* _tree) {
			tree = _tree;
			sjtu::pair<node, int> temp = tree->get_key_pos(key);
			now = temp.first, pos = temp.second;
		}
		iterator& operator =(const iterator& other) {
			now = other.now;
			pos = other.pos;
			tree = other.tree;
			return *this;
		}
		iterator operator++(int) {
			iterator temp(*this);
			++(*this);
			return temp;
		}
		iterator& operator++() {
			++pos;
			if (pos == now.num) {
				if (now.next == invalid_off) {
					now.pos = invalid_off;
					pos = -1;
					return *this;
				}
				now = tree->read_node(now.next);
				pos = 0;
			}
			return *this;
		}
		iterator& operator--() {
			// it's end
			if (now.pos == invalid_off) {
				now = tree->read_node(tree->tail);
				pos = now.num - 1;
				return *this;
			}
			if (pos != 0) {
				--pos;
				return *this;
			}
			else {
				if (now.prev == invalid_off)
					throw(sjtu::runtime_error());
				now = tree->read_node(now.prev);
				pos = now.num - 1;
				return *this;
			}

		}
		bool operator == (const iterator & other) {
			if (now.pos == invalid_off) {
				return other.now.pos == invalid_off;
			}
			return (now == other.now && pos == other.pos);
		}
		bool operator != (const iterator & other) {
			return !(*this == other);
		}

		value_t& operator*() const {
			buffer buf;
			tree->load_Leaf_buf(buf, now);
			return *tree->get_val_leaf(buf, pos);
		}

	};

	bool empty() {
		return root == invalid_off;
	}

	iterator begin() {
		if (empty()) return end();
		node hd = read_node(head);
		return iterator(hd, 0, this);
	}

	iterator end() {
		return iterator();
	}

	iterator iterator_from_key(Key & key) {
		return iterator(key, this);
	}


#ifndef DEBUG1

	void traverse_recursive(node & now) {
		buffer buf;
		if (now.type) {
			load_Leaf_buf(buf, now);
			cout << "Leaf\n" << " having " << now.num << " datas \n";
			for (int i = 0; i < now.num; ++i) {
				Leaf_data leaf;
				leaf = *get_Leaf_data(buf, i);
				cout << "(" << leaf.key << " , " << leaf.data << ") ";
				cout << "\n";
			}
		}
		else {
			load_Node_buf(buf, now);
			cout << "Node\n" << "  having" << now.num << " sons\n\n";
			cout << "keys:\n";
			for (int i = 0; i < now.num; ++i)
				cout << *get_key_Node(buf, i) << ' ';
			cout << "\nsons:\n";
			for (int i = 0; i < now.num; ++i)
				cout << *get_Node_son(buf, i) << " ";
			cout << "\n\n";
			for (int i = 0; i < now.num; ++i) {
				node son = read_node(*get_Node_son(buf, i));
				traverse_recursive(son);
			}
			cout << "\n\n";
		}
	}
	void traverse_tree() {
		if (root == invalid_off) return;
		node rt = read_node(root);
		traverse_recursive(rt);
	}
#endif

#ifndef DEBUG1
	void traverse_Leaf() {
		node p = read_node(head);
		buffer buf;
		Leaf_data leaf;
		while (1) {
			load_Leaf_buf(buf, p);
			for (int i = 0; i < p.num; ++i) {
				leaf = *get_Leaf_data(buf, i);
				cout << leaf.key << " " << leaf.data << " ";
			}
			cout << endl;
			if (p.next == invalid_off) break;
			else p = read_node(p.next);
		}
		cout << endl;
	}
#endif

};

#endif
