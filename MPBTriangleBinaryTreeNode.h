#ifndef __TRIANGLEBINARYTREENODE_H__
#define __TRIANGLEBINARYTREENODE_H__


class MPBTriangleBinaryTreeNode
{

public:

	MPBTriangleBinaryTreeNode();

	MPBTriangleBinaryTreeNode*		m_leftChild;
	MPBTriangleBinaryTreeNode*		m_rightChild;
	MPBTriangleBinaryTreeNode*		m_baseNeighbor;
	MPBTriangleBinaryTreeNode*		m_leftNeighbor;
	MPBTriangleBinaryTreeNode*		m_rightNeighbor;
};



#endif

