#include <memory>
#include <atomic>

template<typename T>
class lockfree_stack_t
{
private:
	class node_t
	{
	private:
		std::shared_ptr<T> data_;
		node_t* next_;
	public:
		node_t(T const& data) : data_{ std::make_shared<T>( data ) }, next_{ nullptr }
		{
		}
		auto next() -> node_t*&
		{
			return next_;
		}
		auto data() -> std::shared_ptr<T>&
		{
			return data_;
		}
	};

	std::atomic<node_t*> head_;
	std::atomic<std::size_t> threads_in_pop_;
	std::atomic<node_t*> to_be_deleted_;
private:
	auto try_reclaim( node_t* node ) -> void
	{
		if( threads_in_pop_ == 1 ) {
			node_t* nodes_to_delete = to_be_deleted_.exchange( nullptr );
			if( --threads_in_pop_ == 0 ) {
				delete_nodes( nodes_to_delete );
			}
			else if( nodes_to_delete ) {
				chain_pending_nodes( nodes_to_delete );
			}
	
			delete node;
		}
		else {
			chain_pending_node( node );
			--threads_in_pop_;
		}
	}

	auto chain_pending_nodes(node_t* nodes) -> void
	{
		node_t* last = nodes;
		while( node_t* const next = last->next() ) {
			last = next;
		}
		
		chain_pending_nodes( nodes, last );
	}

	auto chain_pending_node( node_t* node ) -> void
	{
		chain_pending_nodes( node, node );
	}

	auto chain_pending_nodes( node_t* first, node_t* last ) -> void
	{
		last->next() = to_be_deleted_;
		while( !to_be_deleted_.compare_exchange_weak( last->next(), first ) ) {
			;
		}
	}

	static auto delete_nodes( node_t* nodes ) -> void
	{
		auto node = nodes;
		while( node ) {
			node_t* const next = node->next();
			delete node;
			node = next;
		}
	}
public:
    lockfree_stack_t() : head_{ nullptr }, threads_in_pop_{ 0 }, to_be_deleted_{ nullptr }
    {
        
    }
    
	auto push( T const& data ) -> void
	{
		node_t* const node = new node_t{ data };
		node->next() = head_.load();
		while( !head_.compare_exchange_weak( node->next(), node ) ) {
			;
		}
	}

	auto pop() -> std::shared_ptr<T>
	{
		++threads_in_pop_;
		node_t* node = head_.load();
		while( node && !head_.compare_exchange_weak( node, node->next() ) ) {
			;
		}

		std::shared_ptr<T> result;
		if( node ) {
			result.swap( node->data() );	
		}

		try_reclaim( node );
		
		return result;
	}

    auto empty() -> bool
    {
        return !head_;
    }
};
