#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.hpp"
#include <iostream>
#include <cstddef>
#include<cmath>
#include "exceptions.hpp"

namespace sjtu {

	template<class T>
	class deque {
	private:
		struct node
		{
			T **data;
			int size;
			node*previous, *next;
			node(int s, node*p = NULL, node*n = NULL)
			{
				data = new T*[s];
				size = 0;
				previous = p;
				next = n;
			}
			~node() { delete[]data; }
		};
		node*frontNode, *backNode;
		int nodeSize;
		int length;

		void selfClear()
		{
			node*p = frontNode, *q;
			while (p != NULL)
			{
				q = p->next;
				for (int i = 0; i < p->size; ++i)
				{
					delete p->data[i];
					p->data[i] = NULL;
				}
				delete p;
				p = q;
			}
			backNode = frontNode = NULL;
		}
		void find(const size_t k, node* &resultNode, int &resultPos)const		//找到第k个元素，并得到所在结点和结点中的位置
		{
			node *tmp;
			size_t counter = k + 1;				//到元素k的长度是k+1
			if (k >= length)	//超出范围
			{
				resultPos = -1;
				resultNode = NULL;
				return;
			}

			tmp = frontNode;
			while (counter > tmp->size)
			{
				counter -= tmp->size;
				tmp = tmp->next;
			}
			resultNode = tmp;
			resultPos = counter - 1;
		}

	public:
		class const_iterator;
		class iterator {
			friend deque;
			friend const_iterator;
		private:
			node * currentNode;
			int nodePos;	//在结点中的位置
		public:
			iterator() :currentNode(NULL), nodePos(-1) {}
			iterator(node* n, int pos)
			{
				currentNode = n;
				nodePos = pos;
			}
			iterator &operator=(const iterator &other)
			{
				currentNode = other.currentNode;
				nodePos = other.nodePos;
				return *this;
			}

			iterator operator+(const int &n) const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (n < 0)return (*this) - (-n);
				iterator tmp(*this);


				/*
				//在最后一个结点 如果越界 则一直在原来的结点不要动，nodePos可以表示越界了多少
				if (currentNode->next = NULL)
				{
				tmp.nodePos += n;
				return tmp;
				}
				*/
				int counter = n;
				while (tmp.currentNode->size - tmp.nodePos <= counter)
				{
					counter -= tmp.currentNode->size;
					if (tmp.currentNode->next != NULL)tmp.currentNode = tmp.currentNode->next;
					else
					{
						counter += tmp.currentNode->size;
						tmp.nodePos += counter;
						return tmp;
					}
				}
				tmp.nodePos += counter;
				return tmp;
			}
			iterator operator-(const int &n) const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (n < 0)return (*this) + (-n);
				iterator tmp(*this);

				/*
				if (currentNode->previous==NULL)					//假如越界,则一直在原来的结点不要动，nodePos可以表示越界了多少
				{
				tmp.nodePos -= n;
				return tmp;
				}
				*/
				int counter = n;
				while (tmp.nodePos < counter)
				{
					counter -= (tmp.nodePos + 1);
					if (tmp.currentNode->previous != NULL)
					{
						tmp.currentNode = tmp.currentNode->previous;
						tmp.nodePos = tmp.currentNode->size - 1;
					}
					else
					{
						//tmp.nodePos = 0;
						counter += (tmp.nodePos + 1);
						tmp.nodePos -= counter;
						return tmp;
						//break;
					}
				}
				tmp.nodePos -= counter;
				return tmp;
			}

