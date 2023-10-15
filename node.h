#pragma once
#ifndef KIGU_NODE_H
#define KIGU_NODE_H
#include "common.h"

//// double linked list node ////
struct Node{
	Node* next;
	Node* prev;
};

#define NodeInsertNext(x,new_node) ((new_node)->next=(x)->next,(new_node)->prev=(x),(new_node)->next->prev=(new_node),(x)->next=(new_node))
#define NodeInsertPrev(x,new_node) ((new_node)->prev=(x)->prev,(new_node)->next=(x),(new_node)->prev->next=(new_node),(x)->prev=(new_node))
#define NodeRemove(node) ((node)->next->prev=(node)->prev,(node)->prev->next=(node)->next)

//// tree node ////
struct TNode {
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

// replaces the node 'below' with 'above' and inserts
// 'below' into 'above's children
global void 
insert_above(TNode* below, TNode* above) {
	if(!below->parent) {
		change_parent(above, below);
		return;
	}
	TNode copy = *below;
	copy.parent->child_count++;
	change_parent(0, below);

	if(copy.parent){
		above->parent = copy.parent;
		
		if(copy.next && copy.prev){
			insert_after(copy.prev, above);
		}else if( copy.next && !copy.prev){
			insert_before(copy.next, above);
			copy.parent->first_child = above;
		}else if(!copy.next &&  copy.prev){
			insert_after(copy.prev, above);
			copy.parent->last_child = above;
		}else{
			copy.parent->first_child = copy.parent->last_child = above;
		}
	}

	change_parent(above, below);
}

global void
replace(TNode* with, TNode* me) {
	TNode* my = me;
	TNode* their = with;

	*their = *my;

	for(TNode* n = my->first_child; n; n = n->next) {
		n->parent = their;
	}

	if(my->parent) {
		if(my->parent->first_child == my) {
			my->parent->first_child = their;
		} 
		if(my->parent->last_child == my) {
			my->parent->last_child = their;
		}
	}

	if(my->prev) my->prev->next = their;
	if(my->next) my->next->prev = their;
	
	*me = {0};
}

#endif //KIGU_NODE_H
