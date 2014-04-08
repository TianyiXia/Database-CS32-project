#include "MultiMap.h"
#include <string>
using namespace std;
/************************Node***********************************************/
MultiMap::ListNode::ListNode(int value)
{
	m_val=value;
	m_next=m_prev=NULL;
}
MultiMap::TreeNode::TreeNode(std::string key, unsigned int value)
{
	m_keyName=key;
	parent=left=right=NULL;
	valueNum=1;
	m_head=new ListNode(value);
}

void MultiMap::TreeNode::addValue(int value)
{
	ListNode* temp=m_head;
	while(temp->m_next!=NULL)
	{
		temp=temp->m_next;
	}
	ListNode* toBeAdd=new ListNode(value);
	toBeAdd->m_prev=temp;
	temp->m_next=toBeAdd;
	valueNum++;
}
/************************Node***********************************************/



MultiMap::MultiMap()
{
	m_root=NULL;
}

MultiMap:: ~MultiMap()
{
	clear();
}

void MultiMap::clear()
{
	freeAll(m_root);
	m_root=NULL;
}

void MultiMap::freeAll(TreeNode* curr)
{
	if(curr==NULL)
		return;
	freeAll(curr->left);
	freeAll(curr->right);

	ListNode* currVal=curr->m_head;
	while(currVal!=NULL)
	{
		ListNode* toBeKill=currVal;
		currVal=currVal->m_next;
		delete toBeKill;
	}
	delete curr;
	//notice this function doest not set root to null, this is done in clear function
}
void MultiMap::insert(std::string key, unsigned int value)
{
	if(m_root==NULL)
	{
		m_root=new TreeNode(key,value);//for root its head is NULL by default
		return;
	}
	
	TreeNode* curr=m_root;
	for(;;)
	{
		if(curr->m_keyName==key)
		{
			curr->addValue(value);
			return;
		}
		else if (key < curr->m_keyName)
		{
			if (curr->left != nullptr)
			{
				curr = curr->left;
			}
			else
			{
				curr->left = new TreeNode(key, value);
				curr->left->parent=curr;//remember its parent
				return;
			}
		}
		else if (key > curr->m_keyName)
		{
			if (curr->right != nullptr)
			{
				curr = curr->right;
			}
			else
			{
				curr->right = new TreeNode(key, value);
				curr->right->parent=curr;
				return;
			}
		}		
	}
}




MultiMap::Iterator MultiMap::findEqual(std::string key) const
{
	TreeNode* curr=m_root;
	while(curr!=NULL)
	{
		if(curr->m_keyName==key)
		{
			return Iterator(curr,curr->m_head);
		}
		else if(curr->m_keyName<key)
		{
			curr=curr->right;
		}
		else
		{
			curr=curr->left;
		}
	}
	return Iterator();
}

MultiMap::Iterator MultiMap:: findEqualOrSuccessor(std::string key) const
{
	TreeNode* curr=m_root;
	while(curr!=NULL)
	{
		if(curr->m_keyName==key)
		{
			return Iterator(curr,curr->m_head);
		}
		else if(curr->m_keyName<key)// we need to move to a bigger node
		{
			if(curr->right==NULL)
			{
				//we cannot move to right any more, we can use next()
				//to find the next bigger node, we skip all the duplicate
				//of current node first
				string currKey=curr->m_keyName;
				Iterator it=Iterator(curr,curr->m_head);
				while(it.getKey()==currKey)
				{
					it.next();
					if(!it.valid())//biggest node is smaller than key
						break;
				}
				return it;
			}
			curr=curr->right;
		}
		else//our node is bigger than given key and should be the closest
		{
			if(curr->left==NULL)
			{
			return Iterator(curr,curr->m_head);
			}
			curr=curr->left;
		}
	}
	  //to be tested: 1)curr<key && curr has duplicate 2)curr>key && curr has duplicate
	//try each treenode has duplicates
	//try tree with just one node
		return Iterator();//?????????????

}

	 
MultiMap::Iterator MultiMap:: findEqualOrPredecessor(std::string key) const//?????????????????????????
{
		TreeNode* curr=m_root;
	while(curr!=NULL)
	{
		if(curr->m_keyName==key)
		{
			return Iterator(curr,curr->m_head);
		}
		else if(curr->m_keyName<key)
		{
			if(curr->right==NULL)
				break;
			curr=curr->right;
		}
		else
		{
			if(curr->left==NULL)
				break;
			curr=curr->left;
		}
	}
	Iterator nearest= Iterator(curr,curr->m_head);
	if (nearest.getKey()<key)
		return (nearest);
	else
	{
		string currKey=curr->m_keyName;
		Iterator it=Iterator(curr,curr->m_head);
		while(it.getKey()==currKey)
		{
			it.prev();
			if(!it.valid())
				break;
		}		
		return it;
	}
}


