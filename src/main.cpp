#include<SFML/Graphics.hpp>
#include<bits/stdc++.h>
using namespace std;
using namespace sf;
float velocity = 0.0f;
int main()
{
	Clock clock;
	RenderWindow win(VideoMode({1600,1200}),"Flappy Bird");
	//bird
	Texture birdTexture;
	if (!birdTexture.loadFromFile("assets/Bird16.png"))	{
    	    	cout << "Failed to load bird!" << endl;
	    	return 1;
	}
	Sprite bird(birdTexture, IntRect({0, 0}, {16, 16}));	bird.setScale({10.0f, 10.0f});
	bird.setPosition({400,600});
	//ground
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

	//background
	Texture backgroundTexture;

	if (!backgroundTexture.loadFromFile("assets/Background.png"))
	{
		cout << "Failed to load background!" << endl;
		return 1;
	}
	Sprite background(backgroundTexture);
	background.setScale({1600.0f / 180.0f,1200.0f / 320.0f});


	//pipes
	Texture pipeTexture;

	if (!pipeTexture.loadFromFile("assets/Pipes16.png"))
	{
		cout << "Failed to load pipes!" << endl;
		return 1;
	}
	Sprite bottomPipe(pipeTexture, IntRect({0, 0}, {32, 320}));
	Sprite topPipe(pipeTexture, IntRect({32, 0}, {32, 320}));
	bottomPipe.setScale({3.0f, 2.0f});
	topPipe.setScale({3.0f, 2.0f});

	float gap = 250.0f;

	bottomPipe.setPosition({
		1200.0f,
		groundY - bottomPipe.getGlobalBounds().size.y
	});

	topPipe.setPosition({
		1200.0f,
		bottomPipe.getPosition().y - gap - topPipe.getGlobalBounds().size.y
	});
	//Event handling
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
		velocity += 1000.0f * dt;
		bird.move({0.0f, velocity * dt});

		// cout << bird.getPosition().y << " "<< bird.getGlobalBounds().position.y << " "<< bird.getGlobalBounds().size.y << " "<< groundY << endl;
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
		win.draw(bottomPipe);
		win.draw(topPipe);
		win.display();
	}
	return 0;
}
