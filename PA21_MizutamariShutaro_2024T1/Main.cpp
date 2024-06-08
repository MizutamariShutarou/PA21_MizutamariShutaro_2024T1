# include <Siv3D.hpp> // Siv3D v0.6.13

//コンパイル時定数
constexpr int BALL_RAD = 40;

constexpr float BALL_SPEED = 300.0;

constexpr Size BRICK_SIZE{40, 20};

constexpr int X_COUNT = 20;
constexpr int Y_COUNT = 5;
constexpr int MAX = X_COUNT * Y_COUNT;

constexpr float INTERVAL_TIMER = 0;
float _timer = 0;
float _isActiveTimer = 0;

bool _isActive = false;

constexpr enum STATE
{
	Title,
	InGame,
	Finish,
};

STATE _state;

void Main()
{
	_state = Title;

	Font _text{30};

	Vec2 ballVec{0, BALL_SPEED};

	Circle ball{400, 400, 8};

	Rect bricks[MAX];

	for (int i = 0; i < X_COUNT; ++i)
	{
		for (int j = 0; j < Y_COUNT; ++j)
		{
			int index = (j * X_COUNT) + i;

			bricks[index] = Rect{i * BRICK_SIZE.x, 60 + j * BRICK_SIZE.y, BRICK_SIZE};
		}
	}

	while (System::Update())
	{
		if (_state == Title)
		{
			_isActive = false;
			_timer = 0;
			_isActiveTimer = 0;

			_text(U"Click to MouseLeftButton").drawAt(Scene::Center(), Palette::Skyblue);
			if (MouseL.down())
			{
				_state = InGame;
			}
		}

		if (_state == Finish)
		{
			_text(U"Finish").drawAt(Scene::Center(), Palette::Skyblue);

			if (MouseL.down())
			{
				ball.setPos(400, 400);
				ballVec.x = 0;
				for (int i = 0; i < X_COUNT; ++i)
				{
					for (int j = 0; j < Y_COUNT; ++j)
					{
						int index = (j * X_COUNT) + i;

						bricks[index] = Rect{i * BRICK_SIZE.x, 60 + j * BRICK_SIZE.y, BRICK_SIZE};
					}
				}

				_state = Title;
			}
		}

#pragma region 更新処理

		const Rect paddle{Arg::center(Cursor::Pos().x, 500), 60, 10};

		const Rect npcPaddle{Arg::center(Cursor::Pos().x, 250), 60, 10};

		if (_state == InGame)
		{
			ball.moveBy(ballVec * Scene::DeltaTime());
			_timer += Scene::DeltaTime();

			if (!_isActive && _timer >= 5)
			{
				_isActive = true;
				_isActiveTimer = 0;
			}

			if (_isActive)
			{
				_isActiveTimer += Scene::DeltaTime();

				npcPaddle.rounded(3).draw();
				// NPCパドルとの衝突
				if ((0 < ballVec.y) && ball.y < npcPaddle.y && npcPaddle.intersects(ball))
				{
					ballVec = Vec2{(ball.x - npcPaddle.center().x) * 10, -ballVec.y}.setLength(BALL_SPEED * 1.7);
				}
				if ((0 > ballVec.y) && ball.y > npcPaddle.y && npcPaddle.intersects(ball))
				{
					ballVec = Vec2{(ball.x - npcPaddle.center().x) * 10, -ballVec.y}.setLength(BALL_SPEED * 1.7);
				}

				if (_isActiveTimer >= 8)
				{
					_isActive = false;
					_timer = 0;
				}
			}
		}

#pragma endregion

#pragma region 衝突処理

		for (int i = 0; i < MAX; ++i)
		{
			auto& refBrick = bricks[i];

			if (refBrick.intersects(ball))
			{
				// ブロックの上辺または下辺とのぶつかり
				if (refBrick.bottom().intersects(ball) || refBrick.top().intersects(ball))
				{
					ballVec.y *= -1;
				}
				// ブロックの左辺または右辺
				else
				{
					ballVec.x *= -1;
				}

				//　当たったブロックを画面外に移動させる
				refBrick.y -= 600;

				// 同一フレームでは複数のブロックを検知しない
				break;
			}
		}

		// 天井との衝突
		if ((ball.y < 0) && ballVec.y < 0)
		{
			ballVec.y *= -1;
		}

		// ボールが下に落ちた時
		if (ball.y > Scene::Height())
		{
			_state = Finish;
		}

		// 壁との衝突
		if (((ball.x < 0) && ballVec.x < 0) || ((Scene::Width() < ball.x) && (0 < ballVec.x)))
		{
			ballVec.x *= -1;
		}

		// パドルとの衝突
		if ((0 < ballVec.y) && paddle.intersects(ball))
		{
			ballVec = Vec2{(ball.x - paddle.center().x) * 10, -ballVec.y}.setLength(BALL_SPEED);
		}

#pragma endregion


#pragma region 描画処理

		ball.draw();
		paddle.rounded(3).draw();

		for (int i = 0; i < MAX; ++i)
		{
			bricks[i].stretched(-1).draw(HSV{bricks[i].y - 40});
		}

#pragma endregion
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
