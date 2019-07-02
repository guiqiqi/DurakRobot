# Durak 游戏机器人
这篇文章记录了一个简单的 Durak 游戏机器人实现。
这个机器人由 9 个部分组成，他们分别是：
1. 卡牌实现 - `card.h`
2. 游戏规则实现 - `rule.h`
3. 针对卡片和局势的动态权重计算器 - `ranker.h`
4. 记牌器与游戏者管理 - `counter.h`
5. 玩家实现 - `iplayer.h`
6. 树算法实现 - `tree.h` &  `data.h`
7. 使用生成树算法构建预测树 - `thinker.h`
8. 分析预测树 - `thinker.h`
9. 其他 - `player.h` & `translation.h`

需要进行说明，在 `header.h` 中记录了程序的一些常量，并前置声明了程序中可能用到的所有类、结构体；在每一个除了它之外的头文件都会包含该头文件以避免编译错误。

## 第一部分 - 卡牌实现
这部分由源代码中的 `card.h` 实现，下面是简单的说明：
### `iCard` 类
```
class iCard {
private:
	std::string _suit;
	std::string _rank;

public:
	iCard(std::string, std::string);
	std::string suit(void) const;
	std::string rank(void) const;
	static int compare(const iCard*, const iCard*);
	bool operator>(const iCard*&);
	bool operator<(const iCard*&);
	bool operator>=(const iCard*&);
	bool operator<=(const iCard*&);
	bool operator==(const iCard*&);
	friend std::ostream& operator<<(std::ostream&, const iCard&);
};
```
`iCard` 为程序内部使用的卡片类，他拥有两个类型为 `std::string` 的私有变量：`_suit` 与 `_rank`，这分别对应了卡牌的花色与牌面数字。
类通过传入两个标准字符串进行初始化，并提供了 `suit` 与 `rank` 两个公开接口用以获取卡牌的花色与牌面数字。
类通过运算符重载支持了 `< > >= <= ==` 这些比较运算；当进行比较运算时会将当前牌的 `_rank` 与 目标牌的 `_rank` 进行比较。
同时，类提供了一个静态公开接口 `compare(const iCard*, const iCard*)` 用来在非实例化时进行卡牌之间的比较：当第一个参数的牌较大时，该接口会返回正数、当第二个较大时会返回负数、当两者相同时会返回 `0`。
类通过重载 `<<` 输出运算符实现了对牌信息的打印支持。

### `CardManager` 类
```
class CardManager {
private:
	std::vector<iCard*> all;
	std::unordered_map<std::pair<std::string, std::string>, iCard*> library;

public:
	CardManager(void);
	~CardManager(void);
	std::vector<iCard*> getall(void);
	iCard* get(std::string&, std::string&);
	iCard* get(std::pair<std::string, std::string>&);
};
```
当实例化一个 `CardManager` 类时，类会在堆内存建立所有的卡片，并在内部的 `all` 变量中保存所有的指针；当销毁该实例时用于释放内存。
类提供两个重载的 `get` 接口，用于获得卡在内部的指针。

因为在 `CardManager` 的实现中需要使用 `std::unordered_map` 以 `std::pair<std::string, std::string>` 作为键，所以在这里实现了自定义类型的 `hash` 拓展：
```
// Specialized hash support for pair<string, string>
namespace std {
	template<>
	class hash<std::pair<std::string, std::string>> {
	public:
		std::size_t operator()(const std::pair<std::string, std::string>& obj) const {
			std::hash<std::string> first;
			std::hash<std::string> second;
			return first(obj.first) + second(obj.second);
		}
	};
}
```

## 第二部分 - 游戏规则实现
由于进攻和防守在规则上的差异，我们在 `rule.h` 中提供了两个不同的接口用于计算可以使用的牌。
`rule.h` 中的两个公共接口有着相同的参数结构：`std::vector<iCard*>& desk, std::vector<iCard*>& inhand`，其中 `desk` 指的是桌面上当前有的牌，而 `inhand` 指的是玩家的手牌。

### 进攻
根据游戏的规则，进攻者需要找到当前桌面上出现过号码的牌作为进攻手牌，于是我们首先需要统计在当前牌桌上曾经出现过的牌号。
在类中自定义了一个私有接口 `shown` 用于返回曾经出现过的卡牌的序号。注意，该接口返回的是 `std::unordered_set<std::string>` 类型，因为牌号在卡内的存储是通过标准字符串类型实现的，而在这里我们需要频繁的查找某一字符串是否在该序列内，所以在返回值中使用哈希表可能有助于提高性能。
```
std::vector<iCard*> Rule::attack(std::vector<iCard*>& desk, std::vector<iCard*>& inhand) {
	
	// If inhand is empty
	if (inhand.size() == 0) return std::vector<iCard*>();

	// When the offense board's desktop is empty
	// you can play every cards.
	if (desk.size() == 0) return inhand;

	// Otherwise, you can only play cards that you have ever shown.
	std::vector<iCard*> possible;
	std::unordered_set<std::string> shown = this->shown(desk);
	std::unordered_set<std::string>::iterator notexist = shown.end();

	for (auto& card : inhand) {
	
		// Deal with existed card
		const std::string rank = card->rank();
		if (shown.find(rank) != notexist)
			possible.push_back(card);
	}

	return possible;
}
```
当手牌是空的时候，意味着我们需要返回空序列；而当桌面上没有牌的时候，我们需要返回当前所有的手牌（因为这个时候我们可以出任意的手牌）。

