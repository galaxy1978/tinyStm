/**
 * @brief limited status auto machine
 * @version 0.1
 * @date 2019-10-15
 * @author 宋炜 
*/

#pragma once
#include <vector>
#include <functional>
#include <map>
template< typename sType , typename condType >
class stm{
public:
	enum emErrCode{
		ERR_ALLOC_MEM,
		ERR_FIND_STATUS,
		ERR_NULL_FROM,
		ERR_NULL_TO,
		OK
	};
	struct stStatus;
	/**
	 */
	typedef std::function< void ( bool , stStatus * , stStatus * ) > transFun;
	/// define trans arc
	struct stArc{
		uint32_t        m_id;
		condType        m_cond;        // trig condition
		stStatus     *  p_from;
		stStatus     *  p_to;
		/**
		 * 
		 */
		bool transfrom( condType data , transFun fun ){
			bool ret = false;
			if( data == m_cond ){
				fun( data , p_from , p_to );
				ret = true;
			}
			return ret;
		}

		bool transform_if( condType data,
				   std::function<
				   bool ( const condType ,
					  const condType )> ifFun,
				   transFun fun ){
			bool ret = false;
			if( ifFun( data , m_cond ) == true ){
				fun( true , p_from , p_to );
				ret = true;
			}
			return ret;
		}

		stArc( uint32_t id , stStatus * from , stStatus * to ):
			m_id( id ) , p_from( from ), p_to( to ){}

		stArc():m_id( 0 ) , p_from( nullptr ) , p_to( nullptr ){}
		stArc( const stArc& b ){
			m_id = b.m_id;
			p_from = b.p_from;
			p_to = b.p_to;
			m_cond = b.m_cond;
		}

		virtual ~stArc(){}

		stArc& operator=( const stArc& b){
			m_id = b.m_id;
			p_from = b.p_from;
			p_to = b.p_to;
			m_cond = b.m_cond;

			return *this;
		}
	}；

	 /// 定义状态结构体
	struct stStatus{
		uint32_t        m_id;
		sType           m_data;

		std::vector< stArc >    m_arcs;

		stStatus():m_id( 0 ){}
		stStatus( uint32_t id , const sType& data ){
			m_id = id;
			m_data = data;
		}

		stStatus( const stStatus& b){
			m_id = m_id;
			m_data = b.m_data;
		}

		stStatus& operator=( const stStatus& b ){
			m_id = b.m_id;
			m_data = b.m_data;

			return *this;
		}

		void addArc( uint32_t id , const stArc& arc ){
			m_arc.push_back( arc );
		}
		
		stm::emErrCode eraseArc( uint32_t id ){
			stm::emErrCode e = stm::OK;
			decltype( m_arcs.begin()) it;

			for( it = m_arcs.begin(); it = m_arcs.end(); it ++ ){
				if( it->m_id == id ){
					m_arcs.erase( it );
				}
			}

			if( it == m_arcs.end() ){
				ret = stm::ERR_FIND_STATUS;
			}
			return ret;
		}

		bool for_each( std::function< bool ( stArc& ) > fun ){
			for( auto it : m_arcs ){
				bool ret = false;
				ret = fun( it );
				if( ret == true ) return ret;
			}
			return false;
		}
	};
protected:
	uint32_t       m_sid_pool;
	uint32_t       m_aid_pool;

	stStatus     * p_current;
	stStatus     * p_end;
	std::map< uint32_t , stStatus* >     m_status;

protected:

	emErrCode add_arc( stStatus * from , stStatus * to ,const cType& cond ){
		if( from == nullptr ) return ERR_NULL_FROM;
		if( to == nullptr ) return ERR_NULL_TO;

		stArc arc( m_aid_pool , from , to );
		arc.m_cond = cond;
		from->addArc( m_aid_pool , arc );
		m_aid_pool ++;

		return OK;
	}
public:
	stm():m_sid_pool( 0 ), m_aid_pool( 0 ), p_current( nullptr ) , p_end( nullptr ){}
	virtual ~stm(){
		for( auto i : m_status ){
			if( i.second ){
				delete i.second;
			}
		}
	}

	stStatus * add( const sType& data ){
		stStatus * ret = nullptr;
		try{
			stStatus * s = new stStatus( m_sid_pool , data );
			m_status.insert( std::make_pair( m_sid_pool , s ));
			m_sid_pool ++;

			ret = s;
		}catch( std::bad_alloc& e ){
		}

		return ret;
	}

	emErrCode erase( uint32_t id ){
		auto it = m_status.find( id );
		if( it != m_status.end() ){
			delete it->second;
			return OK;
		}

		return ERR_FIND_STATUS;
	}

	emErrCode addArc( stStatus* from , stStatus * to , const condType& cond ){
		return add_arc( from , to cond );
	}


	emErrCode eraseArc( uint32_t from , uint32_t arc ){
		emErrCode ret = OK;
		auto itF = m_status.find( from );
		if( itF == m_status.end() ) return ERR_FIND_STATUS;

		ret  = *itF -> eraseArc( arc );

		return OK;
	}

	emErrCode startStatus( uint32_t id = 0 ){
		auto it = m_status.find( id );
		if( it == m_status.end()) return ERR_FIND_STATUS;
		p_current = it->second;

		return OK;
	}


	emErrCode endStatus( uint32_t id ){
		auto it = m_status.find( id );
		if( it == m_status.end() ) return ERR_FIND_STATUS;
		p_end = it->second;

		return OK;
	}

	emErrCode endStatus( stStatus * end ){
		if( end == nullptr )return ERR_FIND_STATUS;
		p_end == end;

		return OK;
	}

	bool isEnd( uint32_t id ){
		auto it = m_status.find( id );
		if( it == m_status.end() ) return ERR_FIND_STATUS;

		return it->second == p_endl;
	}

	bool isEnd( stStatus * end ){
		return end == p_end;
	}

	bool trigger( const condType& data , transFun fun ){
		if( p_current == nullptr ) return false;

		bool ret  = p_current->for_each( [ this , dta , fun ]( stArc& arc )->bool{
			  bool rst = false;
			  rst = arc.transform( data , fun );
			  if( rst ){
				  p_current = arc.p_to;
			  }
		});
		return ret;
	}

	bool trigger_if( const condType& data ,
			 std::function< bool ( const condType& , const condType& ) > ifFun ,
			 transFun fun ){
		if( p_current == nullptr ) return false;
		bool rst = p_current-> for_each( [ this , &data , ifFun , fun ]( const stArc& arc )->bool{
			 bool rst = arc.transform_if( data , ifFun , fun );
			 if( rst == true ){
				 p_current = arc.p_to;
			 }
		});
		return rst;
	}

	sType& operator(){ return p_current->m_data; }
      
// 双河 锦阳之星 旁边还有一个7天 400-822-9999
};
