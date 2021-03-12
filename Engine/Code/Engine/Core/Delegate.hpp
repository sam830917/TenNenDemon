#include <vector>
#include <functional>

template <typename ...ARGS>

class Delegate
{
public:
	using function_t = std::function<void( ARGS... )>;
	using c_callback_t = void (*)(ARGS...);

	struct sub_t // subscription_t 
	{
		void const* obj_id      = nullptr;
		void const* func_id     = nullptr;
		function_t callable;

		inline bool operator==( sub_t const& other ) const { return (obj_id == other.obj_id) && (func_id == other.func_id); }
	};

public:
	//------------------------------------------------------------------------
	void Subscribe( c_callback_t const& cb )
	{
		sub_t sub;
		sub.func_id = cb;  // cb = &cb = &&cb = *cb = **cb = ***cb = ...
		sub.callable = cb;

		Subscribe( sub );
	}

	//------------------------------------------------------------------------
	void Unsubscribe( c_callback_t const& cb )
	{
		sub_t sub;
		sub.func_id = cb;

		Unsubscribe( sub );
	}

	//------------------------------------------------------------------------
	template <typename OBJ_TYPE>
	void SubscribeMethod( OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...) )
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb;

		// std::bind
		// capture list
		sub.callable = [ = ]( ARGS ...args ) { (obj->*mcb)(args...); };

		Subscribe( sub );
	}

	//------------------------------------------------------------------------
	template <typename OBJ_TYPE>
	void UnsubscribeMethod( OBJ_TYPE* obj, void (OBJ_TYPE::* mcb)(ARGS...) )
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb;

		Unsubscribe( sub );
	}

	// void unsubscribe_object( void const* obj_ptr ); 
	// void unsubscribe_all(); 

	//------------------------------------------------------------------------
	// int args, int args
	void Invoke( ARGS const& ...args )
	{
		for( sub_t& sub : m_subscriptions ) {
			sub.callable( args... );
		}
	}

	void operator() ( ARGS const& ...args ) { Invoke( args... ); }

private:
	//------------------------------------------------------------------------
	void Subscribe( sub_t const& sub )
	{
		m_subscriptions.push_back( sub );
	}

	//------------------------------------------------------------------------
	void Unsubscribe( sub_t const& sub )
	{
		for( uint i = 0; i < m_subscriptions.size(); ++i ) {
			if( m_subscriptions[i] == sub ) {
				m_subscriptions.erase( m_subscriptions.begin() + i );
				return;
			}
		}
	}

	std::vector<sub_t> m_subscriptions;
};