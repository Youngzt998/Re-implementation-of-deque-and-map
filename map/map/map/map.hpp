/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

	//using AVL tree + list
	template<
		class Key,
		class T,
		class Compare = std::less<Key>
	> class map {

	private:
		Compare cmp;
		struct node
		{
			pair<const Key, T> *data;
			node*left, *right;
			node*parent;
			int height;
			node*pre, *next;
			node(const pair<const Key, T> &p, node*L = NULL, node*R = NULL, node*pa = NULL, int h = 1, node*pr = NULL, node*ne = NULL)
			{
				data = new pair<const Key, T>(p);
				left = L;
				right = R;
				parent = pa;
				height = h;
				pre = pr;
				next = ne;
			}
			node() :data(NULL), left(NULL), right(NULL), parent(NULL), height(0), pre(NULL), next(NULL) {}
			~node() { delete data; }
		};
		node*root;
		node*GND;		//GND: replace NULL (ground)			//only GND's left and right is NULL			//GND->parent:max element	//接地
		int size_;
		int max(int a, int b) { return (a > b) ? a : b; }
		void selfClear(node*t)
		{
			if (t == GND) return;
			selfClear(t->left);
			selfClear(t->right);
			delete t;
		}
		void LL(node*&t)
		{
			node*t1 = t->left;
			//change sons
			t->left = t1->right;
			t1->right = t;
			//change height
			t->height = max(t->left->height, t->right->height) + 1;
			t1->height = max(t1->left->height, t->height) + 1;
			//change parent
			t1->parent = t->parent;
			t->parent = t1;
			if (t->left != GND)
			{
				t->left->parent = t;
			}
			//change current root
			t = t1;
		}
		void RR(node*&t)
		{
			node*t1 = t->right;
			//change sons
			t->right = t1->left;
			t1->left = t;
			//change height
			t->height = max(t->left->height, t->right->height) + 1;
			t1->height = max(t->height, t1->right->height) + 1;
			//change parent
			t1->parent = t->parent;
			t->parent = t1;
			if (t->right != GND)
			{
				t->right->parent = t;
			}
			//change current root
			t = t1;
		}
		void LR(node*&t)
		{
			RR(t->left);
			LL(t);
		}
		void RL(node*&t)
		{
			LL(t->right);
			RR(t);
		}
		void copy(node*&self, node*other, node*GND, node* otherGND)		//for every node,create its left node and right node according to "other"
		{
			if (other == otherGND)		//meet a leaf-node
			{
				self = GND;
				return;
			}

			self = new node(*(other->data), GND, GND, NULL, other->height);

			copy(self->left, other->left, GND, otherGND);
			copy(self->right, other->right, GND, otherGND);
			if (self->left != GND)self->left->parent = self;
			if (self->right != GND)self->right->parent = self;
		}

	public:
		typedef pair<const Key, T> value_type;
		class const_iterator;
		class iterator {
			friend map;
			Compare cmp;
		private:
			node * cur;
			node*gnd;
			node*root;
		public:
			iterator() { cur = NULL; gnd = NULL; root = NULL; }
			iterator(const iterator &other) { cur = other.cur; gnd = other.gnd; root = other.root; }
			iterator(node* c, node*g, node*r) { cur = c; gnd = g; root = r; }

			iterator operator++(int)
			{
				if (cur == gnd)throw index_out_of_bound();
				iterator tmp(*this);
				cur = cur->next;
				return tmp;
			}
			iterator & operator++()
			{
				if (cur == gnd)throw index_out_of_bound();
				cur = cur->next;
				return *this;
			}
			iterator operator--(int)
			{
				if (root == gnd)throw index_out_of_bound();
				if (root->pre == gnd)throw index_out_of_bound();
				iterator tmp(*this);
				cur = cur->pre;
				return tmp;
			}
			iterator & operator--()
			{
				if (root == gnd)throw index_out_of_bound();
				if (root->pre == gnd)throw index_out_of_bound();
				cur = cur->pre;
				return *this;
			}

			bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
			bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
			bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
			bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }

			value_type & operator*() const { return *(cur->data); }
			value_type* operator->() const noexcept { return cur->data; }
		};
		class const_iterator {
			friend map;
			Compare cmp;
		private:
			node * cur;
			node*gnd;
			node*root;
		public:
			const_iterator() { cur = NULL; gnd = NULL; root = NULL; }
			const_iterator(const const_iterator &other) { cur = other.cur; gnd = other.gnd; root = other.root; }
			const_iterator(const iterator &other) { cur = other.cur; gnd = other.gnd; root = other.root; }
			const_iterator(node* c, node*g, node*r) { cur = c; gnd = g; root = r; }

			const_iterator operator++(int)
			{
				if (cur == gnd)throw index_out_of_bound();
				const_iterator tmp(*this);
				cur = cur->next;
				return tmp;
			}
			const_iterator & operator++()
			{
				if (cur == gnd)throw index_out_of_bound();
				cur = cur->next;
				return *this;
			}
			const_iterator operator--(int)
			{
				if (root == gnd)throw index_out_of_bound();
				if (root->pre == gnd)throw index_out_of_bound();
				const_iterator tmp(*this);
				cur = cur->pre;
				return tmp;
			}
			const_iterator & operator--()
			{
				if (root == gnd)throw index_out_of_bound();
				if (root->pre == gnd)throw index_out_of_bound();
				cur = cur->pre;
				return *this;
			}

			bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
			bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
			bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
			bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }

			value_type & operator*() const { return *(cur->data); }
			value_type* operator->() const noexcept { return cur->data; }
		};

		map()
		{
			GND = new node;
			root = GND;				//replace "root=NULL;"
			GND->pre = root;
			GND->next = root;
			size_ = 0;
		}
		map(const map &other)
		{
			GND = new node;
			root = GND;
			GND->pre = root;
			GND->next = root;
			if (other.root == other.GND)return;
			//construct tree , O(n)
			copy(root, other.root, GND, other.GND);

			//construct sorted list, O(nlogn)
			node*t = root, *last;
			while (t->left != GND)t = t->left;
			last = t;
			GND->next = last;
			last->pre = GND;
			while (t != GND)
			{
				//go to next node
				if (t->right != GND)		//not a leaf, find min in cur->right
				{
					t = t->right;
					while (t->left != GND)t = t->left;
				}
				else {								//a leaf
					while (t->parent != NULL && cmp(t->parent->data->first, t->data->first))t = t->parent;	//go up left to the edge
					if (t->parent == NULL)t = GND;		//*this is end()
					else t = t->parent;					//go up right one step
				}
				last->next = t;
				t->pre = last;
				last = t;
			}
			GND->parent = GND->pre;

			size_ = other.size_;
		}
		map & operator=(const map &other)
		{
			if (this == &other)return *this;

			if (root != GND)selfClear(root);

			root = GND;
			GND->pre = root;
			GND->next = root;
			if (other.root == other.GND)return *this;
			copy(root, other.root, GND, other.GND);

			//建表,O(nlogn)
			node*t = root, *last;
			while (t->left != GND)t = t->left;
			last = t;
			GND->next = last;
			last->pre = GND;
			while (t != GND)
			{
				//go to next node
				if (t->right != GND)		//not a leaf, find min in cur->right
				{
					t = t->right;
					while (t->left != GND)t = t->left;
				}
				else {								//a leaf
					while (t->parent != NULL && cmp(t->parent->data->first, t->data->first))t = t->parent;	//go up left to the edge
					if (t->parent == NULL)t = GND;		//*this is end()
					else t = t->parent;					//go up right one step
				}

				last->next = t;
				t->pre = last;
				last = t;
			}
			GND->parent = GND->pre;

			size_ = other.size_;
			return*this;
		}
		~map() { selfClear(root); delete GND; }

		/**
		* TODO
		* access specified element with bounds checking
		* Returns a reference to the mapped value of the element with key equivalent to key.
		* If no such element exists, an exception of type `index_out_of_bound'
		*/
		T & at(const Key &key)
		{
			iterator tmp = find(key);
			if (tmp.cur == GND) throw index_out_of_bound();
			return tmp.cur->data->second;
		}
		const T & at(const Key &key) const
		{
			const_iterator tmp = find(key);
			if (tmp.cur == GND) throw index_out_of_bound();
			return tmp.cur->data->second;
		}

		/**
		* TODO
		* access specified element
		* Returns a reference to the value that is mapped to a key equivalent to key,
		*   performing an insertion if such key does not already exist.
		*/
		T & operator[](const Key &key)
		{
			iterator tmp = find(key);
			T nothing;

			if (tmp.cur != GND) return tmp.cur->data->second;
			else return insert(pair<Key, T>(key, nothing)).first.cur->data->second;
		}
		/**
		* behave like at() throw index_out_of_bound if such key does not exist.
		*/
		const T & operator[](const Key &key) const
		{
			const_iterator tmp = find(key);
			if (tmp.cur == GND) throw index_out_of_bound();
			return tmp.cur->data->second;
		}

		iterator begin()
		{
			return iterator(GND->next, GND, root);
		}
		const_iterator cbegin() const
		{
			return const_iterator(GND->next, GND, root);
		}

		/**
		* return a iterator to the end
		* in fact, it returns past-the-end.
		*/
		iterator end() { return iterator(GND, GND, root); }
		const_iterator cend() const { return const_iterator(GND, GND, root); }

		bool empty() const { return root == GND; }
		size_t size() const { return size_; }
		void clear()
		{
			selfClear(root);
			root = GND;
			GND->parent = NULL;
			GND->pre = root;
			GND->next = root;
			size_ = 0;
		}

		/**
		* insert an element.
		* return a pair, the first of the pair is
		*   the iterator to the new element (or the element that prevented the insertion),
		*   the second one is true if insert successfully, or false.
		*/
	private:
		bool insert(const value_type &value, node*&t, node*&newNode, node*last)
		{
			bool successful = true;
			if (t == GND)		//空树上
			{
				if (t == root)
				{
					root = new node(value, GND, GND, NULL, 1);
					t = root;
					GND->parent = root;
				}
				else t = new node(value, GND, GND, last, 1);
				newNode = t;
			}
			else if (!cmp(value.first, t->data->first) && !cmp(t->data->first, value.first))		//already exists, return,
			{
				newNode = t;
				return false;
			}
			else if (cmp(value.first, t->data->first))		//去插左子树
			{
				successful = insert(value, t->left, newNode, t);
				if (successful && t->left->height - t->right->height == 2)
					if (cmp(value.first, t->left->data->first))
						LL(t);
					else LR(t);
			}
			else if (cmp(t->data->first, value.first))
			{
				successful = insert(value, t->right, newNode, t);
				//if (successful && t->right->parent == NULL)t->right->parent = t;
				if (successful && t->right->height - t->left->height == 2)
					if (cmp(t->right->data->first, value.first))
						RR(t);
					else RL(t);
			}
			if (successful)	t->height = max(t->left->height, t->right->height) + 1;
			return successful;
		}
	public:
		pair<iterator, bool> insert(const value_type &value)
		{
			//bool insert_max = false;
			node*newNode;
			bool successful = insert(value, root, newNode, NULL);

			if (successful)
			{
				size_++;

				node*last = newNode, *next = newNode;

				if (next->right != GND)		//not a leaf, find min in cur->right
				{
					next = next->right;
					while (next->left != GND)next = next->left;
				}
				else {								//a leaf
					while (next->parent != NULL && cmp(next->parent->data->first, next->data->first))next = next->parent;	//go up left to the edge
					if (next->parent == NULL)next = GND;		//*this is end()
					else next = next->parent;					//go up right one step
				}

				if (last->left != GND)		//not a leaf, find min in cur->right
				{
					last = last->left;
					while (last->right != GND)last = last->right;
				}
				else {								//a leaf
					while (last->parent != NULL && cmp(last->data->first, last->parent->data->first))last = last->parent;
					if (last->parent == NULL)last = GND;		//*this is begin()
					else last = last->parent;
				}
				last->next = newNode;
				next->pre = newNode;
				newNode->pre = last;
				newNode->next = next;
			}

			return pair<iterator, bool>(iterator(newNode, GND, root), successful);
		}

		/**
		* erase the element at pos.
		*
		* throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		*/
	private:
		bool adjust(node*&t, bool sub)
		{
			if (sub)
			{
				if (t->left->height - t->right->height == 1) { return true; }
				if (t->right->height == t->left->height) { t->height--; return false; }
				if (t->left->right->height > t->left->left->height)
				{
					LR(t);
					return false;
				}
				LL(t);
				if (t->right->height == t->left->height)return false;
				else return true;
			}
			else
			{
				if (t->right->height - t->left->height == 1) { return true; }
				if (t->right->height == t->left->height) { t->height--; return false; }
				if (t->right->left->height > t->right->right->height)
				{
					RL(t);
					return false;
				}
				RR(t);
				if (t->right->height == t->left->height)return false;
				else return true;
			}
		}
		bool erase(const Key &x, node*&t, node*last)
		{
			if (t == GND)return true;
			if (!(cmp(x, t->data->first) || cmp(t->data->first, x)))
			{
				if (t->left == GND || t->right == GND)
				{
					node*old = t;
					t = (t->left != GND) ? t->left : t->right;
					if (t != GND)t->parent = last;

					old->pre->next = old->next;
					old->next->pre = old->pre;

					delete old;
					return false;
				}
				else
				{
					//find replacement
					node*tmp = t->next;
					node*t1 = t;
					//exchange node
					node*L = t->left, *R = t->right, *P = last;
					int H = t->height;

					if (t1->right != tmp)
					{
						L->parent = tmp;
						R->parent = tmp;
						t = tmp;

						tmp->parent->left = t1;

						t1->left = GND;
						t1->right = tmp->right;
						t1->parent = tmp->parent;
						t1->height = tmp->height;

						tmp->left = L;
						tmp->right = R;
						tmp->parent = P;
						tmp->height = H;
						
					}
					else {
						
						t1->left->parent = tmp;
						L = t1->left; R = t1; P = last; H = t1->height;
						L->parent = tmp;

						t1->left = GND;
						t1->right = tmp->right;
						t1->parent = tmp;
						t1->height = tmp->height;

						tmp->right = R;
						tmp->left = L;
						tmp->parent = P;
						tmp->height = H;
						t = tmp;
					}
					
					if (erase(x, t->right, t))return true;
					return adjust(t, 1);
				}
			}

			if (cmp(x, t->data->first))
			{
				if (erase(x, t->left, t))return true;
				else return adjust(t, 0);
			}
			else
			{
				if (erase(x, t->right, t))return true;
				else return adjust(t, 1);
			}
		}
	public:
		void erase(iterator pos)
		{
			if (pos.cur == GND)throw index_out_of_bound();
			if (pos.gnd != GND)throw invalid_iterator();//not in this map
			erase(pos.cur->data->first, root, NULL);
			size_--;
			/*
			//非递归删除
			bool inLeft;
			bool ok;
			node*tmp = pos.cur;

			if (tmp->right != GND)
			{
			tmp = tmp->right;
			while (tmp->left != GND)tmp = tmp->left;
			value_type *tmpT = pos.cur->data;
			pos.cur->data = new value_type(*(tmp->data));
			delete tmpT;
			}
			node*t = tmp->parent;
			if (cmp(t->data->first, tmp->data->first))t->right = GND;
			else t->left = GND;
			delete tmp;
			//t->height

			while (t != root && !ok)
			{

			}
			*/
		}

		/**
		* Returns the number of elements with key
		*   that compares equivalent to the specified argument,
		*   which is either 1 or 0
		*     since this container does not allow duplicates.
		* The default method of check the equivalence is !(a < b || b > a)
		*/
		size_t count(const Key &key) const
		{
			node*t = root;
			while (t != GND && (cmp(t->data->first, key) || cmp(key, t->data->first)))
			{
				if (cmp(key, t->data->first)) t = t->left;
				else t = t->right;
			}
			if (t == GND)return 0;
			else return 1;
		}

		/**
		* Finds an element with key equivalent to key.
		* key value of the element to search for.
		* Iterator to an element with key equivalent to key.
		*   If no such element is found, past-the-end (see end()) iterator is returned.
		*/
		iterator find(const Key &key)
		{
			node*t = root;
			while (t != GND)
				if (cmp(key, t->data->first))
					t = t->left;
				else if (cmp(t->data->first, key))
					t = t->right;
				else break;

				iterator tmp(t, GND, root);
				return tmp;
		}
		const_iterator find(const Key &key) const
		{
			node*t = root;
			while (t != GND)
				if (cmp(key, t->data->first))
					t = t->left;
				else if (cmp(t->data->first, key))
					t = t->right;
				else break;

				return const_iterator(t, GND, root);
		}
		//typedef map<Key, T> mmp;//:)
	};

}

#endif
