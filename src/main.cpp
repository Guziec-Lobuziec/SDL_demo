
#include <SDL2/SDL.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <cstdint>
#include <array>
#include <cmath>
#include "drawable.hpp"
#include "physics_recipient.hpp"

#define PI 3.14159265

auto errthrow = []( const std::string &e ) {
	std::string errstr = e + " : " + SDL_GetError();
	SDL_Quit();
	throw std::runtime_error( errstr );
};

std::vector<std::tuple<point<int>,Uint32>> draw_circle(int Ox, int Oy, int R) {
	std::vector<std::tuple<point<int>,Uint32>> circle;
	circle.reserve(7*R);

	Uint32 RGBA = 0xff0000ff;

		int i = 0;
		int j = R;
		int f = 5-4*R;

		circle.push_back(std::make_tuple(point<int>(i + Ox,j + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(j + Ox,i + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(i + Ox,-j + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(j + Ox,-i + Oy),RGBA));

		circle.push_back(std::make_tuple(point<int>(-i + Ox,j + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(-j + Ox,i + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(-i + Ox,-j + Oy),RGBA));
		circle.push_back(std::make_tuple(point<int>(-j + Ox,-i + Oy),RGBA));

		// ij ji -ij -ji i-j j-i -i-j -j-i

		while(i <= j)
		{

				if(f>0){
					f = f + 8*(i-j) -20;
					--j;
				}
				else
					f = f + 8*i +12;

				++i;
				circle.push_back(std::make_tuple(point<int>(i + Ox,j + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(j + Ox,i + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(i + Ox,-j + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(j + Ox,-i + Oy),RGBA));

				circle.push_back(std::make_tuple(point<int>(-i + Ox,j + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(-j + Ox,i + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(-i + Ox,-j + Oy),RGBA));
				circle.push_back(std::make_tuple(point<int>(-j + Ox,-i + Oy),RGBA));
		}

	return circle;
}

std::shared_ptr<SDL_Window> init_window( const int width, const int height, const std::string & name) {
	if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) errthrow ( "SDL_Init" );

	SDL_Window *win = SDL_CreateWindow( name.data(),
										SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										width, height, SDL_WINDOW_SHOWN );
	if ( win == nullptr ) errthrow ( "SDL_CreateWindow" );
	std::shared_ptr<SDL_Window> window ( win, []( SDL_Window * ptr ) {
		SDL_DestroyWindow( ptr );
	} );
	return window;
}

std::shared_ptr<SDL_Renderer> init_renderer( std::shared_ptr<SDL_Window> window ) {
	SDL_Renderer *ren = SDL_CreateRenderer( window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if ( ren == nullptr ) errthrow ( "SDL_CreateRenderer" );
	std::shared_ptr<SDL_Renderer> renderer ( ren, []( SDL_Renderer * ptr ) {
		SDL_DestroyRenderer( ptr );
	} );
	return renderer;
}

int main() {

	point<int> window_size(800,600);

	auto window = init_window(window_size.x(),window_size.y(),"test");
	auto renderer = init_renderer(window);
	SDL_Event event;
	bool running = true;

	drawable view(window_size.x(),window_size.y());

	double time_resolution = 1.0/30.0;

	drawable cannon("data/cannon.bmp");

	drawable bucket("data/bucket.bmp");

	std::vector<std::tuple<point<double>,double>> collision_map3;
	for(int l = 0; l<60; l+=5)
		collision_map3.push_back(std::make_tuple(point<double>(2.5 + l*2.5 ,2.5+l*4.166 ),6.0));

	for(int l = 60; l>0; l-=5)
		collision_map3.push_back(std::make_tuple(point<double>( 300 + l*(-2.5) ,-12.5+l*4.166 ),6.0));

	//std::vector<std::tuple<drawable,point<int>>> dr;


	physics_recipient bucket_phy(
		100,
		point<double>(475,250),
		time_resolution,
		collision_map3,
		1,
		0,
		true
	);

	drawable ball("data/circle.bmp");

	std::vector<std::tuple<point<double>,double>> collision_map1;

	collision_map1.push_back(std::make_tuple(point<double>(25.5,25.5),25.5));

	physics_recipient ball_phy(
		1,
		point<double>(75,window_size.y()-100),
		time_resolution,
		collision_map1,
		1,
		0.0005,
		false
	);

	ball_phy
	.set_velocity(alg_vector<double>(0,0))
	.set_acceleration(alg_vector<double>(0,0));

	drawable rubbish(31,31);
	rubbish.draw(draw_circle(15,15,15));

	std::vector<std::tuple<point<double>,double>> rubbish_collision_map;

	rubbish_collision_map.push_back(std::make_tuple(point<double>(15,15),15.0));

	physics_recipient rubbish_phy1(
		0.5,
		point<double>(220,window_size.y()+50),
		time_resolution,
		rubbish_collision_map,
		1,
		0.0001,
		false
	);

	rubbish_phy1
	.set_velocity(alg_vector<double>(4,-55.5))
	.set_acceleration(alg_vector<double>(0,3));

	physics_recipient rubbish_phy2(
		3,
		point<double>(300,-50),
		time_resolution,
		rubbish_collision_map,
		1,
		0.0001,
		false
	);

	rubbish_phy2
	.set_velocity(alg_vector<double>(20,10))
	.set_acceleration(alg_vector<double>(-3.5,1));

	double cannon_angle = -45.0;
	bool spaceHit = false;

	do {

		while( SDL_PollEvent( &event ) != 0 )
		{
				switch (event.type) {
					case SDL_QUIT:
						running = false;
						break;
					case SDL_MOUSEMOTION:
						cannon_angle = -std::atan(
							((double)window_size.y() - (double)event.motion.y)/((double)event.motion.x)
						)*180/PI;
						break;
					case SDL_KEYDOWN:
						if(event.key.keysym.sym == SDLK_SPACE)
							spaceHit = true;
				}
		}

		SDL_RenderClear(renderer.get());

		view.paste_to_renderer(renderer, point<int>(0,0));
		ball.paste_to_renderer(renderer, (point<int>)ball_phy.get_position());
		rubbish.paste_to_renderer(renderer, (point<int>)rubbish_phy1.get_position());
		rubbish.paste_to_renderer(renderer, (point<int>)rubbish_phy2.get_position());
		bucket.paste_to_renderer(renderer, (point<int>)bucket_phy.get_position());
		cannon.paste_to_renderer(renderer, point<int>(-5,window_size.y() - 150), cannon_angle);

		// for(int i = 0; i<collision_map3.size(); i++) {
		//
		// 		drawable d(
		// 			(int)(2*std::get<1>(collision_map3.at(i)))+2,
		// 			(int)(2*std::get<1>(collision_map3.at(i)))+2
		// 		);
		// 		point<int> p(
		// 			(int)(std::get<0>(collision_map3.at(i)).x() + 475  - std::get<1>(collision_map3.at(i))),
		// 			(int)(std::get<0>(collision_map3.at(i)).y() + 100  - std::get<1>(collision_map3.at(i)))
		// 		);
		//
		//
		// 	d.draw(draw_circle(
		// 			(int)(std::get<1>(collision_map3.at(i))/2),
		// 			(int)(std::get<1>(collision_map3.at(i))/2),
		// 			(int)(std::get<1>(collision_map3.at(i)))
		// 		));
		// 	d.paste_to_renderer(renderer,p);
		// }


		SDL_RenderPresent(renderer.get());
		if(spaceHit){
			spaceHit = false;
			ball_phy.set_position(point<double>(75,window_size.y()-100))
			.set_velocity(alg_vector<double>(
				60.0*std::cos(cannon_angle*PI/180),
				60.0*std::sin(cannon_angle*PI/180)
			)).set_acceleration(alg_vector<double>(0,3));
		}

		ball_phy.compute_collisons(bucket_phy);
		ball_phy.compute_collisons(rubbish_phy1);
		ball_phy.compute_collisons(rubbish_phy2);
		rubbish_phy1.compute_collisons(rubbish_phy2);
		rubbish_phy1.compute_collisons(bucket_phy);
		rubbish_phy2.compute_collisons(bucket_phy);

		ball_phy.tick_physics();
		rubbish_phy1.tick_physics();
		rubbish_phy2.tick_physics();

		if(ball_phy.get_position().y()-100 > window_size.y()) {
			ball_phy.set_position(point<double>(75,window_size.y()-100))
			.set_velocity(alg_vector<double>(0,0))
			.set_acceleration(alg_vector<double>(0,0));
		}

		if(rubbish_phy1.get_position().y()-100 > window_size.y()) {
			rubbish_phy1.set_position(point<double>(220,window_size.y()+50))
			.set_velocity(alg_vector<double>(4,-55.5));
		}

		if(rubbish_phy2.get_position().y()-100 > window_size.y()) {
			rubbish_phy2.set_position(point<double>(300,-50))
			.set_velocity(alg_vector<double>(20,10));
		}


		SDL_Delay( ( int )( time_resolution / 1000 ) );

	} while(running);



}
