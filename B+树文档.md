# B+树类说明
#### 简述：
    是一个模板类，要提供的模板参数：
    Key              B+树的主键类型
    value_t          B+树存储的元素类型
    块大小           默认（4096字节），自适应Key和value_t类的大小而改变一个块中存储多少个Key和value_t
    Key的标准比较函数less 

    构造时需要的参数：
    char *filename,  用来维护B+树的文件名字(二进制存储)
    char *indexname, 用来维护filename文件本身信息的文件名

    是一个用k个键分割k个儿子的标准的普通B+树，没有内存池优化。
    对于每一个文件中4096字节的节点，它的结构是先有一个标记这个节点基本信息的node，后面是若干个Key和off_t的对，或Key和value_t的对。

#### private函数：
    size_t findPos_leaf(_buffer _buf, const Key & key, node & p)
    功能：
        返回在p这个node标记下的叶节点中第一个大于等于key的位置
    size_t findPos_Node(_buffer _buf, const Key & key, node & p)
    功能：
        类似上，返回在内节点中第一个大于等于key的位置

    /*--------------------------------------------------------------*	
	 *                           insert                             *
     *--------------------------------------------------------------*/
    node insert_recursive(node& now, const Key& key, const value_t& val) :
    功能：
        插入过程的内函数。
        对于叶节点，插入Key和value_t，并返回可能的更新信息。

        对于返回的信息：
        若没有发生分裂，则返回插入后本节点的node以便于父亲处更新信息。
        若发生分裂，则返回分裂出的新节点的node以便于父亲处增加新节点信息。

        对于内节点，找到Key应在的儿子的位置，并在儿子处完成插入，返回可能的更新信息。
        在插入中用到下面四个函数，分别为向叶子插入，向内节点插入（在儿子处发生分裂时），叶子和内节点分别的分裂函数。       
    
    node buf_leaf_insert(_buffer _buf, const Key & key, const value_t & val, node & p)
    node buf_Node_insert(_buffer _buf, const Key & key, const off_t & son, node & now, const size_t pos)
    node buf_Node_split(_buffer _buf, node & now)
    node buf_Leaf_split(_buffer _buf, node & now)

    /*--------------------------------------------------------------*	
	 *                            erase                             *
     *--------------------------------------------------------------*/
    sjtu::pair<int, Key> erase_recursive(node & now, const Key & key, off_t left, off_t right)
    功能：
        删除过程的内函数。
        对于叶节点，删除key和对应的val，返回可能的更新信息。
        对于内节点，找到Key应在的儿子的位置，并在儿子处完成删除，返回可能的更新信息。
        删除中用到
    
        对于返回的pair对，表示了四种情况：
            -2 儿子处没有发生合并，从左兄弟处借了一个key。
            0  儿子处没有发生合并，从右兄弟处借了一个key。 
            -1 儿子处与左兄弟发生合并。
            1  儿子处与右兄弟发生合并。
        
        过程中用到了以下函数，分别用于平衡（从兄弟处借key或者二者合并），合并。

    sjtu::pair<int, Key> Node_balance_right(_buffer _buf, node & now, node & right)
    sjtu::pair<int, Key> Node_balance_left(_buffer _buf, node & now, node & left)
    void merge_Node(node & now, node & right)

    void buf_leaf_erase(_buffer _buf, node & now, size_t pos) 
    sjtu::pair<int, Key> leaf_balance_right(_buffer _buf, node & now, node & right)
    sjtu::pair<int, Key> leaf_balance_left(_buffer _buf, node & now, node & left)
    void merge_leaf(node & now, node & right)

    /*--------------------------------------------------------------*/
    sjtu::pair<node, int> get_bigger_key_pos(const Key & key)
    功能：
        用于get_range，返回第一个大于等于key的叶子节点和在此叶子节点中的位置。


#### public函数：
     void insert(const Key &key, const value_t &val);
     功能：
        向B+树中插入一个key和val的pair，
        若key已存在则throw(duplicated_key());

    void erase(const Key &key);
    功能：
        删除B+树中存在的一个key及其对应的val。
        若key不存在则throw(nonexistent_key());

    int count(const key &key);
    功能：
        返回key在B+树中是否存在。
        0为不存在，1为存在。

    value_t zhoumo(const Key &key)
    功能：
        返回key对应的value_t，若不存在返回value_t的默认构造函数。

    void get_range(sjtu::vector<value_t> & vec, const Key & lkey, std::function<bool(const Key&, const Key&)> func)
    功能：
        将B+树中从lkey开始的直到不满足func条件之前的key对应的value_t加入vec的尾。
    
    void get_range(sjtu::vector < sjtu::pair < Key, value_t> > & vec, const Key & lkey, std::function<bool(const Key&, const Key&)> func) 
    功能：
        将B+树中从lkey开始的直到不满足func条件之前的key对应的Key 和对应的 value_t 的对加入vec的尾。

    void range_search(const Key & lkey, const Key & rkey, ostream & os)
    功能：
        将B+树中lkey到rkey之间key对应的value_t输出到os中。

    int modify(const Key & key, const value_t & val)
    功能：
        修改B+树中key对应的val信息。
        返回1表示修改成功，0表示B+树中没有key，修改失败。

#### DEBUG函数：
    为了进行小数据量的肉眼查错，使用了两个函数输出B+树信息。
    void traverse_Leaf()
    功能：
        依次输出所有叶子节点中的key和val的对。
    
    void traverse_tree()
    功能：
        从根开始先序遍历整棵B+树。
        对于叶子节点：
            输出叶子节点有多少个key，并依次输出该节点存储的key和value_t的对。
        对于内节点：
            输出内节点共有多少个key，依次输出该节点存储的key和对应儿子在文件中的地址。
            依次traverse遍历儿子。

#### 文件管理：
    使用了disk_manager类来存储B+树文件中有哪些位置是空的，将这些空位置写在了另一个文件中。
    
    private：
        int num;  目前文件中共有多少个空的位置
        off_t file_end; 目前文件的最后一个位置

    public函数：
        off_t new_pos() 
            返回一个文件中的空位置。若有在文件中间的空位置则读取空位置并返回第一个，若没有则返回最后一个位置。

        void delete_pos(off_t del_pos)
            将del_pos加入文件空位置，并写入文件。


