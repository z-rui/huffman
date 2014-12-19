// important: compile with C++11 standard

#include <functional>	// std::function
#include <iostream>	// std::cout, std::endl
#include <memory>	// std::shared_ptr, std::make_shared
#include <map>		// std::map
#include <queue>	// std::priority_queue
#include <string>	// std::string
#include <vector>	// std::vector
#include <climits>	// CHAR_BIT

template <class T>
class HuffmanEncoder {
public:
	struct Stat {
		size_t freq;
		std::string code;
	};
	class Node;
	class Leaf;
	class NonLeaf;
protected:
	typedef std::shared_ptr<Node> NodePtr;
	std::map<T, NodePtr> leafref;
	std::map<T, Stat> stats;
	std::priority_queue<NodePtr, std::vector<NodePtr>, std::function<bool(NodePtr, NodePtr)> > heap;
public:
	HuffmanEncoder() : heap([] (NodePtr x, NodePtr y) {return x->freq > y->freq;}) {}
	void read(T);
	void encode();
	void reset() { leafref.clear(); stats.clear(); heap.clear(); };
	const std::map<T, Stat>& getstats() const { return stats; }
};


template <class T>
void HuffmanEncoder<T>::read(T val)
{
	auto& ref = leafref[val];
	if (ref == nullptr) {
		auto node = std::make_shared<Leaf> (val);
		ref = node;
	}
	ref->freq++;
}

template <class T>
void HuffmanEncoder<T>::encode()
{
	stats.clear();
	for (auto it = leafref.begin(); it != leafref.end(); it++) {
		heap.push(it->second);
	}
	if (heap.size() == 0) // no input
		return;
	while (heap.size() > 1) {
		auto a = heap.top();
		heap.pop();
		auto b = heap.top();
		heap.pop();
		auto c = std::make_shared<NonLeaf> (a, b);
		heap.push(c);
	}
	heap.top()->encode_recursive(stats, "");
}

template <class T>
class HuffmanEncoder<T>::Node {
public:
	size_t freq;
	Node(size_t freq = 0) : freq(freq) {}
	virtual void encode_recursive(std::map<T, Stat>& stats, const std::string&) = 0;
};

template <class T>
class HuffmanEncoder<T>::Leaf : public Node {
public:
	T val;
	Leaf(T val) : val(val) {}
	void encode_recursive(std::map<T, Stat>& stats, const std::string& prefix)
	{
		stats[val].freq = Node::freq;
		stats[val].code = prefix;
	}
};

template <class T>
class HuffmanEncoder<T>::NonLeaf: public Node {
public:
	NodePtr left, right;
	NonLeaf(NodePtr left, NodePtr right) : Node(left->freq + right->freq), left(left), right(right) {}
	void encode_recursive(std::map<T, Stat>& stats, const std::string& prefix)
	{
		left->encode_recursive(stats, prefix + "0");
		right->encode_recursive(stats, prefix + "1");
	}
};

int main()
{
	HuffmanEncoder<char> enc;
	size_t bitcnt0 = 0, bitcnt1 = 0;

	for (char ch; std::cin >> ch; ) {
		enc.read(ch);
	}
	enc.encode();
	auto& stats = enc.getstats();
	for (auto it = stats.begin(); it != stats.end(); it++) {
		std::cout << it->first << '\t' << it->second.freq << '\t' << it->second.code << std::endl;
		bitcnt0 += it->second.freq * CHAR_BIT;
		bitcnt1 += it->second.freq * it->second.code.length();
	}
	std::cout << "uncompressed = " << bitcnt0 << std::endl;
	std::cout << "compressed = " << bitcnt1 << std::endl;
	std::cout << "ratio = " << (double) bitcnt1 / bitcnt0 * 100 << "%" << std::endl;
}
