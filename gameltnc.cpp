#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <TGUI/TGUI.hpp>
#include <time.h>
#include<fstream>
#include<iostream>
#include<cstring>
#include<string>
#include <algorithm>
using namespace sf;
using namespace std;
const int M = 24; //// mac dinh la 20 /////
const int N = 10;

int field[M][N] = { 0 };

struct Point
{
	int x, y;
} a[4], b[4], c[4];

struct HighScore
{
	char name[50];
	int highscore;
}hs[12];

int figures[7][4] =
{
		1,3,5,7, // I
		2,4,5,7, // Z
		3,5,4,6, // S
		3,5,4,7, // T
		2,3,5,7, // L
		3,5,7,6, // J
		2,3,4,5, // O
};

bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
		else if (field[a[i].y][a[i].x]) return 0;

	return 1;
};

bool kt(HighScore a, HighScore b) {
	return a.highscore > b.highscore;
}


int main()
{
	srand(time(0));

	RenderWindow window(VideoMode(320, 480), "TETRIS", Style::Close);
	window.setFramerateLimit(30);
	////<-load font->/////
	Font font;
	font.loadFromFile("font/Actionsh.ttf");

	////<-load images ->/////
	Texture t0, t6, t7;
	t0.loadFromFile("images/background.png");
	t6.loadFromFile("images/gameOver.png");
	t7.loadFromFile("images/tutorial.png");
	Sprite background(t0), gameOver(t6), tutorialBackground(t7);

	Texture t1, t2, t3, t4, t5;
	t1.loadFromFile("images/tiles.png");
	t2.loadFromFile("images/frame1.png");
	t3.loadFromFile("images/frame2.png");
	t4.loadFromFile("images/background1.png");
	Sprite s(t1), frame1(t2), frame2(t3), background1(t4);

	////<-load sound ->//////
	SoundBuffer soundDown, soundRotate, soundBoom, soundMenu, soundOver;
	soundMenu.loadFromFile("sounds/soundMenu.wav");
	soundOver.loadFromFile("sounds/soundOver.wav");
	soundDown.loadFromFile("sounds/soundDown.wav");
	soundRotate.loadFromFile("sounds/soundRotate.wav");
	soundBoom.loadFromFile("sounds/soundBoom.wav");
	Sound sounddown(soundDown), soundrotate(soundRotate), soundboom(soundBoom), soundover(soundOver), soundmenu(soundMenu);
	soundmenu.play();
	soundmenu.setLoop(true);

	////<- Khoi tao ->////
	int dx = 0; bool rotate = 0; int colorNum = 1; int colorNumNext = 1;
	float timer = 0, delay = 1;
	int score = 0, level = 1, line = 0;

	colorNum = 1 + rand() % 7;
	int n0 = rand() % 7;
	for (int i = 0; i < 4; i++)
	{
		a[i].x = figures[n0][i] % 2 + N / 2 - 2;
		a[i].y = figures[n0][i] / 2;
	}

	colorNumNext = 1 + rand() % 7;
	int n = rand() % 7;
	for (int i = 0; i < 4; i++)
	{
		c[i].x = figures[n][i] % 2;
		c[i].y = figures[n][i] / 2;
	}

	bool pause = true;
	Clock clock;
	int choose = 0;
	int menuChose = 0;
	while (window.isOpen())
	{
		switch (menuChose)
		{
		case 0://giao dien menu
		{
			Event e;
			Text play("PLAY", font, 30), tutorial("TUTORIAL", font, 30), highScore("HIGH SCORE", font, 30), exitx("EXIT", font, 30);
			play.setPosition(125, 140);
			tutorial.setPosition(100, 200);
			highScore.setPosition(95, 260);
			exitx.setPosition(130, 320);
			while (window.pollEvent(e))
			{
				if (e.type == Event::Closed)
					window.close();
				if (e.type == Event::KeyPressed)
					if (e.key.code == Keyboard::Up) choose = (choose + 4 - 1) % 4;
					else if (e.key.code == Keyboard::Down) choose = (choose + 1) % 4;

			}
			switch (choose)
			{
			case 0://chon play
			{
				play.setFillColor(Color::Red);
				tutorial.setFillColor(Color::White);
				highScore.setFillColor(Color::White);
				exitx.setFillColor(Color::White);
				if (Keyboard::isKeyPressed(Keyboard::Enter)) menuChose = 1;
				break;
			}
			case 1://chon tutorial
			{
				play.setFillColor(Color::White);
				tutorial.setFillColor(Color::Red);
				highScore.setFillColor(Color::White);
				exitx.setFillColor(Color::White);
				if (Keyboard::isKeyPressed(Keyboard::Enter)) menuChose = 2;
				break;
			}
			case 2://chon highscore
			{
				play.setFillColor(Color::White);
				tutorial.setFillColor(Color::White);
				highScore.setFillColor(Color::Red);
				exitx.setFillColor(Color::White);
				if (Keyboard::isKeyPressed(Keyboard::Enter)) menuChose = 3;
				break;
			}
			case 3://chon exit
			{
				play.setFillColor(Color::White);
				tutorial.setFillColor(Color::White);
				highScore.setFillColor(Color::White);
				exitx.setFillColor(Color::Red);
				if (Keyboard::isKeyPressed(Keyboard::Enter)) window.close();
				break;
			}
			}
			window.clear(Color::White);
			window.draw(background);
			window.draw(play);
			window.draw(tutorial);
			window.draw(highScore);
			window.draw(exitx);
			break;
		}
		case 1://giao dien choi game
		{
			if (Keyboard::isKeyPressed(Keyboard::R)) pause = true;
			if (pause)
			{
				float time = clock.getElapsedTime().asSeconds();
				clock.restart();
				timer += time;

				Event e;
				while (window.pollEvent(e))
				{
					if (e.type == Event::Closed)
						window.close();

					if (e.type == Event::KeyPressed)
						if (e.key.code == Keyboard::Up) rotate = true;
						else if (e.key.code == Keyboard::Left) dx = -1;
						else if (e.key.code == Keyboard::Right) dx = 1;
				}
				if (Keyboard::isKeyPressed(Keyboard::P)) pause = false;//phan pause man choi neu nhan P
				if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;// tang toc do gach roi

				//// <- Move -> ////
				for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
				if (dx != 0) sounddown.play();
				if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

				//// <- Rotate -> ////
				if (rotate)
				{
					Point p = a[1]; //center of rotation
					for (int i = 0; i < 4; i++)
					{
						int x = a[i].y - p.y;
						int y = a[i].x - p.x;
						a[i].x = p.x - x;
						a[i].y = p.y + y;
					}
					if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];
					soundrotate.play();
				}

				//// <- Tick -> ////
				if (timer > delay)
				{
					for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }

					if (!check())
					{
						for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

						colorNum = colorNumNext;
						n0 = n;
						for (int i = 0; i < 4; i++)
						{
							a[i].x = figures[n0][i] % 2 + N / 2 - 2;
							a[i].y = figures[n0][i] / 2;
						}

						colorNumNext = 1 + rand() % 7;
						n = rand() % 7;
						for (int i = 0; i < 4; i++)
						{
							c[i].x = figures[n][i] % 2;
							c[i].y = figures[n][i] / 2;
						}
						//// kiem tra xem co lost khong ////
						bool kt = true;
						for (int j = 0; j < N; j++)
						{
							if (field[4][j]) kt = false;
						}
						if (kt == false)
						{
							menuChose = 5;
							soundmenu.stop();
							soundover.play();
						}
					}
					sounddown.play();
					timer = 0;
				}


				//// <- check lines -> /////
				int k = M - 1;
				for (int i = M - 1; i > 0; i--)
				{
					int count = 0;
					for (int j = 0; j < N; j++)
					{
						if (field[i][j]) count++;
						field[k][j] = field[i][j];
					}
					if (count < N) k--;
				}
				if (k != 0) soundboom.play();
				line += k;

				//// <- update level -> ////
				score += level * 10 * k;
				if (line >= level)
				{
					line = 0;
					level++;
				}

				dx = 0; rotate = 0;
				delay = 1 - (level - 1) * 0.07;
				if (delay < 0) delay = 0.05;
				/////////draw//////////
				window.clear(Color::White);
				window.draw(background1);
				window.draw(frame1);
				for (int i = 0; i < M; i++)
					for (int j = 0; j < N; j++)
					{
						if (field[i][j] == 0) continue;
						s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
						s.setPosition(j * 18, i * 18);
						s.move(37, 26.2); //offset
						window.draw(s);
					}

				for (int i = 0; i < 4; i++)
				{
					s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
					s.setPosition(a[i].x * 18, a[i].y * 18);
					s.move(37, 26.2); //offset
					window.draw(s);
				}

				window.draw(frame2);
				Text text1("Level: ", font, 18), text2("Score: ", font, 18);
				Text text3(std::to_string(level), font, 18), text4(std::to_string(score), font, 18);
				text1.setPosition(240, 250);
				text3.setPosition(250, 270);
				text2.setPosition(240, 290);
				text4.setPosition(250, 310);
				window.draw(text1);
				window.draw(text2);
				window.draw(text3);
				window.draw(text4);
				//// <- ve khoi hinh tiep theo ->////
				for (int i = 0; i < 4; i++)
				{
					s.setTextureRect(IntRect(colorNumNext * 18, 0, 18, 18));
					s.setPosition(c[i].x * 18, c[i].y * 18);
					s.move(255, 125);
					window.draw(s);
				}

			}
			break;
		}
		case 2: // Tutorial
		{
			Event e;
			while (window.pollEvent(e)) {
				if (e.type == Event::Closed)
					window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::BackSpace)) {
				menuChose = 0;
			}
			Text tutorial1("Tutorial", font, 40), returnmenu("Return", font, 20);
			returnmenu.setPosition(120, 420);
			tutorial1.setPosition(75, 0);
			tutorialBackground.setPosition(0, 54);
			tutorial1.setFillColor(Color::Black);
			returnmenu.setFillColor(Color::Red);
			window.clear(Color::White);
			window.draw(tutorialBackground);
			window.draw(tutorial1);
			window.draw(returnmenu);
			break;
		}
		case 3:// diem cao- hight score vs du lieu tu file highScore.txt
		{
			Event e;
			while (window.pollEvent(e))
			{
				if (e.type == Event::Closed)
					window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::BackSpace))
			{
				menuChose = 0;
			}
			ifstream input("highScore.txt");//lấy dữ liệu điểm từ file 
			int k = 0;
			while (!input.eof())
			{
				char str[50];
				input.getline(str, 50);
				strcpy_s(hs[k].name, str);
				input >> hs[k].highscore;
				input.getline(str, 50);
				k++;
			}
			input.close();
			if (hs[k - 1].highscore == 0) k--;
			if (k > 10) k = 10;

			Text highScore1("High Score", font, 40), returnmenu2("Return", font, 20);
			returnmenu2.setPosition(120, 420);
			highScore1.setPosition(70, 0);
			highScore1.setFillColor(Color::Black);
			returnmenu2.setFillColor(Color::Red);
			window.clear(Color::White);
			for (int i = 0; i < k; i++)
			{
				Text STT(to_string(i + 1), font, 18);
				Text Name(hs[i].name, font, 18);
				Text HighScore(to_string(hs[i].highscore), font, 18);
				STT.setPosition(10, 70 + i * 25);
				Name.setPosition(50, 70 + i * 25);
				HighScore.setPosition(220, 70 + i * 25);
				Name.setFillColor(Color::Green);
				STT.setFillColor(Color::Green);
				HighScore.setFillColor(Color::Green);
				window.draw(Name);
				window.draw(HighScore);
				window.draw(STT);

			}
			window.draw(highScore1);
			window.draw(returnmenu2);
			break;
		}
		case 4://thiet lap lai tro choi
		{
			
			Event e;
			while (window.pollEvent(e))
			{
				if (e.type == Event::Closed)
					window.close();
			}
			window.clear(Color::Black);
			gameOver.setPosition(0, 100);
			window.draw(gameOver);
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				menuChose = 0;
				soundmenu.play();
				soundmenu.setLoop(true);
				for (int i = 0; i < M; i++)
					for (int j = 0; j < N; j++)
					{
						field[i][j] = 0;
					}
				dx = 0; rotate = 0; colorNum = 1; colorNumNext = 1;
				timer = 0, delay = 1;
				score = 0, level = 1, line = 0;

				colorNum = 1 + rand() % 7;
				n0 = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = figures[n0][i] % 2 + N / 2 - 2;
					a[i].y = figures[n0][i] / 2;
				}

				colorNumNext = 1 + rand() % 7;
				n = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					c[i].x = figures[n][i] % 2;
					c[i].y = figures[n][i] / 2;
				}

				pause = true;
			}
			break;
		}
		case 5:// luu diem cao
		{
			RenderWindow window1(VideoMode(320, 480), "TGUI");
			window1.setFramerateLimit(60);
			Texture t8;
			t8.loadFromFile("images/saveScore.png");
			Sprite saveScore(t8);
			tgui::Gui gui(window1);
			tgui::EditBox::Ptr box = tgui::EditBox::create();
			box->setSize(150, 40);
			box->setPosition(90, 100);
			gui.add(box);
			while (window1.isOpen())
			{
				Event e;
				while (window1.pollEvent(e))
				{
					if (e.type == Event::Closed)
						window1.close();
					gui.handleEvent(e);
				}
				if (Keyboard::isKeyPressed(Keyboard::Enter))
				{
					if (box->getText().toAnsiString() != "")
					{
						window1.close();
						std::fstream fsFile("highScore.txt", std::ios::in);
						int k = 0;
						while (!fsFile.eof())
						{
							char str[40];
							fsFile.getline(str, 40);
							strcpy_s(hs[k].name, str);
							fsFile >> hs[k].highscore;
							fsFile.getline(str, 50);
							k++;
						}
						fsFile.close();
						/// sx cac so
						if (k > 10) k = 10;
						string str = box->getText().toAnsiString();
						copy(str.begin(), str.end(), hs[k].name);
						hs[k].name[str.size()] = '\0';
						//strcpy(hs[k].name, str.c_str());
						hs[k].highscore = score;
						//cout << hs[k].highscore << " " << k << endl;
						sort(hs, hs + 11, kt);
						/// viet ra file
						ofstream output("highScore.txt");
						for (int i = 0; i < 10; i++) {
							output << hs[i].name << endl << hs[i].highscore << endl;
							cout << hs[i].name << endl << hs[i].highscore << endl;
						}
							
						output.close();
					}
				}
				window1.clear(Color::Black);
				window1.draw(saveScore);
				Text nameScore("Name", font, 40), returnmenu("Save", font, 20);
				Text text1("Score", font, 30), text2(std::to_string(score), font, 30);
				text1.setPosition(120, 200);
				text2.setPosition(120, 270);
				returnmenu.setPosition(140, 400);
				nameScore.setPosition(120, 20);
				nameScore.setFillColor(Color::Blue);
				returnmenu.setFillColor(Color::Red);
				window1.draw(text1);
				window1.draw(text2);
				window1.draw(nameScore);
				window1.draw(returnmenu);
				gui.draw();

				window1.display();
			}
			menuChose = 4;
			break;
		}
		}
		
		window.display();
	}



	return 0;
}