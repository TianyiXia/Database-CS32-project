#ifndef MULTIMAP_INCLUDED
#define MULTIMAP_INCLUDED
#include <string>

class MultiMap
{
	struct TreeNode;
	struct ListNode;
public:
	class Iterator
	{
	public:
		Iterator();
		Iterator(TreeNode* keyAddress, ListNode* valueAddress);
		bool valid() const;
		std::string getKey() const;
		unsigned int getValue() const;
		bool next();
		bool prev();
	private:
		TreeNode* treePtr;
		ListNode* listPtr;
		bool isValid;
	};
	 MultiMap();
	 ~MultiMap();
	 void clear();
	 void insert(std::string key, unsigned int value);
     Iterator findEqual(std::string key) const;
	 Iterator findEqualOrSuccessor(std::string key) const;
	 Iterator findEqualOrPredecessor(std::string key) const;

private:
	/******************Node***************************************/
	struct ListNode
	{
		ListNode(int value);
		int m_val;
		ListNode* m_next;
		ListNode* m_prev;
	};

	struct TreeNode
	{
		//tree node constructor
		TreeNode(std::string key, unsigned int value);
		//helper: add value to existing node,only call this when head is already there
		void addValue(int value);
		std::string m_keyName;
		TreeNode* left;
		TreeNode* right;
		TreeNode* parent;
		int valueNum;
		ListNode* m_head;
	};
	/*******************************************************************************/
	TreeNode* m_root;	
	MultiMap(const MultiMap& other);
	MultiMap& operator=(const MultiMap& rhs);

	//helper: 
	void freeAll(TreeNode* curr);
	Iterator getMin() const;
	Iterator getMax() const;
};

#endif