MultiMap::Iterator::Iterator()
{
	isValid=false;
}
MultiMap::Iterator::Iterator(TreeNode* keyAddress, ListNode* valueAddress)
{
	treePtr=keyAddress;
	listPtr=valueAddress;
	isValid=true;
}
bool MultiMap::Iterator::valid() const
{
	return isValid;
}

std::string MultiMap::Iterator::getKey() const
{
	return treePtr->m_keyName;
}

unsigned int MultiMap::Iterator::getValue() const
{
	return listPtr->m_val;
}

 bool MultiMap::Iterator::next()
 {
	 if(!valid())
		 return false;
	 //see if there are other values with same key
	 if(listPtr->m_next!=NULL)
	 {
		 listPtr=listPtr->m_next;
		 return true;
	 }
	 //see if there is a right sub node
	 if(treePtr->right!=NULL)
	 {
		 //find the smallest(left most node) of the sub tree using its right subnode as root
		 treePtr=treePtr->right;
		 while(treePtr->left!=NULL)
		 {
			 treePtr=treePtr->left;
		 }
		 listPtr=treePtr->m_head;
		 return true;
	 }
	 //reach here means no right subtree 
	 //check if there is parentnode
	 while(treePtr->parent!=NULL)
	 {
		 if(treePtr->parent->left==treePtr)//curent node is the left subnode,meaniing parent is bigger
		 {
			 treePtr=treePtr->parent;
			 listPtr=treePtr->m_head;
			 return true;
		 }
		 //curr node is a right sub node, the go up one level and repeat
		 treePtr=treePtr->parent;
		 listPtr=treePtr->m_head;
	 }
	 //meanning last node
	 isValid=false;
	 return false;
 }

 bool MultiMap::Iterator::prev()
 {
	 if(!valid())
		 return false;
	 //see if we've at the head of linked list
	 if(listPtr->m_prev!=NULL)
	 {
		 listPtr=listPtr->m_prev;
		 return true;
	 }

	 //see if there is a left sub node
	 if(treePtr->left!=NULL)
	 {
		 //we want to biggest(right most) value in the left sub tree
		 treePtr=treePtr->left;
		 while(treePtr->right!=NULL)
			 treePtr=treePtr->right;
		 //!notice that we want the last listnode of linkedlsit
		 ListNode* tail=treePtr->m_head;
		 while(tail->m_next!=NULL)
			 tail=tail->m_next;
		 listPtr=tail;
		 return true;
	 }
	 //reach here means no left subtree 
	 //check if there is parentnode
	 while(treePtr->parent!=NULL)
	 {
		 if(treePtr->parent->right==treePtr)//curent node is the right subnode,meaniing parent is smaller
		 {
			 treePtr=treePtr->parent;
			 ListNode* tail=treePtr->m_head;
			 while(tail->m_next!=NULL)
				 tail=tail->m_next;
			 listPtr=tail;			 
			 return true;
		 }
		 treePtr=treePtr->parent;
	 }
	 //meanning first node
	 isValid=false;
	 return false;
 }

 MultiMap::Iterator MultiMap::getMin() const
 {
	 TreeNode* curr=m_root;
	 while(curr->left!=NULL)
		 curr=curr->left;
	 return Iterator(curr,curr->m_head);
 }

  MultiMap::Iterator MultiMap::getMax() const
 {
	 TreeNode* curr=m_root;
	 while(curr->right!=NULL)
		 curr=curr->right;
	 ListNode* temp=curr->m_head;
	 while(temp->m_next!=NULL)
		 temp=temp->m_next;
	 return Iterator(curr,temp);
 }


