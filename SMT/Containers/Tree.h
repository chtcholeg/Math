#ifndef __TREE_H__
#define __TREE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree (STL doesn't contain similar type of containers)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <vector>

namespace SMT
{

// Base class for tree node
class TreeNode : public std::enable_shared_from_this<TreeNode>
{
public:
	using SharedPtr = std::shared_ptr<ResultTreeNode>;
	using WeakPtr = std::weak_ptr<ResultTreeNode>;
	static const size_t NotFound = static_cast<size_t>(-1);

	// Payload data
	class Data {
	public:
		using namespace SharedPtr = std::shared_ptr<Data>;
		virtual ~Data() {}
		TreeNode::SharedPtr Owner() const { return owner_.lock(); }
		
		// -- Copying
		virtual bool CanDuplicate() const { return false; }
		virtual Data::SharePtr Duplicate() const { return nullptr; }
		
	private:
		TreeNode::WeakPtr owner_;	// A node which owns this data. This value is changed by TreeNode-class and mustn't be changed wherever else.
		friend class TreeNode;
	};

public:
	static TreeNode::SharedPtr Create(const Data::SharedPtr& data = nullptr) 
	{ 
		auto result = std::make_shared<TreeNode>(data);
		result->ResetData(data);
		return result;
	}
	virtual ~TreeNode() {}

	// -- Payload data
	Data::SharedPtr Data() const { return data_; }
	void ResetData(const Data::SharedPtr& data) 
	{
		auto currentOwner = data_.owner_.lock();
		if (currentOwner != nullptr)
		{
			currentOwner->data_.reset();
		}
		data_ = data;
		data_.owner_ = shared_from_this();
	}

	// -- Parent
	TreeNode::SharedPtr Parent() { return parent_.lock(); }
	const TreeNode::SharedPtr Parent() const { return const_cast<TreeNode*>(this)->Parent(); }

	// -- Children
	std::vector<SharedPtr> Children() const { children_; }
	size_t FindNodeIndex(const TreeNode::SharedPtr& child)
	{
		for (size_t i = 0; i < children_.size(); ++i)
		{
			if (children_[i] == child)
			{
				return i;
			}
		}
		return NotFound;
	}

	void AddChild(TreeNode::SharedPtr& child)
	{
		if (child == nullptr) 
		{
			return;
		}
		
		// Remove the child from the previous parent
		{
			auto currentParent = child->parent_.lock();
			if (currentParent.get() == this)
			{
				return;
			}
			if (currentParent != nullptr)
			{
				currentParent->DeleteChild(child);
			}
		}
		
		// Add to current parent
		children_.push_back(child);
		child.parent_ = shared_from_this();
	}
	void DeleteChild(TreeNode::SharedPtr& child)
	{
		const size_t index = FindNodeIndex(child);
		if (index == NotFound)
		{
			return;
		}
		assert(index < children_.size());
		if (index >= children_.size())
		{
			return;
		}
		children_.erase(children_.begin() + index);
		if (child != nullptr)
		{
			child->parent_.reset();
		}
	}

	// -- Copying
	bool CanDuplicate() const
	{
		return canDuplicate_Recursively(*this);
	}
	
	TreeNode::SharePtr Duplicate() const
	{
		return duplicate_Recursively(*this);
	}

private:
	Data::SharedPtr data_;
	WeakPtr parent_;
	std::vector<SharedPtr> children_;

	explicit TreeNode(const Data::SharedPtr& data) : data_(data) {}
	TreeNode(const TreeNode&) = deleted;
	
	static bool canDuplicate_Recursively(const TreeNode& node)
	{
		if ( (node.data_ != nullptr) && !node.data_->CanDuplicate())
		{
			return false;
		}
		
		for (const auto& child : children_)
		{
			if (child == nullptr)
			{
				continue;
			}
			if (!canDuplicate_Recursively(*child))
			{
				return false;
			}
		}
		return true;
	}
	
	static TreeNode::SharePtr duplicate(const TreeNode& node)
	{
		return Create((node.data_ == nullptr) ? nullptr : node.data_->Duplicate());
	}
	
	static TreeNode::SharePtr duplicate_Recursively(const TreeNode& root)
	{
		TreeNode::SharePtr newRoot = duplicate(root);
		for (const auto& child : root.children_)
		{
			TreeNode::SharePtr newChild = (child == nullptr) ? nullptr : duplicate(*child);
			newRoot->AddChild(newChild);
		}
		return newRoot;
	}
};

} // namespace SMT

#endif // __TREE_H__