			int operator-(const iterator &rhs) const
			{
				int counter = 0;
				node*p = currentNode, *q = rhs.currentNode;
				if (p == q)nodePos - rhs.nodePos;

				//assume p is before q
				while (p != q && p != NULL)
				{
					if (p != NULL)counter += p->size;
					p = p->next;
				}
				if (p != NULL)return -(counter + rhs.nodePos - nodePos);

				counter = 0;
				p = currentNode;
				q = rhs.currentNode;
				while (q != p && q != NULL)
				{
					if (q != NULL)counter += q->size;
					q = q->next;
				}
				if (q != NULL)return counter + nodePos - rhs.nodePos;

				throw invalid_iterator();
			}
			iterator operator+=(const int &n) { *this = (*this) + n; return *this; }
			iterator operator-=(const int &n) { *this = (*this) - n; return *this; }

			iterator operator++(int)
			{
				iterator tmp(*this);
				*this = (*this) + 1;
				return tmp;

			}
			iterator& operator++() { *this = (*this) + 1; return *this; }
			iterator operator--(int)
			{
				iterator tmp(*this);
				*this = (*this) - 1;
				return tmp;
			}
			iterator& operator--() { *this = (*this) - 1; return *this; }

			T& operator*() const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (nodePos >= currentNode->size)throw invalid_iterator();
				if (nodePos < 0)throw invalid_iterator();
				return *(currentNode->data[nodePos]);
			}
			T* operator->() const noexcept
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (nodePos >= currentNode->size)throw invalid_iterator();
				if (nodePos < 0)throw invalid_iterator();
				return (currentNode->data[nodePos]);
			}

			bool operator==(const iterator &rhs) const { return (currentNode == rhs.currentNode && nodePos == rhs.nodePos); }
			bool operator==(const const_iterator &rhs) const { return (currentNode == rhs.currentNode && nodePos == rhs.nodePos); }
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
		};
		class const_iterator {
			friend deque;
			friend iterator;
		private:
			node * currentNode;
			int nodePos;	//在结点中的位置
			int position;  //在deque中的位置
		public:
			const_iterator() :currentNode(NULL), nodePos(-1), position(-1) {}
			const_iterator(node* n, int pos, int p)
			{
				currentNode = n;
				nodePos = pos;
				position = p;
			}
			const_iterator &operator=(const iterator &other)
			{
				if (this == &other)return *this;
				currentNode = other.currentNode;
				nodePos = other.nodePos;
				position = other.position;
				return *this;
			}

			const_iterator(const const_iterator &other)
			{
				currentNode = other.currentNode;
				nodePos = other.nodePos;
				position = other.position;
			}
			const_iterator(const iterator &other)
			{
				currentNode = other.currentNode;
				nodePos = other.nodePos;
				position = other.position;
			}

			const_iterator operator+(const int &n) const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (n < 0)return (*this) - (-n);
				const_iterator tmp(*this);

				int counter = n + 1;
				while (tmp.currentNode->size - tmp.nodePos < counter)
				{
					counter -= tmp.currentNode->size;
					if (tmp.currentNode->next != NULL)tmp.currentNode = tmp.currentNode->next;
					else
					{
						counter += tmp.currentNode->size;
						tmp.nodePos += --counter;
						return tmp;
					}
				}
				tmp.nodePos += --counter;
				return tmp;
			}
			const_iterator operator-(const int &n) const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (n < 0)return (*this) + (-n);
				const_iterator tmp(*this);

				int counter = n;
				while (tmp.nodePos < counter)
				{
					counter -= (tmp.nodePos + 1);
					if (tmp.currentNode->previous != NULL)
					{
						tmp.currentNode = tmp.currentNode->previous;
						tmp.nodePos = tmp.currentNode->size - 1;
					}
					else
					{
						//tmp.nodePos = 0;
						counter += (tmp.nodePos + 1);
						tmp.nodePos -= counter;
						return tmp;
						//break;
					}
				}
				tmp.nodePos -= counter;
				return tmp;
			}

			int operator-(const const_iterator &rhs) const
			{
				// return the distance between two iterator,
				// if these two iterators points to different vectors, throw invaild_iterator.
				int counter = 0;
				node*p = currentNode, *q = rhs.currentNode;
				if (p == q)nodePos - rhs.nodePos;

				//assume p is before q
				while (p != q && p != NULL)
				{
					if (p != NULL)counter += p->size;
					p = p->next;
				}
				if (p != NULL)return -(counter + rhs.nodePos - nodePos);

				counter = 0;
				p = currentNode;
				q = rhs.currentNode;
				while (q != p && q != NULL)
				{
					if (q != NULL)counter += q->size;
					q = q->next;
				}
				if (q != NULL)return counter + nodePos - rhs.nodePos;

				throw invalid_iterator();
			}
			const_iterator operator+=(const int &n) { *this = (*this) + n; return *this; }
			const_iterator operator-=(const int &n) { *this = (*this) - n; return *this; }
			const_iterator operator++(int)
			{
				const_iterator tmp(*this);
				*this = (*this) + 1;
				return tmp;
			}
			const_iterator& operator++() { *this = (*this) + 1; return *this; }
			const_iterator operator--(int)
			{
				const_iterator tmp(*this);
				*this = (*this) - 1;
				return tmp;
			}
			const_iterator& operator--() { *this = (*this) - 1; return *this; }

			T& operator*() const
			{
				if (currentNode == NULL)throw invalid_iterator();
				if (nodePos >= currentNode->size)throw invalid_iterator();
				if (nodePos<0)throw invalid_iterator();
				return *(currentNode->data[nodePos]);
			}
			T* operator->() const noexcept
			{
				if (nodePos >= currentNode->size)throw invalid_iterator();
				if (currentNode == NULL)throw invalid_iterator();
				if (nodePos >= currentNode->size)throw invalid_iterator();
				if (nodePos<0)throw invalid_iterator();
				return (currentNode->data[nodePos]);
			}

			bool operator==(const iterator &rhs) const { return (currentNode == rhs.currentNode&&nodePos == rhs.nodePos); }
			bool operator==(const const_iterator &rhs) const { return (currentNode == rhs.currentNode&&nodePos == rhs.nodePos); }
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }

			//self test
			T get() { return *(currentNode->data[nodePos]); }

		};

		deque()
		{	
			nodeSize = 548;
			frontNode = backNode = new node(nodeSize, NULL, NULL);
			length = 0;
		}
		deque(const deque &other)
		{
			nodeSize = other.nodeSize;
			length = other.length;
			frontNode = new node(nodeSize);
			frontNode->size = other.frontNode->size;
			if (length == 0) { backNode = frontNode; return; }

			node*p = other.frontNode, *q = frontNode;
			int pos = 0;
			while (p != NULL)
			{
				q->data[pos] = new T(*(p->data[pos]));
				++pos;
				if (pos == p->size)
				{
					p = p->next;
					if (p != NULL)
					{
						q = q->next = new node(nodeSize, q, NULL);
						q->size = p->size;
						pos = 0;
					}
				}
			}
			backNode = q;
		}
		~deque() { selfClear(); /*std::cout << "\ndeconstructed successfully!\n";*/ }
		deque &operator=(const deque &other)
		{
			if (this == &other)return *this;
			selfClear();
			nodeSize = other.nodeSize;
			length = other.length;
			frontNode = new node(nodeSize);
			frontNode->size = other.frontNode->size;
			if (length == 0) { backNode = frontNode; return *this; }

			node*p = other.frontNode, *q = frontNode;
			int pos = 0;
			while (p != NULL)
			{
				q->data[pos] = new T(*(p->data[pos]));
				++pos;
				if (pos == p->size)
				{
					p = p->next;
					if (p != NULL)
					{
						q = q->next = new node(nodeSize, q, NULL);
						q->size = p->size;
						pos = 0;
					}
				}
			}
			backNode = q;
			//std::cout << "operator= done!\n";
			return *this;
		}

		T & at(const size_t &pos)
		{
			if (pos >= length)throw index_out_of_bound();
			node *p;
			int position;
			find(pos, p, position);
			return *(p->data[position]);
		}
		const T & at(const size_t &pos) const
		{
			if (pos >= length)throw index_out_of_bound();
			node *p;
			int position;
			find(pos, p, position);
			return *(p->data[position]);
		}
		T & operator[](const size_t &pos)
		{
			if (pos >= length)throw index_out_of_bound();
			node *p;
			int position;
			find(pos, p, position);
			return *(p->data[position]);
		}
		const T & operator[](const size_t &pos) const
		{
			if (pos >= length)throw index_out_of_bound();
			node *p;
			int position;
			find(pos, p, position);
			return *(p->data[position]);
		}

		const T & front() const
		{
			if (length == 0)throw container_is_empty();
			return *(frontNode->data[0]);
		}
		const T & back() const
		{
			if (length == 0)throw container_is_empty();
			return *(backNode->data[backNode->size - 1]);
		}

		iterator begin()
		{
			iterator itr;
			itr.currentNode = frontNode;
			itr.nodePos = 0;
			return itr;
		}
		const_iterator cbegin() const
		{
			const_iterator itr;
			itr.currentNode = frontNode;
			itr.nodePos = 0;
			return itr;
		}
		iterator end()
		{
			iterator itr;
			itr.currentNode = backNode;
			itr.nodePos = backNode->size;
			return itr;
		}
		const_iterator cend() const
		{
			const_iterator itr;
			itr.currentNode = backNode;
			itr.nodePos = backNode->size;
			return itr;
		}
		bool empty() const { return length == 0; }
		size_t size() const { return static_cast<size_t>(length); }
		void clear()
		{
			selfClear();
			frontNode = backNode = new node(nodeSize);
			length = 0;
		}

		iterator insert(iterator pos, const T &value)
		{
			if (pos.currentNode == NULL)throw invalid_iterator();
			if (pos.currentNode->size < pos.nodePos)throw invalid_iterator();
			if (pos.nodePos<0)throw invalid_iterator();
			node*tmp = pos.currentNode;
			while (tmp != NULL)
				if (tmp->next != NULL)
					tmp = tmp->next;
				else break;
				if (tmp != backNode)throw invalid_iterator();

				node*p = pos.currentNode;
				int nodePos = pos.nodePos;

				//remain: insert end
				if (p == backNode && nodePos == nodeSize)
				{
					p->next = new node(nodeSize, p, NULL);
					backNode = p->next;
					backNode->data[0] = new T(value);
					backNode->size = 1;
					length++;
					iterator itr(backNode, 0);
					return itr;
				}

				//当前结点满
				if (p->size == nodeSize)
				{
					p->next = new node(nodeSize, p, p->next);
					if (p != backNode && p->next->next != NULL)p->next->next->previous = p->next;
					if (backNode == p)backNode = p->next;

					for (int i = nodePos; i < p->size; ++i)
					{
						p->next->data[i - nodePos] = p->data[i];
						p->data[i] = NULL;
					}
					p->data[nodePos] = new T(value);
					p->next->size = p->size - nodePos;
					p->size = nodePos + 1;
				}
				else//当前结点未满
				{
					for (int i = p->size; i > nodePos; --i)p->data[i] = p->data[i - 1];
					p->data[nodePos] = new T(value);
					p->size++;
				}

				length++;

				//reconstruct？
				iterator itr(p, nodePos);
				return itr;

		}
		iterator erase(iterator pos)
		{
			if (length == 0)throw invalid_iterator();
			if (pos.currentNode == NULL)throw invalid_iterator();
			if (pos.currentNode->size <= pos.nodePos)throw invalid_iterator();
			if (pos.nodePos<0)throw invalid_iterator();
			node*tmp = pos.currentNode;
			//check if belong to current deque
			while (tmp != NULL)
				if (tmp->next != NULL)
					tmp = tmp->next;
				else break;
				if (tmp != backNode)throw invalid_iterator();

				node*p = pos.currentNode;
				int nodePos = pos.nodePos;

				delete p->data[nodePos];
				for (int i = nodePos; i < p->size - 1; ++i)
					p->data[i] = p->data[i + 1];
				//p->data[p->size - 1] = NULL;
				p->size--;
				length--;

				iterator itr;
				//当前结点未空
				if (p->size > 0)
				{
					//删到了最后一个位置
					if (pos.nodePos == p->size)
					{
						//恰好又在最后一个结点
						if (p == backNode)
						{
							itr.currentNode = backNode;
							itr.nodePos = backNode->size;
							return itr;
						}
						//但不在最后一个结点
						else
						{
							itr.currentNode = p->next;
							itr.nodePos = 0;
							return itr;
						}
					}
					itr.nodePos = pos.nodePos;
					itr.currentNode = p;
					return itr;
				}

				if (p->size == 0)
				{
					//维持原状：只剩一个结点
					if (frontNode == backNode)
					{
						itr.nodePos = 0;
						itr.currentNode = frontNode;
						return itr;
					}

					//需要删除结点 //分类：
					//剩多个结点   头结点删空   尾结点删空  中间结点删空
					if (p == frontNode)
					{
						frontNode = frontNode->next;
						frontNode->previous = NULL;
						delete p;
						//itr.currentNode = frontNode;
						//itr.nodePos = 0;
						return begin();
					}

					if (p == backNode)
					{
						backNode = backNode->previous;
						backNode->next = NULL;
						delete p;
						//itr.currentNode = backNode;
						//itr.nodePos = backNode->size;
						return end();
					}

					itr.currentNode = p->next;
					itr.nodePos = 0;
					p->next->previous = p->previous;
					p->previous->next = p->next;
					delete p;
					return itr;
				}
		}

		void push_back(const T &value)
		{
			if (backNode->size == nodeSize) //最后一个结点满了
			{
				backNode->next = new node(nodeSize, backNode, NULL);
				backNode = backNode->next;

				//backPos = 0;
				backNode->data[0] = new T(value);
				backNode->size = 1;
				length++;
				return;
			}
			backNode->data[backNode->size++] = new T(value);
			//backNode->size++;
			length++;

			//reconstruct？
		}
		void pop_back()
		{
			if (length == 0)
				throw container_is_empty();

			length--;

			delete backNode->data[backNode->size - 1];
			backNode->data[backNode->size - 1] = NULL;

			backNode->size--;
			if (backNode->size == 0 && backNode != frontNode)//保留一个结点
			{
				node*p = backNode;
				backNode = backNode->previous;			//尾结点前移
				backNode->next = NULL;
				delete p;								//删除最后空出来的结点
														//backPos = backNode->size - 1;
			}

			//reconstruct
		}
		void push_front(const T &value)
		{
			length++;
			if (frontNode->size == nodeSize) //第一个结点满了
			{
				frontNode = new node(nodeSize, NULL, frontNode);
				frontNode->next->previous = frontNode;
				frontNode->data[0] = new T(value);
				frontNode->size = 1;
				return;
			}
			for (int i = frontNode->size; i > 0; --i)frontNode->data[i] = frontNode->data[i - 1];
			frontNode->data[0] = new T(value);
			frontNode->size++;

			//reconstruct？
		}
		void pop_front()
		{
			if (length == 0)throw container_is_empty();
			delete frontNode->data[0];
			length--;
			for (int i = 0; i < frontNode->size - 1; ++i)frontNode->data[i] = frontNode->data[i + 1];
			frontNode->data[frontNode->size - 1] = NULL;

			frontNode->size--;
			if (frontNode->size == 0 && frontNode != backNode)
			{
				node*p = frontNode;
				frontNode = frontNode->next;
				frontNode->previous = NULL;
				delete p;
			}

			//reconstruct？
		}

	};

}

#endif

