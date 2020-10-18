#include <bangtal>
#include <string>
#include <cstdio>
#include <ctime>

using namespace bangtal;
using namespace std;

class Door;
typedef shared_ptr<Door> DoorPtr;

class LockedDoor;
typedef shared_ptr<LockedDoor> LockedDoorPtr;

class Flowerpot;
typedef shared_ptr<Flowerpot> FlowerpotPtr;

class Switch;
typedef shared_ptr<Switch> SwitchPtr;

class Item;
typedef shared_ptr<Item> ItemPtr;

class Key;
typedef shared_ptr<Key> KeyPtr;

class Puzzle;
typedef shared_ptr<Puzzle> PuzzlePtr;

class PicturePuzzle;
typedef shared_ptr<PicturePuzzle> PicturePuzzlePtr;

class SpotDifference;
typedef shared_ptr<SpotDifference> SpotDifferencePtr;

// 아이템 정의
// 주울수 있는 모든 오브젝트가 아이템이다.
class Item : public Object {
public:
	static ItemPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = ItemPtr(new Item(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Item(const string& image, ScenePtr scene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			pick();
		}
		return true;
	}
};

// 열쇠 정의
// 아이템중에 문을 열 수 있는 아이템이 열쇠다.
class Key : public Item {
public:
	static KeyPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = KeyPtr(new Key(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Key(const string& image, ScenePtr scene, int x, int y, bool shown) : Item(image, scene, x, y, shown) {
		this->setScale(0.3f);
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			showMessage("열쇠를 주웠다.");
			Item::onMouse(x, y, action);
		}
		return true;
	}
};

// 문 클래스 정의
// 한 번 클릭하면 문이 열리고 한 번 더 클릭하면 문에 연결된 방에 들어갈 수 있다.
class Door : public Object {
public:
	static DoorPtr create(const string& closedImage, const string& openImage, ScenePtr locatedScene = nullptr, ScenePtr connectedScene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = DoorPtr(new Door(closedImage, openImage, locatedScene, connectedScene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Door(const string& closedImage, const string& openImage, ScenePtr locatedScene, ScenePtr connectedScene, int x, int y, bool shown) : Object(closedImage, locatedScene, x, y, shown) {
		_openImage = openImage;
		_closedImage = closedImage;
		_connectedScene = connectedScene;
	}

public:
	void doorOpen() {
		isClosed = false;
		setImage(_openImage);
	}
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (isClosed == true) {
				doorOpen();
			}
			else {
				_connectedScene->enter();
			}
		}
		return true;
	}

protected:
	bool isClosed = true;
	string _openImage, _closedImage;
	ScenePtr _connectedScene;
};

// 잠긴문 클래스 정의
// 열쇠를 isHanded해야 열 수 있다.
class LockedDoor : public Door {
public:
	static LockedDoorPtr create(const string& closedImage, const string& openImage, ScenePtr locatedScene = nullptr, ScenePtr connectedScene = nullptr, KeyPtr key = nullptr, int x = 0, int y = 0, bool shown = true, bool locked = true) {
		auto object = LockedDoorPtr(new LockedDoor(closedImage, openImage, locatedScene, connectedScene, key, x, y, shown, locked));
		Object::add(object);
		return object;
	}

protected:
	LockedDoor(const string& closedImage, const string& openImage, ScenePtr locatedScene, ScenePtr connectedScene, KeyPtr key, int x, int y, bool shown, bool locked) : Door(closedImage, openImage, locatedScene, connectedScene, x, y, shown) {
		_key = key;
		isLocked = locked;
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (isLocked == true) {
				if (_key->isHanded()) {
					showMessage("찰칵! 문이 열렸다.");
					isLocked = false;
				}
				else {
					showMessage("잠겼다.");
				}
			}
			else {
				Door::onMouse(x, y, action);
			}
		}
		return true;
	}

protected:
	bool isLocked;
	KeyPtr _key;
};

// 화분 클래스 정의
// 화분을 제 위치에 뒀을 때 열쇠가 떨어진다.
class Flowerpot : public Object {
public:
	static FlowerpotPtr create(const string& image, ScenePtr scene = nullptr, KeyPtr key = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = FlowerpotPtr(new Flowerpot(image, scene, key, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Flowerpot(const string& image, ScenePtr scene, KeyPtr key, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		flowerpot_initial_x = x;
		flowerpot_initial_y = y;
		_scene = scene;
		_key = key;
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			// 화분이 올바른 곳에 위치했을 때, 더이상 움직이지 않도록 하기
			if (flowerpot_Onlocation)
				return true;

			// 화분을 드래그할때 상하좌우로 움직이도록 하기
			if (action == MouseAction::MOUSE_DRAG_LEFT) {
				if (flowerpot_initial_x <= 50)
					return true;
				flowerpot_initial_x -= 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_RIGHT) {
				if (flowerpot_initial_x >= 1150)
					return true;
				flowerpot_initial_x += 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_DOWN) {
				if (flowerpot_initial_y <= 50)
					return true;
				flowerpot_initial_y -= 100;
			}
			else if (action == MouseAction::MOUSE_DRAG_UP) {
				if (flowerpot_initial_y >= 150)
					return true;
				flowerpot_initial_y += 100;
			}
			this->locate(_scene, flowerpot_initial_x, flowerpot_initial_y);

			// 화분을 올바른 곳에 옮겼을 때 숨겨진 문 생성
			if (flowerpot_initial_x == flowerpot_Onlocation_x && flowerpot_initial_y == flowerpot_Onlocation_y) {
				flowerpot_Onlocation = true;
				showMessage("딸그락(무언가 떨어지는 소리가 들렸다)");
				_key->show();
			}
		}
		return true;
	}

private:
	int flowerpot_initial_x;			// 화분의 초기 위치
	int flowerpot_initial_y;
	int flowerpot_Onlocation = false;	// 화분이 올바른 곳에 있는지를 저장하는 플래그
	int flowerpot_Onlocation_x = 1050;	// 화분의 올바른 위치의 좌표
	int flowerpot_Onlocation_y = 50;
	ScenePtr _scene;
	KeyPtr _key;
};

// 스위치 정의
// 불을 끄면 힌트가 나온다.
class Switch : public Object {
public:
	static SwitchPtr create(const string& image, ScenePtr scene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = SwitchPtr(new Switch(image, scene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Switch(const string& image, ScenePtr scene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		_scene = scene;
		this->setScale(3.0f);
		flowerpot_hint = Object::create("images/화분-힌트.png", scene, 1120, 50, false);
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			if (lighted) {
				_scene->setLight(0.2f);
				flowerpot_hint->show();
				lighted = false;
			}
			else {
				_scene->setLight(1.0f);
				flowerpot_hint->hide();
				lighted = true;
			}
		}
		return true;
	}

private:
	ScenePtr _scene;
	bool lighted = true;
	ObjectPtr flowerpot_hint;
};

// 퍼즐 정의
// 사진 퍼즐 또는 틀린그림찾기가 퍼즐 클래스이다.
// 퍼즐 클래스는 클릭시 다른 씬으로 전환되며(이때 대화창 인벤창 없어짐) 이전 씬으로 가는 뒤로가기 버튼이 공통적으로 있다.
class Puzzle : public Object {
	static PuzzlePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = PuzzlePtr(new Puzzle(image, scene, connectedScene, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	Puzzle(const string& image, ScenePtr scene, ScenePtr connectedScene, int x, int y, bool shown) : Object(image, scene, x, y, shown) {
		_connectedScene = connectedScene;
		prev_scene = scene;

		// 뒤로가기 버튼 만들기
		auto backward_button = Object::create("images/back.png", _connectedScene, 220, 670, true);
		backward_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, true);
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, true);
			prev_scene->enter();
			return true;
		});
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
			_connectedScene->enter();
		}
		return true;
	}

protected:
	ScenePtr _connectedScene;
	ScenePtr prev_scene;
};


// 사진 퍼즐 정의
// 사진 퍼즐을 풀면 숨겨진 퍼즐이 생긴다.
class PicturePuzzle : public Puzzle {
public:
	static PicturePuzzlePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, PuzzlePtr secretPuz = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = PicturePuzzlePtr(new PicturePuzzle(image, scene, connectedScene, secretPuz, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	PicturePuzzle(const string& image, ScenePtr scene, ScenePtr connectedScene, PuzzlePtr secretPuz, int x, int y, bool shown) : Puzzle(image, scene, connectedScene, x, y, shown) {
		_secretPuz = secretPuz;
		init_puzzles();
		//shuffle();
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			Puzzle::onMouse(x, y, action);
		}
		return true;
	}

private:
	PuzzlePtr _secretPuz;

	static const int num_of_puzzles = 9;	// 퍼즐 총 개수
	const int puzzle_size = 200;			// 픽셀
	const int invisible_puzzle_num = 8;		// 안보일 퍼즐 숫자
	const int x_0 = 374, y_0 = 465;			// 퍼즐 좌표
	const int x_1 = x_0 + puzzle_size, y_1 = y_0 - puzzle_size;
	const int x_2 = x_1 + puzzle_size, y_2 = y_1 - puzzle_size;

	struct Position {
		int x, y;
	};

	// 퍼즐 각 좌표를 저장하기
	Position position[num_of_puzzles] = {
		{ x_0, y_0 },
		{ x_1, y_0 },
		{ x_2, y_0 },
		{ x_0, y_1 },
		{ x_1, y_1 },
		{ x_2, y_1 },
		{ x_0, y_2 },
		{ x_1, y_2 },
		{ x_2, y_2 }
	};

	// 퍼즐 하나의 구조체
	struct Puzz {
		Position pos;
		ObjectPtr puzObj;
	};

	// 퍼즐 목록
	Puzz puzzles[num_of_puzzles];

	// 퍼즐이 빈칸과 붙어있는지를 리턴하는 함수
	bool isAdjacent(Position puz_pos, Position invisible_pos) {
		// 빈칸과 붙어 있다고 알려주는 플래그
		bool adj_flag = false;

		// 클릭된 퍼즐 오른쪽에 빈칸이 있으면
		if (puz_pos.x + puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
			adj_flag = true;
		}
		// 클릭된 퍼즐 왼쪽에 빈칸이 있으면
		else if (puz_pos.x - puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
			adj_flag = true;
		}
		// 클릭된 퍼즐 위에 빈칸이 있으면
		else if (puz_pos.y + puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
			adj_flag = true;
		}
		// 클릭된 퍼즐 아래에 빈칸이 있으면
		else if (puz_pos.y - puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
			adj_flag = true;
		}

		return adj_flag;
	}

	// 퍼즐의 위치를 화면상에서 바꾸는 함수
	void puzzle_swap(Puzz puzzle, Puzz invisible)
	{
		puzzle.puzObj->locate(_connectedScene, puzzle.pos.x, puzzle.pos.y);
		invisible.puzObj->locate(_connectedScene, invisible.pos.x, invisible.pos.y);
	}

	// 퍼즐의 좌표를 바꾸는 함수
	void position_swap(Puzz& puzzle, Puzz& invisible) {
		int tx = puzzle.pos.x;
		int ty = puzzle.pos.y;

		puzzle.pos.x = invisible.pos.x;
		puzzle.pos.y = invisible.pos.y;

		invisible.pos.x = tx;
		invisible.pos.y = ty;
	}

	// 모든 퍼즐들이 제자리에 있는지 확인
	bool check_alright()
	{
		for (int i = 0; i < num_of_puzzles; i++) {
			if (!(puzzles[i].pos.x == position[i].x && puzzles[i].pos.y == position[i].y)) {
				return false;
			}
		}
		return true;
	}

	void init_puzzles() {
		// 저장한 좌표로 퍼즐조각 객체를 만듦
		for (int i = 0; i < num_of_puzzles; i++) {
			bool visi = true;
			if (i == invisible_puzzle_num)
				visi = false;

			string path = "images/puzzles/" + to_string(i) + ".jpg";
			puzzles[i].puzObj = Object::create(path, _connectedScene, position[i].x, position[i].y, visi);
			puzzles[i].pos.x = position[i].x;
			puzzles[i].pos.y = position[i].y;

			// 각 퍼즐의 콜백 적용
			puzzles[i].puzObj->setOnMouseCallback([&, i](ObjectPtr object, int x, int y, MouseAction action)->bool {
				if (object == puzzles[i].puzObj)
					if (isAdjacent(puzzles[i].pos, puzzles[invisible_puzzle_num].pos)) {
						position_swap(puzzles[i], puzzles[invisible_puzzle_num]);
						puzzle_swap(puzzles[i], puzzles[invisible_puzzle_num]);
					}

				if (check_alright()) {
					showMessage("퍼즐이 완성되자 암흑속에서 무언가가 튀어나왔다.");
					_secretPuz->show();
					for (int i = 0; i < num_of_puzzles; i++)
						puzzles[i].puzObj->setOnMouseCallback(nullptr);
				}
				return true;
			});
		}
	}

	// 퍼즐 섞기
	void shuffle() {
		srand((unsigned int)time(NULL));
		int random;

		for (int i = 0; i < num_of_puzzles; i++) {
			random = rand() % num_of_puzzles;
			position_swap(puzzles[i], puzzles[random]);
			puzzle_swap(puzzles[i], puzzles[random]);
		}
	}
};

// 틀린그림 찾기 퍼즐 정의
// 틀린그림을 풀면 옆에 숨겨진 문이 나온다.
class SpotDifference : public Puzzle {
public:
	static SpotDifferencePtr create(const string& image, ScenePtr scene = nullptr, ScenePtr connectedScene = nullptr, DoorPtr secretDoor = nullptr, int x = 0, int y = 0, bool shown = true) {
		auto object = SpotDifferencePtr(new SpotDifference(image, scene, connectedScene, secretDoor, x, y, shown));
		Object::add(object);
		return object;
	}

protected:
	SpotDifference(const string& image, ScenePtr scene, ScenePtr connectedScene, DoorPtr secretDoor, int x, int y, bool shown) : Puzzle(image, scene, connectedScene, x, y, shown) {
		_secretDoor = secretDoor;
		initGame();
	}

public:
	virtual bool onMouse(int x, int y, MouseAction action) {
		if (!Object::onMouse(x, y, action)) {
			Puzzle::onMouse(x, y, action);
		}
		return true;
	}

private:
	DoorPtr _secretDoor;
	ObjectPtr problem;

