/**
 * @bief 状态机测试
 * @version 1.0
 * @date 2019-10-16
 * @author 宋炜
 */
#include <iostream>
#include <string>
#include "stm.hpp"

int main( int argc , const char * argv[] )
{
	int ret = 0;

	stm<std::string , int >  mach;
	try{
		// add two status node p_s1 as start , and p_s2 as end;
		stm<std::string,  int >::stStatus * p_s1 = mach.add( "ready" ); 
		// specify the start node which id is 0
		mach.startStatus();

		stm<std::string , int > :: stStatus *p_s2 = mach.add("end" );
		// specify the end node with node pointer.
		mach.endStatus( p_s2 );
		// add convert arc , which trigged by a interger number of 1
		mach.addArc( p_s1 , p_s2 , 1 );
		// 从这里开始
		int data;
		while( true ){
			std::cout << "Input command: ";
			std::cin >> data;
			stm<std::string , int >::stStatus * f = nullptr, * e = nullptr;
			bool isOK = false;
			mach.trigger( data , [&isOK ,  &f , &e ]( bool isTransOk , stm<std::string , int >::stStatus * from , stm<std::string, int >::stStatus * end ){
				   // do something before convert;
				   isOK = isTransOk;
				   f = from;
				   e = end;			       
                        });

			// do something after convert;
			std::cerr << "CURRENT STATUS: " << mach.getCurrent() << std::endl;
			if( mach.isEnd( e ) == true ){
				std::cerr << "the machine is finished" << std::endl;
				break;
			}
						  
		}
	}catch( std::bad_alloc& e ){
		std::cerr << "Allocate memory fail" << std::endl;
		ret = -1;
	}
	return ret;
}
