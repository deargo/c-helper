#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <assert.h>

namespace CppHelper
{

template<typename Type>
class CLinkList
{
private:
    //����˫�����ǰ�����ͺ�̽ڵ�
    struct NodeBase
    {
        NodeBase* next;
        NodeBase* prev;
        NodeBase() : next(nullptr), prev(nullptr) {}
    };

    //���������ݳ�Ա�������ݲ����������������
    struct Node : public NodeBase
    {
        Type data;
    };

private:

    //˫�������ǰ������
    class Iterator;

    //const˫�������
    class ConstIterator
    {
        friend class CLinkList;

        const NodeBase* node = nullptr;
        const NodeBase* guard = nullptr;

        ConstIterator(const NodeBase *pGuard, const NodeBase *pNode) : guard(pGuard),node(pNode) {}

    public:
        typedef ptrdiff_t                       difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef Type                            value_type;
        typedef const Type*                     pointer;
        typedef const Type&                     reference;

    public:
        ConstIterator(const Iterator& it) : guard(it.guard),node(it.node) {}

        reference operator*() const { assert(guard!=node); return static_cast<const Node*>(node)->data; }

        pointer operator->() const { assert(guard!=node); return &static_cast<const Node*>(node)->data; }

        ConstIterator& operator++() { node = node->next; return *this; }
        ConstIterator operator++(int) { const ConstIterator temp = *this; node = node->next; return temp; }

        ConstIterator& operator--() { node = node->prev; return *this; }
        ConstIterator operator--(int) { const ConstIterator temp = *this; node = node->prev; return temp; }

        ConstIterator operator+(int distance) const
        {
            ConstIterator temp = *this;
            while(temp.node && distance--) temp.node = temp.node->next;
            return temp;
        }

        ConstIterator operator-(int distance) const
        {
            ConstIterator temp = *this;
            while(temp.node->prev && distance--) temp.node = temp.node->prev;
            return temp;
        }

        ConstIterator& operator+=(int distance) { while(node && distance--) node = node->next; return *this; }
        ConstIterator& operator-=(int distance) { while(node->prev && distance--) node = node->prev;return *this;}

        bool operator==(const ConstIterator& other) const { return node == other.node; }
        bool operator!=(const ConstIterator& other) const { return node != other.node; }

        bool valid() const { return guard!=node;}
    };

    //˫�������
    class Iterator
    {
        friend class CLinkList;

        NodeBase* node = nullptr;
        const NodeBase* guard = nullptr;

        Iterator(NodeBase* pGuard, NodeBase* pNode) : guard(pGuard),node(pNode){}

    public:
        typedef ptrdiff_t                        difference_type;
        typedef std::bidirectional_iterator_tag  iterator_category;
        typedef Type                             value_type;
        typedef Type*                            pointer;
        typedef const Type*                      const_pointer;
        typedef Type&                            reference;
        typedef const Type&                      const_reference;

    public:

        reference operator*() { assert(guard!=node); return static_cast<Node*>(node)->data; }
        const_reference operator*() const  { assert(guard!=node); return static_cast<Node*>(node)->data; }

        pointer operator->() { assert(guard!=node); return &static_cast<Node*>(node)->data; }
        const_pointer operator->() const { assert(guard!=node); return &static_cast<Node*>(node)->data; }

        Iterator& operator++() { node = node->next; return *this; }
        Iterator operator++(int) { Iterator temp = *this; node = node->next; return temp; }

        Iterator& operator--() { node = node->prev; return *this; }
        Iterator operator--(int) { Iterator temp = *this; node = node->prev; return temp; }

        Iterator operator+(int distance)
        {
            Iterator temp = *this;
            while(temp.node && distance--) temp.node = temp.node->next;
            return temp;
        }
        ConstIterator operator+(int distance) const
        {
            Iterator temp = *this;
            while(temp.node && distance--) temp.node = temp.node->next;
            return ConstIterator(temp.node);
        }

        Iterator operator-(unsigned int distance)
        {
            Iterator temp = *this;
            while(temp.node->prev && distance--) temp.node = temp.node->prev;
            return temp;
        }
        ConstIterator operator-(unsigned int distance) const
        {
            Iterator temp = *this;
            while(temp.node->prev && distance--) temp.node = temp.node->prev;
            return ConstIterator(temp.node);
        }

        Iterator& operator+=(unsigned int distance) { while(node && distance--) node = node->next; return *this; }
        Iterator& operator-=(unsigned int distance) { while(node->prev && distance--) node = node->prev; return *this; }