### 防守
防守的规则稍微复杂一些：玩家可以打出比进攻方牌号更大的牌，或者是王牌中的任意一张；而当进攻方使用王牌进攻时，防守方则需要使用比进攻方更大的王牌来进行防守，否则宣告防守失败。
在这里我们需要判断一张牌是不是王牌，于是在类中定义了私有接口 `istrump`，该接口用于判断指定的牌是否是王牌。
同时，我们还需要获得桌子上的最后一张牌作为要防守的牌。这里，因为 `vector` 是有序的，我们的每次 `push_back` 操作用于向桌子上打入一张牌，所以需要调取最后一张牌时，既可以通过 `std::vector<iCard*>::back` 实现。这部分程序代码在 `Rule::last` 中实现。
```
std::vector<iCard*> Rule::defend(std::vector<iCard*>& desk, std::vector<iCard*>& inhand) {

	// If inhand or desk is empty
	if (inhand.size() == 0) return inhand;

	// In defend module, you can play card with bigger or trump card
	std::vector<iCard*> possible;
	const iCard* last = this->last(desk);

	std::string trump_suit = trump->suit();
	std::string last_suit = last->suit();
	bool defend_trump = (last_suit == trump_suit);

	for (auto& card : inhand) {

		std::string card_suit = card->suit();

		// When in the trump defensive mode, 
		// the current card needs to be greater than the last;
		if (defend_trump == true) {

			// If it is general card
			if (card_suit != trump_suit)
				continue;

			if (card->operator>(last))
				possible.push_back(card);

			continue;
		}

		// Otherwise, if the current card is a trump card, 
		// we can defend successfully;
		if (card_suit == trump_suit) {
			possible.push_back(card);
			continue;
		}

		// When both cards are not ace, 
		// check if the two cards are the same suit;
		if (card_suit == last_suit) {

			// Defensive success when the same suit 
			// and the current card is larger
			if (card->operator>(last))
				possible.push_back(card);

		}

	}

	return possible;
}
```
代码中的布尔变量 `defend_trump` 用于指示程序当前需要防守的牌是否为王牌，以进行不同的防守逻辑。

## 第三部分 - Ranker: 动态的权重计算器
无论在进攻还是防守过程中，我们都可能遇到有多张牌可以使用的情景。这时，我们需要衡量一张牌对己方的重要性；通过 Ranker 类，我们可以计算在当前情景下某一章指定的手牌对我们的重要性，下面简述任一牌的权重计算因素：

1. 牌的序号越大，则其权重越大
2. 若是王牌，则其权重要比一般的牌更大
3. 如果这张牌的同号码牌在手牌中重复出现，那么在下一轮进攻中可能有用，其权重增大
4. 游戏进行到后期时，权重大的牌对我们来说相对更加重要，其权重增大

基于以上考虑，为了程序的解耦合，我们将上面的4条规则定义为4个公共接口，在 `ranker.h` 中实现：
```
class Ranker {
private:
	Counter<global::players>* _counter;
	const iCard* _trump;

public:
	static double zipvalue(double);
	static double absolute(const iCard* card);

	Ranker(Counter<global::players>*, const iCard*);
	double progress(void);
	double trump(const iCard* card);
	double repeat(iPlayer*, const iCard*);
};
```