	class Rect {
	public:
		int centerX, centerY, radius;

		Rect(int cx, int cy, int r) : centerX(cx), centerY(cy), radius(r) {};

		bool checkIn(int x, int y) {
			return (x > centerX - radius) && (x < centerX + radius) && (y > centerY - radius) && (y < centerY + radius);
		}
	};

	static const int num_of_difference = 7;
	const int check_size = 25;
	int count = 0;

	Rect left_rects[num_of_difference] = {
		{ 568, 594, 54 },
		{ 99, 551, 17 },
		{ 383, 482, 16 },
		{ 401, 158, 27 },
		{ 61, 255, 36 },
		{ 592, 421, 35 },
		{ 320, 117, 13 },
	};
	Rect right_rects[num_of_difference] = {
		{ 1186, 594, 54 },
		{ 717, 551, 17 },
		{ 1001, 482, 16 },
		{ 1019, 158, 27 },
		{ 679, 255, 36 },
		{ 1210, 421, 35 },
		{ 938, 117, 13 },
	};

	ObjectPtr left_checks[num_of_difference];
	ObjectPtr right_checks[num_of_difference];

	void initGame() {
		problem = Object::create("images/spot/problem.png", _connectedScene);

		for (int i = 0; i < num_of_difference; i++) {
			left_checks[i] = Object::create("images/spot/check.png", _connectedScene, left_rects[i].centerX - check_size, left_rects[i].centerY - check_size, false);
			right_checks[i] = Object::create("images/spot/check.png", _connectedScene, right_rects[i].centerX - check_size, right_rects[i].centerY - check_size, false);
		}

		problem->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			for (int i = 0; i < num_of_difference; i++) {
				if (left_rects[i].checkIn(x, y) || right_rects[i].checkIn(x, y)) {
					left_checks[i]->show();
					right_checks[i]->show();
					count++;
				}
			}

			if (count == num_of_difference) {
				showMessage("(퍼즐이 풀리자 옆에서 이상한 소리가 난다)");
				problem->hide();
				_secretDoor->doorOpen();
				_secretDoor->show();
			}
			return true;
		});
	}
};

ScenePtr initGame()
{
	// 씬 초기화
	auto start_room = Scene::create("시작 방", "images/배경-1.png");
	auto right_room = Scene::create("오른쪽 방", "images/배경-2.png");
	auto left_room = Scene::create("왼쪽 방", "images/배경-3.png");
	auto puzzle_scene = Scene::create("사진 퍼즐", "images/kakao_cut.jpg");
	auto spot_scene = Scene::create("틀린 그림 찾기", "images/spot/problem.png");
	auto escape = Scene::create("탈출", "images/탈출.jpg");

	// 열쇠
	auto left_room_key = Key::create("images/열쇠.png", left_room, 1000, 100, false);

	// 시작 방 객체 초기화
	auto start_room_rightDoor = LockedDoor::create("images/문-오른쪽-닫힘.png", "images/문-오른쪽-열림.png", start_room, right_room, left_room_key, 800, 270, true, true);
	auto start_room_leftDoor = Door::create("images/문-왼쪽-닫힘.png", "images/문-왼쪽-열림.png", start_room, left_room, 100, 260, true);
	auto flowerpot = Flowerpot::create("images/화분.png", start_room, left_room_key, 550, 150, true);

	// 왼쪽 방 객체 초기화
	auto left_room_rightDoor = Door::create("images/문-오른쪽-닫힘.png", "images/문-오른쪽-열림.png", left_room, start_room, 900, 250, true);
	left_room_rightDoor->doorOpen();
	auto left_room_switch = Switch::create("images/스위치.png", left_room, 200, 440, true);

	auto left_room_leftDoor = Door::create("images/문-왼쪽-닫힘.png", "images/문-왼쪽-열림.png", left_room, escape, 260, 290, false);
	left_room_leftDoor->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
		setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
		showMessage("당신은 탈출하셨습니다.");
		return false;
	});

	auto spot_mini = SpotDifference::create("images/틀린퍼즐.png", left_room, spot_scene, left_room_leftDoor, 100, 420, false);


	// 오른쪽 방 객체 초기화
	auto right_room_leftDoor = Door::create("images/문-왼쪽-닫힘.png", "images/문-왼쪽-열림.png", right_room, start_room, 320, 270, true);
	right_room_leftDoor->doorOpen();
	auto puzzle_mini = PicturePuzzle::create("images/사진퍼즐.png", right_room, puzzle_scene, spot_mini, 800, 400, true);

	return start_room;
}

int main(void)
{
	startGame(initGame());
	return 0;
}