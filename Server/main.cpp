// Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#pragma warning(disable:4996)
#include "stdafx.h"
#include "Server.h"



int _tmain(int argc, _TCHAR* argv[])
{
	//try
	//{
//	databody::join bhe;
//	databody::login _login;
//	google::protobuf::Message *p = &_login;

	
//	cout << bhe.GetDescriptor()->index();
//	cout << bhe.GetDescriptor()->full_name();
//	cout << bhe.GetDescriptor()->name();

//	cout << p->GetDescriptor()->index();

	/*
	gameuser_machine gm(NULL);
	gm.initiate();
	gm.process_event(evtick(1.0f));
	gm.process_event(evtick(1.0f));
	gm.process_event(evtick(1.0f));
	gm.process_event(evmove());
	gm.process_event(evskill<skilltype::skill1>());

	gm.process_event(evtick(1.1f));
	gm.process_event(evtick(1.1f));
	*/


		server s(8999, 9258);
		s.run();

		
		/*b2World* world;
		b2Vec2 vec(0.0f,-9.8f);
		world = new b2World(vec);
		world->Step(1.0f / 30.0f, 10, 10);
		*/
//	}
//	catch(std::exception& e){
//		std::cerr << "exception : " << e.what() << "\n";
//		int i;
//		std::cin >> i;

// }
	return 0;
}


