#include<SFML/Graphics.hpp>
#include<bits/stdc++.h>
using namespace std;
using namespace sf;
float velocity = 0.0f;
float pipeSpeed = 300.0f;
float gap = 300.0f;
int pipesCrossed = 0;
bool dead = false;
struct PipePair
{
    Sprite top;
    Sprite bottom;
    bool counted = false;
};


int mainOld()
{
	Clock clock;
	RenderWindow win(VideoMode({1600,1200}),"Flappy Bird");
	Texture birdTexture;
	if (!birdTexture.loadFromFile("assets/Bird16.png"))	{
    	    	cout << "Failed to load bird!" << endl;
	    	return 1;
	}
	Sprite bird(birdTexture, IntRect({0, 0}, {16, 16}));	bird.setScale({10.0f, 10.0f});
	bird.setPosition({400,600});
	Texture groundTexture;
	if (!groundTexture.loadFromFile("assets/Ground.png"))
	{
		cout << "Failed to load ground!" << endl;
		return 1;
	}
	Sprite ground(groundTexture);
	ground.setScale({1600.0f / 180.0f, 3.75f});
	ground.setPosition({0, 1200 - ground.getGlobalBounds().size.y});
	float groundY = ground.getPosition().y;

	Texture backgroundTexture;

	if (!backgroundTexture.loadFromFile("assets/Background.png"))
	{
		cout << "Failed to load background!" << endl;
		return 1;
	}
	Sprite background(backgroundTexture);
	background.setScale({1600.0f / 180.0f,1200.0f / 320.0f});


	Texture pipeTexture;
	if (!pipeTexture.loadFromFile("assets/Pipes16.png"))
	{
		cout << "Failed to load pipes!" << endl;
		return 1;
	}

	vector<PipePair> pipes;

	for (int i = 0; i < 4; i++)
	{
		PipePair pair{
			Sprite(pipeTexture, IntRect({32, 0}, {32, 320})),
			Sprite(pipeTexture, IntRect({0, 0}, {32, 320}))
		};

		pair.top.setScale({5.0f, 2.0f});
		pair.bottom.setScale({5.0f, 2.0f});

		float gapTop = 250.0f + rand() % 450;

		pair.top.setPosition({
			1200.0f + i * 500.0f,
			gapTop - pair.top.getGlobalBounds().size.y
		});

		pair.bottom.setPosition({
			1200.0f + i * 500.0f,
			gapTop + gap
		});

		pipes.push_back(pair);
	}
	while(win.isOpen()){
	while(const optional event = win.pollEvent()){
		if(event->is<Event::Closed>())
			win.close();
		
		if(event->is<Event::KeyPressed>()){
			if(event->getIf<Event::KeyPressed>()->code == Keyboard::Key::Space){
				velocity = -500.0f;
			}
		}
		}
		float dt = clock.restart().asSeconds();
		for(auto& pipe: pipes){
			pipe.bottom.move({-pipeSpeed * dt, 0.0f});
		    pipe.top.move({-pipeSpeed * dt, 0.0f});
		}
		for (auto& pipe : pipes)
		{
			if (pipe.top.getPosition().x + pipe.top.getGlobalBounds().size.x < 0)
			{
				float gapTop = 250.0f + rand() % 400;

				pipe.top.setPosition({
					1600.0f,
					gapTop - pipe.top.getGlobalBounds().size.y
				});

				pipe.bottom.setPosition({
					1600.0f,
					gapTop + gap
				});

				pipe.counted = false;
			}
		}
		velocity += 1000.0f * dt;
		for(auto& pipe:pipes){
			bird.move({0.0f, velocity * dt});
			if (!dead &&
				(bird.getGlobalBounds().findIntersection(pipe.bottom.getGlobalBounds()) ||
				bird.getGlobalBounds().findIntersection(pipe.top.getGlobalBounds())))
			{
				dead = true;
				velocity = 0.0f;
			}
		}

		if (bird.getPosition().y + bird.getGlobalBounds().size.y >= groundY)
		{
			bird.setPosition({
				bird.getPosition().x,
				groundY - bird.getGlobalBounds().size.y
			});

			velocity = 0.0f;
		}
		win.clear(Color(135,206,235));
		win.draw(background);
		win.draw(bird);
		win.draw(ground);
		for(auto& pipe:pipes){
			win.draw(pipe.top);
			win.draw(pipe.bottom);
		}
		win.display();
	}
	return 0;
}
