/* maxflow.cpp */


#include <stdio.h>
#include "graph.h"


/*
	special constants for node->parent
*/
#define TERMINAL ( (arc *) 1 )		/* to terminal */
#define ORPHAN   ( (arc *) 2 )		/* orphan */


#define INFINITE_D ((int)(((unsigned)-1)/2))		/* infinite distance to the terminal */

/***********************************************************************/

/*
	Functions for processing active list.
	i->next points to the next node in the list
	(or to i, if i is the last node in the list).
	If i->next is NULL iff i is not in the list.

	There are two queues. Active nodes are added
	to the end of the second queue and read from
	the front of the first queue. If the first queue
	is empty, it is replaced by the second queue
	(and the second queue becomes empty).
*/


template <typename captype, typename tcaptype, typename flowtype> 
	inline void Graph<captype,tcaptype,flowtype>::set_active(node *i)
{
	if (!i->next)
	{
		/* it's not in the list yet */
		if (queue_last[1]) queue_last[1] -> next = i;
		else               queue_first[1]        = i;
		queue_last[1] = i;
		i -> next = i;
	}
}

/*
	Returns the next active node.
	If it is connected to the sink, it stays in the list,
	otherwise it is removed from the list
*/
template <typename captype, typename tcaptype, typename flowtype> 
	inline typename Graph<captype,tcaptype,flowtype>::node* Graph<captype,tcaptype,flowtype>::next_active()
{
	node *i;

	while ( 1 )
	{
		if (!(i=queue_first[0]))
		{
			queue_first[0] = i = queue_first[1];
			queue_last[0]  = queue_last[1];
			queue_first[1] = NULL;
			queue_last[1]  = NULL;
			if (!i) return NULL;
		}

		/* remove it from the active list */
		if (i->next == i) queue_first[0] = queue_last[0] = NULL;
		else              queue_first[0] = i -> next;
		i -> next = NULL;

		/* a node in the list is active iff it has a parent */
		if (i->parent) return i;
	}
}

/***********************************************************************/

template <typename captype, typename tcaptype, typename flowtype> 
	inline void Graph<captype,tcaptype,flowtype>::set_orphan_front(node *i)
{
	nodeptr *np;
	i -> parent = ORPHAN;
	np = nodeptr_block -> New();
	np -> ptr = i;
	np -> next = orphan_first;
	orphan_first = np;
}

template <typename captype, typename tcaptype, typename flowtype> 
	inline void Graph<captype,tcaptype,flowtype>::set_orphan_rear(node *i)
{
	nodeptr *np;
	i -> parent = ORPHAN;
	np = nodeptr_block -> New();
	np -> ptr = i;
	if (orphan_last) orphan_last -> next = np;
	else             orphan_first        = np;
	orphan_last = np;
	np -> next = NULL;
}

/***********************************************************************/

template <typename captype, typename tcaptype, typename flowtype> 
	inline void Graph<captype,tcaptype,flowtype>::add_to_changed_list(node *i)
{
	if (changed_list && !i->is_in_changed_list)
	{
		node_id* ptr = changed_list->New();
		*ptr = (node_id)(i - nodes);
		i->is_in_changed_list = true;
	}
}

/***********************************************************************/

template <typename captype, typename tcaptype, typename flowtype> 
	void Graph<captype,tcaptype,flowtype>::maxflow_init()
{
	node *i;

	queue_first[0] = queue_last[0] = NULL;
	queue_first[1] = queue_last[1] = NULL;
	orphan_first = NULL;

	TIME = 0;

	for (i=nodes; i<node_last; i++)
	{
		i -> next = NULL;
		i -> is_marked = 0;
		i -> is_in_changed_list = 0;
		i -> TS = TIME;
		if (i->tr_cap > 0)
		{
			/* i is connected to the source */
			i -> is_sink = 0;
			i -> parent = TERMINAL;
			set_active(i);
			i -> DIST = 1;
		}
		else if (i->tr_cap < 0)
		{
			/* i is connected to the sink */
			i -> is_sink = 1;
			i -> parent = TERMINAL;
			set_active(i);
			i -> DIST = 1;
		}
		else
		{
			i -> parent = NULL;
		}
	}
}

template <typename captype, typename tcaptype, typename flowtype> 
	void Graph<captype,tcaptype,flowtype>::maxflow_reuse_trees_init()
{
	node* i;
	node* j;
	node* queue = queue_first[1]