        bool operator==(const Iterator& other) const { return node == other.node; }
        bool operator!=(const Iterator& other) const { return node != other.node; }

        bool valid() const{ return guard!=node;}
    };

public:
    typedef Type                                  value_type;
    typedef Type*                                 pointer;
    typedef const Type*                           const_pointer;
    typedef Type&                                 reference;
    typedef const Type&                           const_reference;
    typedef CLinkList::Iterator                   iterator;
    typedef CLinkList::ConstIterator              const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;

public:
    CLinkList() { _initialize();}
    explicit CLinkList(size_type size, const value_type& value = value_type()) { _initialize();insert(begin(), size, value); }
    CLinkList(const CLinkList& other) { _initialize(); insert(begin(), other.begin(), other.end());}
    template<typename InputIterator>
    CLinkList(InputIterator first, InputIterator last)   { _initialize(); insert(begin(), first, last); }
    explicit CLinkList(const std::set<Type>& set) { _initialize(); insert(begin(),set.begin(), set.end()); }
    explicit CLinkList(const std::vector<Type>& vec) { _initialize(); insert(begin(),vec.begin(), vec.end()); }
    explicit CLinkList(const std::list<Type>& list){ _initialize(); insert(begin(),list.begin(), list.end());}
    virtual ~CLinkList() { _finalize(); }

    bool operator==(const CLinkList& other) const { return size()==other.size() && std::equal(begin(),end(),other.begin());}
    bool operator!=(const CLinkList& other) const { return size()!=other.size() || false ==std::equal(begin(),end(),other.begin());}

    value_type& operator [](size_type index){ iterator it = begin(); while(index--) ++it; return *it; }
    const value_type& operator [](size_type index) const { const_iterator it = begin(); while(index--) ++it; return *it; }

    CLinkList& operator=(const CLinkList& other)
    {
        if (other != *this)
        {
            removeAll();
            iterator it = begin();
            const_iterator itOther = other.begin();

            for (; it != end() && itOther != other.end(); ++it, ++itOther)
            {
                *it = *itOther;
            }

            if (itOther == other.end())
            {
                erase(it, end());
            }
            else
            {
                insert(end(), itOther, other.end());
            }
        }
        return *this;
    }

    CLinkList operator +(Type&& value){ CLinkList temp(*this); temp.push_back(std::forward<Type>(value)); return temp; }
    CLinkList operator +(const CLinkList& other){ CLinkList temp(*this); temp.push_back(other); return temp; }

    CLinkList& operator +=(Type&& value){ push_back(std::forward<Type>(value)); return *this; }
    CLinkList& operator +=(const CLinkList& other){ push_back(other); return *this; }

    CLinkList& operator<<(Type&& value){ push_back(std::forward<Type>(value)); return *this; }
    CLinkList& operator<<(const CLinkList& other){ push_back(other); return *this; }
    CLinkList& operator<<(const std::set<Type>& set){ push_back(set); return *this; }
    CLinkList& operator<<(const std::vector<Type>& vec){ push_back(vec); return *this; }
    CLinkList& operator<<(const std::list<Type>& list){ push_back(list); return *this; }

public:
    void append(Type&& value){ push_back(std::forward<Type>(value)); }
    void append(const CLinkList& other){ push_back(other); }
    void append(const std::set<Type>& set){ push_back(set); }
    void append(const std::vector<Type>& vec){ push_back(vec); }
    void append(const std::list<Type>& list){ push_back(list); }

    void assign(int size, const value_type& value)
    {
        iterator it = begin();
        for (; it != end() && size > 0; ++it, --size)
        {
            *it = value;
        }

        if (size > 0)
        {
            while (size --) { insert(end(), value);}
        }
        else
        {
            erase(it, end());
        }
    }
    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        iterator it = begin();
        for (; it != end() && first != last; ++it, ++first)
        {
            *it = *first;
        }

        if (first == last)
        {
            erase(it, end());
        }
        else
        {
            insert(end(), first, last);
        }
    }

    const Type& at(size_type index) const{ return this->operator[](index);  }

    reference back() { iterator temp = end(); --temp; return *temp; }
    const_reference back() const { const_iterator temp = end(); --temp; return *temp; }

    iterator begin() { return iterator(usedguard, usedguard->next); }
    const_iterator begin() const { return const_iterator(usedguard,usedguard->next);  }

    //ɾ������Ԫ�أ�����ձ����ڴ�
    void clear()
    {
        removeAll();
        NodeBase *pCurrNode = allocated;
        while (nullptr != pCurrNode)
        {
            //������������ڴ�
            NodeBase *temp = pCurrNode;
            pCurrNode = pCurrNode->next;
            delete temp;
        }
        alloc_size = 0;
        allocated = nullptr;

        _initialize();
    }