### `Ranker::progress` - 计算游戏进程
```
// Calc for progress coefficent
double Ranker::progress(void) {
	double all = global::total;
	double left = this->_counter->unknown().size();
	if (left == 0) left = 1;
	double progress = all / left;
	return this->zipvalue(progress);
}
```
在这部分中，我们读取了游戏中位置卡片的数量 - `left`，将其除游戏的初始卡牌数量（默认是52，定义在 `global::total` 中），便得到了游戏进程相关的因子（该值一定大于`1`）。
需要注意的是，当游戏刚刚开始时，该值可能会非常大（例如 `42/2 = 21`），我们需要将其压缩在 `[1, 2]` 的区间内，在这里使用 `Ranker::zipvalue` 接口：
```
// Scale the function value field to [1,2]
double Ranker::zipvalue(double value) {
	return atan(value) / (0.5 * M_PI) + 1; 
}
```
该接口使用数学函数将任意大的值压缩至 `[1, 2]` 的区间内，其函数图像如下：
![zipvalue](https://github.com/guiqiqi/DurakRobot/raw/master/images/zipvalue.png)
其函数定义如下：![](http://latex.codecogs.com/gif.latex?\\frac{\\arctan{x}}{0.5*\\pi}+1)

### `Ranker::trump`
该接口实现如下：
```
// Add weight to the trump card
double Ranker::trump(const iCard* card) {
	if (card->suit() == this->_trump->suit())
		return global::trump_factor;
	return 1.0;
}
```
当判断该牌是王牌时返回自定义的参数 `global::trump_factor`，否则返回 `1`

### `Ranker::absolute`
该接口用于计算纯粹基于牌号的权重值，将牌号乘以自定义的参数值 `global::unconstrained_factor` 即可获得：
```
// Calc absolute rank for card
double Ranker::absolute(const iCard* card) {
	std::string rank = card->rank();
	double raw = global::iranks[rank];
	return global::unconstrained_factor * raw;
}
```

### `Ranker::repeat`
该接口统计在手牌中出现重复牌的数量，并乘以自定义参数 `global::repetition_factor` 即可获得：
```
double Ranker::repeat(iPlayer* player, const iCard* target) {
	int count = 0;
	std::string rank = target->rank();
	std::vector<iCard*>& inhand = player->hand();
	for (auto& card : inhand)
		if (card->rank() == rank)
			count++;

	return global::repetition_factor * count;
}
```

## 第四部分 - 记牌器与游戏者管理
该部分在 `counter.h` 中定义，因为需要在后面的实现中对局势进行*模拟推演*，所以我们并不在游戏者 `iplayer.h` 中记录游戏者的手牌信息，而是将其放入 `Counter` 类的几个私有属性中：
```
template <int number>
class Counter {
private:
	std::vector<iCard*> _desk; // cards in desk
	std::vector<iCard*> _unknown; // card we don't know temproary

private:
	CardManager* manager;
	std::array<iPlayer*, number> players; // all players
	std::array<size_t, number> count; // count of every one's cards
	std::array<std::vector<iCard*>, number> inhand; // cards for everyone

public:
	Counter(CardManager&, iCard*);
	Counter(const Counter<global::players>& counter);

	// Moveout one card from a specified vector
	static void moveout(std::vector<iCard*>& set, iCard*& target);

public:
	int join(iPlayer* player); // join a player
	iPlayer* player(int index) const; // return player
	iPlayer* player(std::string name) const; // return player

public:
	void grab(iPlayer*);
	void replenish(iPlayer*, int);
	void hit(iPlayer*, iCard*);
	void get(iPlayer*, iCard*);
	void hit(iPlayer*, std::string&, std::string&);
	void get(iPlayer*, std::string&, std::string&);

public:
	void clear(void); // clear desk
	std::vector<iCard*>& desk(void); // return desk
	std::vector<iCard*>& unknown(void); // return unknown
	std::vector<iCard*>& hand(iPlayer*); // return inhands for player
	int left(iPlayer*); // return number of remaining cards for player
};
```
类中的私有变量分别记录如下信息：
- `std::vector<iCard*> _desk`: 当前牌桌上的牌
- `std::vector<iCard*> _unknown`: 暂时未知的牌（牌堆）
- `CardManager manager`: 上文记述的卡片管理器，提供初始化卡片存储
- `std::array<iPlayer*, number> player`: 记录所有的玩家指针
- `std::array<size_t, number> count`: 记录对应位置玩家的手牌数量
- `std::array<std::vector<iCard*>, number> inhand;`: 记录实际在对应位置玩家手牌的序列

类的公有方法说明：
- `int Counter::join`: 加入一个玩家
- `iPlayer Counter::player(int index) const`: 按照位置查找玩家
- `iPlayer Counter::player(std::stirng name) const`: 按照名称查找玩家
- `Counter::grab`: 某玩家认输，并拿走桌上所有的牌
- `Counter::replenish`: 某玩家拿牌，但并不知道拿到的牌信息
- `Counter::get`: 某玩家拿牌，并记录牌信息
- `Counter::hit`: 某玩家打出某张牌
- `Counter::clear`: 清理桌面
- `Counter::desk`: 返回桌面引用
- `Counter::unknown`: 返回未知牌堆的引用
- `Counter::hand`: 返回某一玩家的手牌引用
- `Counter::left`: 返回某一玩家的手牌数量

需要注意的是，尽管上述接口中手牌数量返回值可能不为 `0`，但是在调用玩家手牌的引用中，可能会返回空序列，这是因为我们在某些情况中并不清楚敌方手中牌的信息，但是却知道敌方牌的数量。
`Counter` 类为模板类，需要在初始化时给定玩家数量，该数量在 `global::players` 中指定。
以上接口的实现较为简单，在 `counter.cpp` 中实现。

## 第五部分 - 玩家实现
玩家个体在 `iplayer.h` 中实现，其定义如下：
```
class iPlayer {
private:
	int _index; // our index
	bool _we; // its our player
	bool _turn; // our turn or not
	bool _status; // attack or defend
	std::string _name;

	Rule* analyser;
	Ranker* ranker;
	Counter<global::players>* counter;

public:
	int total(void); // return total count of our cards
	std::vector<iCard*>& desk(void); // return ref for desk cards
	std::vector<iCard*>& hand(void); // return ref for inhand's cards
	std::vector<iCard*>& unknown(void); // return ref for unknown cards

public:
	iPlayer(bool we, bool turn, bool status, std::string name);
	bool us(void) const; // return if it's our player
	bool turn(void) const; // return if it's our turn
	bool status(void) const; // return status for player
	int index(void) const; // return index
	std::string name(void) const; // return name
	void set_index(int);
	void set_counter(Counter<global::players>* counter); // reset counter for think
	void set_analyser(Rule* analyser); // set analyser
	void set_ranker(Ranker* ranker); // set ranker

public:
	void grab(void); // grab all cards
	void replenish(void); // get some cards we dont know
	void get(iCard*); // get one card
	void hit(iCard*); // hit one card
	void get(std::string&, std::string&); // get one card
	void hit(std::string&, std::string&); // hit one card

public:
	std::vector<iCard*> analysis(void); // analysis we could play
	double posibility(const iCard* card); // return for one card's posibility
	double rank(const iCard* card); // return for one card's rank

public:
	void transform(void); // takein/out our turn's control
	void toggle(void); // change our attack/defend mode
};
```
类的私有变量表示的意义如下：
- `_index`: 该玩家在 `Counter` 类中的玩家编号
- `_we`: 该玩家属于已知玩家（手牌信息是否全部知晓）
- `_turn`: 是否轮到该玩家进攻、防守
- `_status`: 该玩家处于进攻\防守状态
- `_name`: 玩家名称

以上大部分接口的实现都十分简单，可以参考 `iplayer.cpp` 中的代码以及代码注释。
需要说明的是如下三个接口：
- `std::vector<iCard*> iPlayer::analysis(void)`: 该接口通过调用上文记述的 `Rule` 类在玩家内部存储的实例来分析玩家当前可以打出的牌
- `double iPlayer::rank(const iCard* card)`: 该接口通过调用上文记述的 `Ranker` 类实例来计算某一张牌的权重大小
- `double iPlayer::posibility(const iCard* card)`: 该接口计算某一张牌在当前玩家手牌中的概率，计算方法如下：
	1. 当该玩家属于已知玩家（查看 `_we` 私有变量）时，如果指定牌在我们的手牌记录中，则返回 `1`，否则返回 `0`
	2. 当该玩家不属于已知玩家，但指定牌在手牌记录中时，返回 `1`
	3. 当该玩家不属于已知玩家，但指定牌不在未知牌以及手牌记录中时，说明该张牌已经不存在，属于废牌，返回 `0`
	4. 当该玩家不属于已知玩家，且在未知牌中找到了目标牌时，按照如下方法计算：
		1. `let a = unknown.size()` - 计算未知牌堆的大小
		2. `let b = inhand.size()` - 计算已知手牌的数量
		3. `let c = total` - 获取总共手牌的数量
		4. 概率为：![](http://latex.codecogs.com/gif.latex?\\frac{c-b}{a+c-b})

其原理为：
问题相当于从n个对象中提取m并查找指定的对象，由概率论知识可以知道，其概率为：![](http://latex.codecogs.com/gif.latex?\\frac{C_{m-1}^{n-1}}{C_m^n}=m/n=\\frac{c-b}{a+c-b})

## 第六部分 - 树算法实现
树算法用来在后续的局势预测中发挥作用，树由三部分组成：数据(`Data`)、节点(`Node`)、树(`Tree`)，下面分别介绍。

### 数据类
数据类在 `data.h` 中定义，其组成很简单：
```
struct Data {
	double rank; double possibility; iCard* card;
	Data(iCard* card, double rank, double possibility);
};
typedef struct Data Data;
```
其中，`rank` 代表当前节点所代表卡的权重；`possibility` 用来表示该节点描述情况的出现可能性；`card` 为该节点要操作的卡牌。

### 节点类
节点类定义如下：
```
/* Tree (data structure) support */
class Node {
private:
	int _degree;
	Data* data;
	Node* _father = nullptr;
	std::vector<Node*> children;

public:
	Node(void);
	Node(const Data& data);
	~Node(void);

	std::vector<Node*>::const_iterator begin(void) const;
	std::vector<Node*>::const_iterator end(void) const;

public:
	Data* get(void) const;
	int degree(void) const;
	Node* father(void) const;

	void add(Node*& child);
	void add(const Data& data);
	void remove(const int index);

	Node* operator[] (int index);
	const Node* operator[] (int index) const;
};
```
私有变量描述：
- `_degree`: 该节点的度
- `data`: 该节点 `Data` 类指针
- `_father`: 父节点地址
- `children`: 子节点列表

因为我们需要使用 `C++ 11` 新特性 - 基于范围的循环来遍历节点的子节点，所以该类实现了 `begin` 与 `end` 方法。
节点类通过重载 `[]` 操作符实现了索引操作的支持。
调用节点类的析构函数会尝试将所有子节点全部从堆内存中删除。

### 树类
树类的定义如下：
```
/*
	Tree support
	bool exist(Node* target); // Find if specified node in tree
	std::vector<Node*> path(const Node*); // Find path for specified node
*/
class Tree {
private:
	Node* root;

public:
	Tree(Node*);
	~Tree(void);
	bool exist(const Node*);
	std::stack<Node*> search(const Node*);
	static std::stack<Node*> path(const Node*);

	// Give an iterator that generates a root node to a leaf node
	class Parser {
	private:
		bool running = true;
		DFSTraverse traverser;

	public:
		Parser(Node*);
		bool status(void) const;
		std::stack<Node*> yield(void);
	};

	Tree::Parser leaves(void);
};
```
其中，`Tree::Parser` 使用 `DFS` 算法实现了一个从根节点到所有叶子节点的路径遍历器（下文中会介绍）。其使用方法如下：
```
Tree::Parser parser = tree.leaves();
while (parser.status()):{
	std::stack<Node*> path = parser.yield();
	...
}
```
公共方法介绍：
1. `Tree(Node*)` - 类构造函数，用来存储根节点
2. `~Tree(Node*)` - 类析构函数。需要注意的是，析构函数会尝试删除 `root` 根节点，所以需要将根节点存储在堆内存。而根据上文 `Node` 类的定义，这会递归的从堆内存中删除所有的节点
3. `bool exist(const Node*)` - 使用 `BFS` 算法在树中查找指定节点，找到目标节点后返回 `true`，否则返回 `false`
4. `std::stack<Node*> search(const Node*)` - 静态函数，由于每个节点存储了父节点的信息，调用该函数会尝试遍历所有的父节点，将父节点入栈，并返回栈
5. `std::stack<Node*> search(const Node*)` - 以上两个函数的合并调用：函数会尝试在树中寻找目标节点，找到之后会返回该节点的路径信息；否则返回空栈
树示意图：
![tree](https://github.com/guiqiqi/DurakRobot/raw/master/images/tree.png)

## `BFS` `DFS` 算法与 `Traverse` 类
### `BFS` 算法
`BFS` - `Breadth-first search`
`BFS` 算法在内部维护了一个队列，利用队列先进先出(FIFO)的特性，递归的遍历树的所有节点。其遍历过程如下：
1. 先将根节点加入任务队列
2. 从队列中取出一个节点，将它所有的子节点加入任务队列
3. 重复第二步，直到队列为空

因为队列 - `std::queue` 是先进先出的，这样总会按照层级顺序遍历树，其示意图如下：
![bfs](https://github.com/guiqiqi/DurakRobot/raw/master/images/bfs.png)

### `DFS` 算法
`DFS` - `Depth-first search`
`DFS` 算法与 `BFS` 基本相同，只不过它内部维护的是一个栈，与队列不同的是：栈是先进后出(FILO)的，所以上面的遍历方法会优先按照深度遍历，其示意图如下：
![dfs](https://github.com/guiqiqi/DurakRobot/raw/master/images/dfs.png)

### `Traverse` 类
因为程序中要同时实现 `BFS` 与 `DFS` 遍历，而其大部分代码都是相同的，于是我们先实现相同部分的基类：`Traverse`，其定义如下：
```
/* Traversing support */
template <typename Contaniner>
class Traverse {
protected:
	Node* current;
	bool running = true;
	Contaniner unvisited;

public:
	// Prevent the compiler from generating a default constructor
	Traverse(void) { this->current = nullptr; }

	typedef Node* data;
	Traverse(Node* root) {
		this->unvisited.push(root);
		this->current = root;
	}

	bool status(void) const {
		return this->running;
	}

	virtual void next(void) = 0;

	data get(void) const {
		return this->current;
	}

	data yield(void) {
		try {
			this->next();
			data current = this->current;
			return current;
		}
		catch (std::out_of_range) {
			this->running = false;
			return nullptr;
		}
	}
};
```
这是一个模板类，使用 `Container` 可以指定数据容器（当为 `Stack` 时实现 `DFS`，`Queue`时实现 `BFS`）。而在某些方法上 `Stack` 与 `Queue` 的接口不同，于是我们将调取数据的接口 `next` 设置为纯虚函数，在子类中继承并实现它：
```
/*
	Traversing the tree using the BFS algorithm
	Traverse all nodes of the tree using the queue's first-in,
	first-out (FIFO) feature level.
*/
class BFSTraverse : public Traverse<std::queue<Node*>> {
	using Traverse::Traverse;
	void next(void) {
		// If queue is empty
		if (!this->unvisited.size())
			throw std::out_of_range("traverse ended.");

		this->current = this->unvisited.front();
		this->unvisited.pop();

		// Traversing the child nodes to the queue
		if (!this->current->degree())
			return;
		for (auto child : *(this->current))
			this->unvisited.push(child);
	}
};

/*
	Traversing the tree using the DFS algorithm
	The principle is the same as above, but the stack is LIFO
*/
class DFSTraverse : public Traverse<std::stack<Node*>> {
	using Traverse::Traverse;
	void next(void) {
		// If list if empty
		if (!this->unvisited.size())
			throw std::out_of_range("traverse ended.");

		this->current = this->unvisited.top();
		this->unvisited.pop();

		// Traversing the child nodes to the queue
		if (!this->current->degree())
			return;
		for (auto child : *(this->current))
			this->unvisited.push(child);
	}
};
```
这样通过分别实例化 `BFSTraverse` 和 `DFSTraverse` 类就可以实现两个不同的树节点遍历器了。
结合上面的节点、树的实现就可以实现基础的树维护算法。

## 第七部分 - 使用生成树算法的局势预测器
上面我们分别实现了树算法、玩家类、规则类、以及概率和权重计算，接下来我们将以上的所有部分综合起来，利用生成树的算法实现一个简单的局势预测器。
首先我们在生成每个节点的时候需要保存的信息至少有这些：
1. 当前节点的状态 - 进攻 或者 防守
2. 当前节点的深度 - `layer`
3. 当前节点要操作的牌 - `card`
4. 当前节点的父节点 -` father`
5. 当前节点的局势 - `situation`

于是我们得到以下预测节点结构体 - `thinker.h`：
```
// Node for prediction, using in task queue
struct Prediction {
	bool mode; // attack or defense
	int layer; // current prediction's layer
	iCard* card; // operate card
	Node* father; // father node
	Counter<global::players>* situation; // layer
};
```
需要注意的是，我们每一个节点都需要将当前的局势 `Counter` 类完整的拷贝一份，以便在进行预测操作的同时不影响原来游戏的进度；这需要 `Counter` 类支持拷贝构造，并在拷贝构造函数中完整的将所有数据进行拷贝操作。这一部分在上面 `Counter` 类中已经实现，不再赘述。

接下来我们需要利用上面的预测节点递归的生成一颗符合 `Data` 结构体的树，这部分的定义在 `TreeMaker` 类中：
```
class TreeMaker {
private:
	Node* root = nullptr; // root node
	int maxlayer = global::depth;
	iPlayer* attacker; iPlayer* defender;

private:
	std::queue<Prediction*> tasks; // tasks queue
	static Node* complex(iCard*& card, iPlayer*& player);
	std::vector<Counter<global::players>*> trashbin;

public:
	TreeMaker(bool mode, iCard* rootcard, iPlayer* attacker, 
		iPlayer* defender, Counter<global::players>* situation);
	~TreeMaker(void);

public:
	Tree* make(void); // remember to delete tree after use!
};
```
可以看到，类中存储了任务队列（`tasks`）、进攻者玩家，防守者玩家，根节点。
下面是生成树的实现：
```
/* Using BFS algorithm to generate pretree */
Tree* TreeMaker::make(void) {
	// Produce with tasks queue
	while (!this->tasks.empty()) {

		// Get prediction job
		Prediction* prediction = this->tasks.front();
		bool mode = prediction->mode;
		int layer = prediction->layer;
		iCard* card = prediction->card;
		Node* father = prediction->father;
		Counter<global::players>* situation = prediction->situation;
		this->tasks.pop();

		// Get player
		iPlayer* player; iPlayer* passive;
		if (mode == global::attack) {
			player = this->attacker;
			passive = this->defender;
		}
		else {
			player = this->defender;
			passive = this->attacker;
		}

		// Make node and insert to father while not root
		Node* child;
		if (father == nullptr) {
			child = this->root;
		}
		else {
			child = this->complex(card, player);
			father->add(child);
		}

		// Make predictions and add new tasks
		{
			this->attacker->set_counter(situation);
			this->defender->set_counter(situation);
			player->hit(card);

			/*
				If current prediction layer miner global::depth,
				analyze possible cards in the current situation;
				and also exchange offense and defense.

				Otherwise, stop prediction.
			*/
			if (layer < this->maxlayer)
				for (auto& card : passive->analysis()) {
					Prediction* prediction = new Prediction;
					prediction->card = card;
					prediction->mode = !mode;
					prediction->layer = layer + 1;
					prediction->father = child;
					prediction->situation = \
						new Counter<global::players>(*situation);
					this->trashbin.push_back(prediction->situation);
					this->tasks.push(prediction);
				}
		}

		// Release memeory
		delete prediction;
	}

	Tree* tree = new Tree(this->root);
	return tree;
}
```
我们进行了如下操作：
1. 我们首先获取了所需要的信息，包括操作的卡，当前进攻状态，预测层数
2. 在这之后我们判断了当前的进攻者与防守者状态，并指定了玩家(`player`)与“配合者”(`passive`)
3. 下面我们使用类的 `complex` 接口（下文描述）将以上的信息转换为一个 `Node*` 节点，并添加到指定的父节点中去
4. 之后进入新预测分支的生成过程：
	1. 我们通过 `set_counter` 接口将进攻者和防守者的 `situation` 设置为当前拷贝出的局势，以免干扰其他分支预测
	2. 我们将要进行的操作执行 - `player.hit(card)`
	3. 在这之后我们判断当前的预测深度是不是已经达到了指定的最大深度 `global::depth`，如果已经达到，则不再进行新的分支预测，否则进行下面的操作
	4. 因为上面的玩家 `player` 已经进行了操作，需要“配合者” - `passive` 对其进行回应，我们对他的所有出牌可能进行遍历，对每一种可能性都生成一个新的预测分支，并将其加入任务队列，*并将当前的局势拷贝加入类的垃圾筐(`trashbin`)中*
	5. 返回第1步的操作

需要注意的是，为了防止爆栈，我们的预测节点(`Predction`)，局势拷贝(`Counter`)、以及树节点(`Node`)全部都在堆内存中进行操作，所以在使用完成之后需要及时的释放内存。
而在 `TreeMaker::trashbin` 中则保存了所有上述预测过程中的局势拷贝，在类的析构函数中将它们全部释放：
```
TreeMaker::~TreeMaker(void) {
	// release all Counter's memory
	for (auto& counter : this->trashbin)
		delete counter;
	this->trashbin.clear();
}
```
### `complex` 转换器
在 `TreeMaker` 类中有一个 `complex` 转换器，负责将得到的 `Prediction` 信息转换成 `Node` 节点，它的实现如下：
```
Node* TreeMaker::complex(iCard*& card, iPlayer*& player) {
	// This function use card to create Node in ***heap***
	double rank, possibility;
	if (card != nullptr) {
		rank = player->rank(card) * global::rank_factor;
		possibility = player->posibility(card) * global::possibility_factor;
	} else {
		rank = 0.0;
		possibility = 0.0;
	}
	Data data(card, rank, possibility);
	Node* node = new Node(data);
	return node;
}
```
这个接口调用了对指定玩家的指定卡片的权重和可能性进行了计算，并在堆内存中创建一个新的 `Node` 节点，将其计算的权重，可能性，进行保存并返回。
通过调用 `TreeMaker` 类的 `make` 接口，我们可以生成一颗关于当前操作的所有分支预测树，接下来我们对其进行分析。
![predction](https://github.com/guiqiqi/DurakRobot/raw/master/images/predction.png)

## 第八部分 - 分析树
刚刚我们通过 `TreeMaker` 类对当前局势进行了预测，生成了一颗包含所有可能分支的多叉预测树。现在我们通过对这颗树进行分析，得到最后要进行操作的手牌。
下面是分析类的定义：
```
class Thinker {
private:
	iPlayer* we;
	iPlayer* enemy;
	Counter<global::players>* situation;

private:
	Tree* maketree(bool status, iCard* card);
	static double ranksum(bool starting, std::stack<Node*>& path);
	static iCard* byrank(iPlayer*& player);
	static void showpath(std::stack<Node*> path, double rank);

public:
	Thinker(iPlayer* we, iPlayer* enemy,
		Counter<global::players>* situation);

public:
	iCard* attack(void);
	iCard* attack(int);
	iCard* defend(iCard* action);
};
```
私有函数的解释：
- `maketree(bool status, iCard* card)` - 针对给定的操作卡片和出事进攻状态调用 `TreeMaker` 生成一棵树并返回
- `static double ranksum(bool starting, std::stack<Node*>& path)` - 针对给定的初始状态和给定的路径计算路径的总和（下文详述）
- `static iCard* byrank(iPlayer*& player)` - 仅仅根据所有可能卡片的权重给出最优选择，而不进行预测
- `static void showpath(std::stack<Node*> path, double rank)` - 打印出给定的路径和节点权重总和

### `ranksum` 函数
其实现如下：
```
/* 
	Calculate the weight of the path and 
	the sum of the probabilities;
	The probability value is in the interval[0, 1],
	multiplied by the weight, and the path sum is obtained.

	Record our weight loss as a negative number and 
	the enemy's record as a positive number. 
	The larger the calculated result value, the better.
*/
double Thinker::ranksum(bool starting, std::stack<Node*>& opath) {
	// make a copy for path
	double sum = 0; bool status = starting;
	std::stack<Node*> path(opath);

	while (!path.empty()) {
		Node* node = path.top();
		Data* data = node->get();
		double loss = data->possibility * data->rank;

		if (status)
			sum -= loss;
		else
			sum += loss;

		path.pop();
		status = !status;
	}
	
	return sum;
}
```
可以看到：该函数计算路径的权重和概率的总和：其概率值在区间 `[0, 1]` 中，将该值乘以权重，并对所有节点依次求和获得总路径和。
而当初始状态为进攻时：将我们的权重记为负值，用来表示我们为此次进攻丢失的权重；而将敌人的权重记为正值，表示敌人为了防守我们的卡牌所丢失的权重。这样该函数返回的值就可以表示该路径对我们的利弊，其值越大，对我方越有利。

### 进攻与防守
类中定义了三个进攻与防守接口：
```
iCard* attack(void);
iCard* attack(int);
iCard* defend(iCard* action);
```
我们的思路如下：
当我们是进攻轮且桌面上没有卡片时不进行预测，仅根据权重打出最小的一张牌：因为此时的预测可能性太多了，分支预测的准确度会下降；
当我们是进攻论且桌面上有卡片时进行预测，并给出总权重和最大的一个，此时对我方最有利；
当我们是防守轮时根据对方的进攻卡片进行预测，得出的权重最小值对我方最有利；
其接口实现如下：
```
iCard* Thinker::attack(void) {
	return this->byrank(this->we); 
}

iCard* Thinker::attack(int) {
	// record all ways could be
	std::map<double, iCard*> record;

	std::vector<iCard*> list = this->we->analysis();
	// if no cards can be play, return nullptr
	if (list.empty()) return nullptr;

	for (auto& card : list) {
		Tree* tree = this->maketree(global::attack, card);
		Tree::Parser parser = tree->leaves();

		while (parser.status()) {
			std::stack<Node*> path = parser.yield();
			if (path.empty()) continue;

			double rank = this->ranksum(global::attack, path);
			this->showpath(path, rank);
			record[rank] = path.top()->get()->card;
		}

		delete tree;
	}

	// Return the branch with the highest weight
	auto it = record.end(); it--;
	return it->second;
}
```
这里使用了 `std::map` 来存储权重值与牌的对应关系，因为`map`的本质是一颗红黑树，其最小值存储在根部，最大值存储在叶子节点；分别使用 `std::map<double, iCard*>::begin()/end()`即可获得。

## 第九部分 - 其他
### 卡牌转换器
我们在内部实现的 `iCard` 类需要和 `Card` 类实现相互转换，于是在 `translator.h` 中定义了如下静态函数：
```
// translate card* to icard*
static iCard* icard(CardManager* manager, const Card* card) {
	if ((card == Dealer::GetNocard())
		||
		(card == Dealer::GetPas())
		||
		(card == nullptr)
		) {
		return nullptr;
	}
	std::string suit = Dealer::SuitName(card);
	std::string rank = Dealer::RankName(card);
	iCard* target = manager->get(suit, rank);
	return target;
}
```
该函数通过查询 `Card` 类的`suit`和`rank`属性，在`CardManager`中寻找对应的 `iCard` 指针并返回。

### `Player` 类
玩家类的定义如下：
```
class Player : public PlayerAbstract {

private:
	// Mapping table between external card and internal card
	iPlayer* weinenemy;
	std::unordered_map<iCard*, Card*> cardmap;
	bool ending = false;

protected:
	iCard* trump = nullptr;
	Rule* analyser = nullptr;
	Ranker* ranker = nullptr;
	CardManager* manager = nullptr;
	Counter<global::players>* counter = nullptr;
	iPlayer* enemy = nullptr;
	Thinker* thinker = nullptr;

	iPlayer* we;
	
public:
	Player(const char* name, CardManager* manager);
	~Player(void);
	void YouTurn(bool);
	void PutCard(void);
	void TakeCards(void);
	void GetHeadTrick(void);
	void TakeOneCard(Card*& nc);
	void ShowCards(void);
	bool INeedCard(void);
	int GetCardNum(void);

public:
	void setwe(iPlayer* enemy); // Get our player's address
	iPlayer* getenemy(void) const; // Set the address of the imaginary enemy
};
```
在每一个玩家类中都生成了两个 `iPlayer` 对象，一个存储为 `we`，另外一个存储为`enemy`；这分别是玩家自己和对应的假想敌。
而在进行操作时（`TakeCards`, `GetHeadTrick`, `TakeOneCard`, `PutCard`），我们需要通知对方玩家中的假想敌更新相应的状态，由此实现两边的相互预测。
因此，在类中还需要保存一个 `iPlayer* weinenemy` 对象，每当我们进行资源改变的相关操作时，需要对该对象同时进行：
```
// Take one card
void Player::TakeOneCard(Card*& nc) {
	iCard* target = icard(this->manager, nc);

	// While there is no card for us
	if (target == nullptr) {
		this->ending = true;
		return;
	}

	this->cardmap[target] = nc;
	this->we->get(target);

	// Notify enemy that we get one card
	this->weinenemy->replenish();
}
```
最后一行的 `this->weinenemy->replenish()` 就是通知对方的假想敌我们拿到了手牌，却并不告诉对方我们拿到了什么样的牌。

`Player` 类中的构造函数需要传入一个公共的 `CardManager` 类，这是因为所有的 `iCard*` 对象的地址时公共的信息，不需要成为类的私有成员；当在外部初始化类之后传入类对象可以节省内存。

`void Player::set(iPlayer* enemy)` - 调用该接口可以在己方设置对方的假想敌
`iPlayer* getenemy(void) const` - 调用该接口则可以返回己方的假想敌，供对方通知使用

由于在类的构造函数中在堆内存中初始化了所有的组件，在类的析构函数中需要将其释放：
```
Player::~Player(void) {
	// Release all created resources
	delete this->analyser;
	delete this->ranker;
	delete this->counter;
	delete this->we;
	delete this->enemy;
	delete this->thinker;
}
```

另外，当牌堆的中的牌已经发完了之后，如果仍然声明 `INeedCard` 则会持续的给对象发出 `Dealer::GetPas()` 中的牌，这在内部会造成错误。我们需要设置一个标志位(`Player::ending`)，当发现已经没有牌可以给我们发的时候声明不再需要牌：
```
// Return if we need cards
bool Player::INeedCard(void) {
	// If there is no card for us - ending status
	if (ending) return false;

	if (this->we->total() < global::fullcard)
		return true;
	return false;
}
...
// Take one card
void Player::TakeOneCard(Card*& nc) {
	iCard* target = icard(this->manager, nc);

	// While there is no card for us
	if (target == nullptr) {
		this->ending = true;
		return;
	}

	this->cardmap[target] = nc;
	this->we->get(target);

	// Notify enemy that we get one card
	this->weinenemy->replenish();
}
```
而在初始化游戏时，由于上述的特性，我们需要多进行两步操作用以初始化 `CardManager` 与设置假想敌：
```
Card* trump = Dealer::GetTrump();
CardManager manager;
std::cout << "Trump: " << *icard(&manager, trump) << std::endl;
Card* temp;
Player p1("Konstantin", &manager);
Player p2("Grigory", &manager);
	
p1.setwe(p2.getenemy()); 
p2.setwe(p1.getenemy());
```

## 总结
根据以上的算法，我们可以动态的分析当前的局势，从而给出一个相对合理的攻击/防守方案；而通过调节 `header.h` 中的设置参数，我们可以动态的更改分析器的行为。
下面给出预测深度为 `3` 时，100局自对抗的结果及预测部分截图：
![processing](https://github.com/guiqiqi/DurakRobot/raw/master/images/processing.png)
对抗结果：
![result](https://github.com/guiqiqi/DurakRobot/raw/master/images/result.png)
