#pragma once
#ifndef KIGU_NODE_H
#define KIGU_NODE_H
#include "common.h"
#include "unicode.h"

//// double linked list node ////
struct Node{
	Node* next;
	Node* prev;
};
#define NodeInsertNext(x,node) ((node)->next=(x)->next,(node)->prev=(x),(node)->next->prev=(node),(x)->next=(node))
#define NodeInsertPrev(x,node) ((node)->prev=(x)->prev,(node)->next=(x),(node)->prev->next=(node),(x)->prev=(node))
#define NodeRemove(node) ((node)->next->prev=(node)->prev,(node)->prev->next=(node)->next)

//// tree node ////
struct TNode {
	Type  type;
	Flags flags;
	
	TNode* next = 0;
	TNode* prev = 0;
	TNode* parent = 0;
	TNode* first_child = 0;
	TNode* last_child = 0;
	u32    child_count = 0;
	
#if BUILD_INTERNAL
	str8 debug;
#endif
};

#define for_node(node) for(auto it = node; it != 0; it = it->next)
#define for_nodeX(id, node) for(auto id = node; id != 0; id = id->next)
#define for_node_reverse(node) for(auto it = node; it != 0; it = it->prev)

global inline void insert_after(TNode* target, TNode* node) {
	if (target->next) target->next->prev = node;
	node->next = target->next;
	node->prev = target;
	target->next = node;
}

global inline void insert_before(TNode* target, TNode* node) {
	if (target->prev) target->prev->next = node;
	node->prev = target->prev;
	node->next = target;
	target->prev = node;
}

global inline void remove_horizontally(TNode* node) {
	if (node->next) node->next->prev = node->prev;
	if (node->prev) node->prev->next = node->next;
	node->next = node->prev = 0;
}

global void insert_last(TNode* parent, TNode* child) {
	if (parent == 0) { child->parent = 0; return; }
	if(parent==child){DebugBreakpoint;}
	
	child->parent = parent;
	if (parent->first_child) {
		insert_after(parent->last_child, child);
		parent->last_child = child;
	}
	else {
		parent->first_child = child;
		parent->last_child = child;
	}
	parent->child_count++;
}

global void insert_first(TNode* parent, TNode* child) {
	if (parent == 0) { child->parent = 0; return; }
	
	child->parent = parent;
	if (parent->first_child) {
		insert_before(parent->first_child, child);
		parent->first_child = child;
	}
	else {
		parent->first_child = child;
		parent->last_child = child;
	}
	parent->child_count++;
}

global void change_parent(TNode* new_parent, TNode* node) {
	//if old parent, remove self from it 
	if (node->parent) {
		if (node->parent->child_count > 1) {
			if (node == node->parent->first_child) node->parent->first_child = node->next;
			if (node == node->parent->last_child)  node->parent->last_child = node->prev;
		}
		else {
			Assert(node == node->parent->first_child && node == node->parent->last_child, "if node is the only child node, it should be both the first and last child nodes");
			node->parent->first_child = 0;
			node->parent->last_child = 0;
		}
		node->parent->child_count--;
	}
	
	//remove self horizontally
	remove_horizontally(node);
	
	//add self to new parent
	insert_last(new_parent, node);
}

global void move_to_parent_first(TNode* node){
	if(!node->parent) return;
	
	TNode* parent = node->parent;
	if(parent->first_child == node) return;
	if(parent->last_child == node) parent->last_child = node->prev;

	remove_horizontally(node);
	node->next = parent->first_child;
	parent->first_child->prev = node;
	parent->first_child = node;
}

global void move_to_parent_last(TNode* node){
	if(!node->parent) return;
	
	TNode* parent = node->parent;
	if(parent->last_child == node) return;
	if(parent->first_child == node) parent->first_child = node->next;

	remove_horizontally(node);
	node->prev = parent->last_child;
	parent->last_child->next = node;
	parent->last_child = node;
}

global void remove(TNode* node) {
	//add children to parent (and remove self from children)
	for(TNode* it = node->first_child; it != 0; ) {
		TNode* next = it->next;
		change_parent(node->parent, it);
		it = next;
	}
	
	//remove self from parent
	if (node->parent) {
		if (node->parent->child_count > 1) {
			if (node == node->parent->first_child) node->parent->first_child = node->next;
			if (node == node->parent->last_child)  node->parent->last_child = node->prev;
		}
		else {
			Assert(node == node->parent->first_child && node == node->parent->last_child, "if node is the only child node, it should be both the first and last child nodes");
			node->parent->first_child = 0;
			node->parent->last_child = 0;
		}
		node->parent->child_count--;
	}
	node->parent = 0;
	
	//remove self horizontally
	remove_horizontally(node);
}

#endif //KIGU_NODE_H