    bool contains(Type&& value) const { return  std::find(begin(), end(),std::forward<Type>(value)) != end(); }
    template <class CompareFunction>  //��Ҫ�������� bool (*compareFunction)(Type&& v1, Type&& v2) �Ķ�Ԫν�ʺ�����v1Ϊelement-value��v2Ϊinput-value
    bool contains(Type&& value,CompareFunction compareFunction) const
    {
        return  std::find_if(begin(), end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)))
                != end();
    }

    size_type count(Type&& value) const { return std::count(begin(), end(),std::forward<Type>(value)); }
    template <class CompareFunction>  //��Ҫ�������� bool (*compareFunction)(Type&& v1, Type&& v2) �Ķ�Ԫν�ʺ�����v1Ϊelement-value��v2Ϊinput-value
    size_type count(Type&& value,CompareFunction compareFunction) const
    {
        return  std::count_if(begin(), end(),
                              std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    bool empty() const { return usedguard->next == usedguard; }

    iterator end() { return iterator(usedguard, usedguard); }
    const_iterator end() const  { return const_iterator(usedguard,usedguard); }

    //ɾ��Ԫ�أ���������뱣���ڴ棬����ɾ��Ԫ�صĺ��������
    iterator erase(iterator pos)
    {
        if(pos == end())
        {
            return pos;
        }
        NodeBase* pPosNode = pos.node;
        iterator it(usedguard, pPosNode->next);

        //�Ƴ���ǰ���
        pPosNode->prev->next = pPosNode->next;
        pPosNode->next->prev = pPosNode->prev;

        //���뱣������
        _del_node(pos.node);

        return it;
    }
    //ɾ��Ԫ�أ���������뱣���ڴ棬�������һ��ɾ��Ԫ�صĺ��������
    iterator erase(iterator first, iterator last) { while (first != last) { first = erase(first); } return first; }

    iterator find(Type&& value) { return  std::find(begin(), end(),std::forward<Type>(value)); }
    const_iterator find(Type&& value) const { return  std::find(begin(), end(),std::forward<Type>(value)); }
    template <class CompareFunction>  //��Ҫ�������� bool (*compareFunction)(Type&& v1, Type&& v2) �Ķ�Ԫν�ʺ�����v1Ϊelement-value��v2Ϊinput-value
    iterator find(Type&& value,CompareFunction compareFunction)
    {
        return  std::find_if(begin(), end(), std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }
    template <class CompareFunction>  //��Ҫ�������� bool (*compareFunction)(Type&& v1, Type&& v2) �Ķ�Ԫν�ʺ�����v1Ϊelement-value��v2Ϊinput-value
    const_iterator find(Type&& value,CompareFunction compareFunction ) const
    {
        return  std::find_if(begin(), end(), std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    template <class Function>
    Function for_each(Function func){ return std::for_each(begin(), end(),func); }

    reference front()  { return *begin(); }
    const_reference front() const { return *begin(); }

    int index(Type&& value) const {
        for (int idx = 0; idx <(int)size();++idx){
            if (this->operator[](idx)==std::forward<Type>(value)) return idx;
        }
        return -1;
    }
    template <class CompareFunction>  //��Ҫ�������� bool (*compareFunction)(Type&& v1, Type&& v2) �Ķ�Ԫν�ʺ�����v1Ϊelement-value��v2Ϊinput-value
    int index(Type&& value,CompareFunction compareFunction) const {
        for (int idx = 0; idx <(int)size();++idx){
            if (compareFunction(this->operator[](idx),std::forward<Type>(value))) return idx;
        }
        return -1;
    }

    //��ǰ����ǰ�����ֵ
    iterator insert(iterator pos, const value_type& value)
    {
        Node* pAddNode = _get_node();
        pAddNode->data = value;
        iterator itAdd(usedguard, pAddNode);

        //��ǰ����ǰ����ӽ�㣬������ѭ���б��Ͳ����ж�tail��
        NodeBase* pPosNode = pos.node;
        pAddNode->next = pPosNode;
        pAddNode->prev = pPosNode->prev;
        pPosNode->prev->next = pAddNode;
        pPosNode->prev = pAddNode;

        return itAdd;
    }
    void insert(iterator pos, size_type size, const value_type& value) { while(size --) { insert(pos, value);} }
    template<typename InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last) { while (first != last) { insert(pos, *first); ++first; }}

    size_type max_size() const { return size_type(-1); }

    void merge(CLinkList& target) { merge(target,[](value_type v1,value_type v2){ return v1<v2;}); }
    template<typename StrictWeakOrdering>
    void merge(CLinkList& target, StrictWeakOrdering comp)
    {
        iterator it = begin();
        iterator itEnd = end();
        iterator itTarget = target.begin();
        iterator itTargetEnd = target.end();

        while (it != itEnd && itTarget != itTargetEnd)
        {
            if (comp(*itTarget,*it))
            {
                iterator itTargetNext = itTarget;
                _transfer(it, itTarget, ++itTargetNext);
                itTarget = itTargetNext;
            }
            else
            {
                ++it;
            }
        }

        if (itTarget != itTargetEnd)
        {
            _transfer(itEnd, itTarget, itTargetEnd);
        }
    }

    void pop_back() { erase(end()-1); }
    void pop_front() { erase(begin()); }

    void push_back(value_type&& value) { insert(end(), std::forward<value_type>(value)); }
    void push_back(const CLinkList& other){  insert(end(),other.begin(),other.end());}
    void push_back(const std::set<Type>& set){ insert(end(),set.begin(),set.end()); }
    void push_back(const std::vector<Type>& vec){ insert(end(),vec.begin(),vec.end()); }
    void push_back(const std::list<Type>& list){ insert(end(),list.begin(),list.end()); }

    void push_front(value_type&& value) { insert(begin(), std::forward<value_type>(value)); }
    void push_front(const CLinkList& other){  insert(begin(),other.begin(),other.end());}
    void push_front(const std::set<Type>& set){ insert(begin(),set.begin(),set.end()); }
    void push_front(const std::vector<Type>& vec){ insert(begin(),vec.begin(),vec.end()); }
    void push_front(const std::list<Type>& list){ insert(begin(),list.begin(),list.end()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    //�Ƴ�Ԫ�أ���������뱣���ڴ�
    void remove(value_type&& value) { this->remove_if([&](const value_type& v){return v==std::forward<value_type>(value);}); }
    //�Ƴ�Ԫ�أ���������뱣���ڴ�
    template<typename Predicate>
    void remove_if(Predicate pred)
    {
        for(auto it = begin(); it != end();)
        {
            if (pred(*it))
            {
                it = erase(it);
            }else
            {
                ++it;
            }
        }
    }
    //�Ƴ�����Ԫ�أ���������뱣���ڴ�
    void removeAll() { for (iterator it = begin() ; it != end(); ) { it = erase(it); } }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    void resize(size_type size, value_type&& value = value_type())
    {
        size_type len = 0;
        iterator it = begin();
        iterator itEnd = end();
        for (; it != itEnd && len < size; ++it, ++len)
        {
            ;//empty operater
        }

        if (len == size)
        {
            erase(it, itEnd);
        }
        else
        {
            insert(itEnd, size - len, std::forward<value_type>(value));
        }
    }

    void reverse()
    {
        if (usedguard->next == usedguard || usedguard->next->next == usedguard)
        {
            return;
        }

        NodeBase *pNode = usedguard;
        do
        {
            NodeBase *temp = pNode->next;
            pNode->next = pNode->prev;
            pNode->prev = temp;

            pNode = pNode->next;
        } while (pNode != usedguard);
    }

    size_type size() const { return std::distance(begin(), end()); }

    void sort() { this->sort([](value_type v1,value_type v2){ return v1<v2;}); }
    template<typename StrictWeakOrdering>
    void sort(StrictWeakOrdering comp)
    {
        if (usedguard->next == usedguard || usedguard->next->next == usedguard)
        {
            return;
        }

        CLinkList listCarry;
        CLinkList listCounter[64];
        int nFill = 0;

        while (!empty())
        {
            listCarry.splice(listCarry.begin(), *this,begin());
            int i = 0;
            while (i < nFill && !listCounter[i].empty())
            {
                listCounter[i].merge(listCarry, comp);
                listCarry.swap(listCounter[i++]);
            }

            listCarry.swap(listCounter[i]);
            if (i == nFill)
            {
                ++nFill;
            }
        }

        for (int i = 1; i < nFill; ++i)
        {
            listCounter[i].merge(listCounter[i - 1], comp);
        }
        swap(listCounter[nFill - 1]);
    }

    void splice(iterator pos, CLinkList& target) { if (!target.empty()) { _transfer(pos, target.begin(), target.end());  } }
    void splice(iterator pos, CLinkList& , iterator targetPos)
    {
        iterator it = targetPos;
        ++it;
        if (pos == targetPos || pos == it)
        {
            return;
        }
        _transfer(pos, targetPos, it);
    }
    void splice(iterator pos, CLinkList& ,iterator first, iterator last) { if (first != last) { _transfer(pos, first, last); } }

    void swap(CLinkList& listTarget)
    {
        NodeBase *temp = usedguard;
        usedguard = listTarget.usedguard;
        listTarget.usedguard = temp;
    }

    std::list<value_type> toStdList() const { return std::list<value_type>(begin(),end());}
    std::vector<value_type> toStdVec() const{ return std::vector<value_type>(begin(),end());}
    std::set<value_type> toStdSet() const{ return std::set<value_type>(begin(),end());}

    //���Ԫ��ȥ�أ������Ҫȫ��ȥ�أ���������
    void unique() { this->unique([](value_type v1,value_type v2){ return v1 == v2;});  }
    template<typename EqualFunction>
    void unique(EqualFunction equalFunction)
    {
        iterator it = begin();
        iterator itEnd = end();
        if (it == itEnd)
        {
            return;
        }

        iterator itNext = it;
        while (++itNext != itEnd)
        {
            if (equalFunction(*it, *itNext))
            {
                erase(itNext);
            }
            else
            {
                it = itNext;
            }

            itNext = it;
        }
    }
    //ȫ��Ԫ��ȥ�أ�����Ҫ�����ұ�֤ԭ˳��
    void uniqueAll(){ this->uniqueAll([](value_type v1,value_type v2){ return v1 == v2;});}
    template<typename EqualFunction>
    void uniqueAll(EqualFunction equalFunction)
    {
        for(auto it=begin();it!=end();++it)
        {
            //auto it2= std::find_if(it+1, end(), std::bind(equalFunction,std::placeholders::_1,*it));
            //if(it2!=end()) erase(it2);
            //���淽�����죬��find_if�㷨
            for(auto itFind= it+1;itFind !=end();)
            {
                if (equalFunction(*it, *itFind))
                {
                    itFind = erase(itFind);
                } else
                {
                    ++itFind;
                }
            }
        }
    }
private:
    //Ĭ��������󳤶�
    enum { DEFAULT_MAX_ALLOC_SIZE = 1000000 };

    //��������ʣ������
    size_t alloc_size = 0;
    //�������������
    size_t alloc_size_max = DEFAULT_MAX_ALLOC_SIZE;
    //�û������ڱ���㣬�����ν�ѭ������������ͷָ���жϣ������Ԫ�ء�
    NodeBase* usedguard = nullptr;
    //����������ʵ�ֵ�������������л�ȡ���ͷ���黹������ͷ��㴦����
    NodeBase* allocated = nullptr;

private:
    void _initialize()
    {
        if (nullptr == usedguard)
        {
            //�ڱ����ΪNodeBase���ͣ��������ݣ�������ΪNode����ָ�롣
            usedguard = new NodeBase;
        }
        //��ʼ��ѭ������
        usedguard->next = usedguard;
        usedguard->prev = usedguard;
    }

    void _finalize()
    {
        //ɾ������Ԫ�أ�����ձ����ڴ�
        clear();
        //ɾ��˫������
        if (nullptr != usedguard)
        {
            delete usedguard;
            usedguard = nullptr;
        }
    }

    void _del_node(NodeBase *pNode)
    {
        if (alloc_size < alloc_size_max)
        {
            //���뱣�������У���Ϊͷ��㡣
            pNode->next = allocated;
            allocated = pNode;
            ++alloc_size;
        }
        else
        {
            //�����ڴ���أ�ֱ������
            delete pNode;
        }
    }

    Node* _get_node()
    {
        if (alloc_size > 0)
        {
            //�ӱ��������ȡ�ռ�
            --alloc_size;
            NodeBase *pNode = allocated;
            allocated = allocated->next;
            return static_cast<Node *>(pNode);
        }
        else
        {
            //��������Ϊ�գ�֮������Node����
            return new Node;
        }
    }

    //ʵ��prev��λ�ƣ���first��prev��Ϊlast��prev��last��prev��Ϊpos��prev��pos��prev��Ϊfirs��prev��
    void _transfer(iterator pos, iterator first, iterator last)
    {
        if (pos != last)
        {
            //���Ե�prev�ҵ��µ�next
            last.node->prev->next = pos.node;
            first.node->prev->next = last.node;
            pos.node->prev->next = first.node;
            //�������µ�prev
            NodeBase *temp = pos.node->prev;
            pos.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = temp;
        }
    }

};

}
